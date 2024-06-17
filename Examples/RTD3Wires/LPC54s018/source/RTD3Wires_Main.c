/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_iocon.h"
#include <stdbool.h>
#include "NAFESDK.h"
#include <math.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SPI_MOSI_PORT                       (3)
#define SPI_MOSI_PIN                        (21)
#define SPI_MISO_PORT                       (3)
#define SPI_MISO_PIN                        (22)
#define SPI_CLK_PORT                        (3)
#define SPI_CLK_PIN                         (20)
#define NAFE_CS_PORT                        (3)
#define NAFE_CS_PIN                         (30)
#define NAFE_DATA_RDY_PORT                  (1)
#define NAFE_DATA_RDY_PIN                   (18)
#define NAFE_DATA_SYNC_PORT                 (0)
#define NAFE_DATA_SYNC_PIN                  (24)


#define SPI_CLK_FRQ                         (8*1000*1000)
#define SPI_INSTACE                         (SPI9)
#define NAFE_ADDR                           (0)

#define ADC_READING_TO_sINT24(code)         ((code + 8388608)%16777216)-8388608
#define sINT24_TO_VG(sINT24, Gain)          (10.F/16777216)*(sINT24/Gain)
#define ADC_RATE_SELECT                     (7)

 /*******************************************************************************
 * Variables
 ******************************************************************************/
static nafe_t       instance;
static uint8_t      s_adcBuf[64 * 1024]; 
static RINGBUFF_T   s_adcRingBuf;
 /*******************************************************************************
 * Code
 ******************************************************************************/
 
static void delay(uint32_t ms)
{
    volatile int i,j;
    for(i=0; i<ms; i++)
    {
        for(j=0; j<10000; j++)
        {
            __NOP();
        }
    }
}



static void pinmux_init(void)
{
    CLOCK_EnableClock(kCLOCK_Iocon);
    CLOCK_EnableClock(kCLOCK_Gpio0);
    CLOCK_EnableClock(kCLOCK_Gpio1);
    CLOCK_EnableClock(kCLOCK_Gpio2);
    CLOCK_EnableClock(kCLOCK_Gpio3);
    
    IOCON_PinMuxSet(IOCON, SPI_MOSI_PORT, SPI_MOSI_PIN, IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF); /* MOSI */
    IOCON_PinMuxSet(IOCON, SPI_MISO_PORT, SPI_MISO_PIN, IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF); /* MISO */
    IOCON_PinMuxSet(IOCON, SPI_CLK_PORT, SPI_CLK_PIN, IOCON_FUNC1 | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF); /* SCK */
    
    /* Set READY pin to be input */
    GPIO->DIRCLR[NAFE_DATA_RDY_PORT] = 1 << NAFE_DATA_RDY_PIN;
    IOCON_PinMuxSet(IOCON, NAFE_DATA_RDY_PORT, NAFE_DATA_RDY_PIN, IOCON_FUNC0 | IOCON_MODE_INACT | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    
    /* Set SYNC to be output */
    GPIO->DIRSET[NAFE_DATA_SYNC_PORT] = 1 << NAFE_DATA_SYNC_PIN;
    IOCON_PinMuxSet(IOCON, NAFE_DATA_SYNC_PORT, NAFE_DATA_SYNC_PIN, IOCON_FUNC0  | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    GPIO_PinWrite(GPIO, NAFE_DATA_SYNC_PORT, NAFE_DATA_SYNC_PIN, 0);
    
    /* set CS pin to be output */
    GPIO->DIRSET[NAFE_CS_PORT] = 1 << NAFE_CS_PIN; /* SSEL */
    IOCON_PinMuxSet(IOCON, NAFE_CS_PORT, NAFE_CS_PIN, IOCON_FUNC0  | IOCON_DIGITAL_EN | IOCON_INPFILT_OFF);
    GPIO_PinWrite(GPIO, NAFE_CS_PORT, NAFE_CS_PIN, 1);
}


int main(void)
{
 	const int conversionNumber=100;
 	const float R0=100;
 	const float A=3.908e-3;
 	const float ISource=0.750e-3;

 	int i;
	int len;
    uint8_t buf[3];
    float voltage;
    int32_t adc_res;
    float sINT24;
    float gain;

    /* Init board hardware. */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    DMA_Init(DMA0);
    INPUTMUX_Init(INPUTMUX);
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitDebugConsole();


    CLOCK_AttachClk(kFRO_HF_to_FLEXCOMM9);
    RESET_PeripheralReset(kFC9_RST_SHIFT_RSTn);
    
    /* Setup DMA */
    DMA_EnableChannel(DMA0, kDmaRequestFlexcomm9Tx);
    DMA_EnableChannel(DMA0, kDmaRequestFlexcomm9Rx);
    DMA_SetChannelPriority(DMA0, kDmaRequestFlexcomm9Tx, kDMA_ChannelPriority3);
    DMA_SetChannelPriority(DMA0, kDmaRequestFlexcomm9Rx, kDMA_ChannelPriority2);
    
    pinmux_init();
    
    PINT_Init(PINT);
    PINT_PinInterruptConfig(PINT, kPINT_PinInt0, kPINT_PinIntEnableLowLevel, NULL);
    INPUTMUX_AttachSignal(INPUTMUX, kPINT_PinInt0, kINPUTMUX_GpioPort1Pin18ToPintsel);

    /* Initialize ADC ringbuffer */
    RingBuffer_Init(&s_adcRingBuf, &s_adcBuf, 1, sizeof(s_adcBuf));
    
    /* setup all resource for NAFE driver */
    instance.spi_clk_frq = SPI_CLK_FRQ;
    instance.SPIx = SPI_INSTACE;
    instance.addr = NAFE_ADDR;
    instance.gpio_cs_port = NAFE_CS_PORT;
    instance.gpio_cs_pin = NAFE_CS_PIN;
    instance.gpio_rdy_port = NAFE_DATA_RDY_PORT;
    instance.gpio_rdy_pin = NAFE_DATA_RDY_PIN;
    instance.spi_dma_tx_chl = kDmaRequestFlexcomm9Tx;
    instance.spi_dma_rx_chl = kDmaRequestFlexcomm9Rx;
    instance.pAdcRingBuf = &s_adcRingBuf;

    NAFESDK_Init(&instance, CLOCK_GetFroHfFreq());
    NAFESDK_CmdReset(&instance);
    delay(10);

    // RTD Sensing and Forcing
    NAFESDK_CmdSetCurrentPointer(&instance,0); // Set Pointer to ch0
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x11B1); // AI1P to AI1N - GAIN 4x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x5079); // SINC4+1 - 1125Sps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x2E80); // Delay 16.5us
    NAFESDK_WriteReg16B(&instance, CH_CONFIG3, 0xB009); // Current - Positive Pol - 750uA - AI1P + AI1N
    gain=4;


    while(1)
    {
    	PRINTF("\r\n->Click any button to start one Conversion sequences<-\r\n");
    	GETCHAR(); // Wait a keyboard import to start a conversion sequence.

		NAFESDK_CmdSetCurrentPointer(&instance,0); // Set Pointer to ch0
		startSccr(&instance,conversionNumber); // Start a Single Channel Conversion, in order to get "conversionNumber" results
	    while(NAFESDK_GetReadingsRemaining(&instance) != 0) {}; // Wait for conversion end.

		voltage = 0; // Initialize result variable
	    len = NAFESDK_GetRBCount(&instance); // get length of buffer
	    // convert sample to voltage and sum it in order to perform an average
	    for(i=0; i<len / ADC_READ_SIZE; i++)
	    {
	        NAFESDK_ReadFromRB(&instance, buf, 3);
	        adc_res = (buf[0]<<16) + (buf[1]<<8) + (buf[2]<<0); // convert 24 bit result into int32_t format
			sINT24 = ADC_READING_TO_sINT24(adc_res); //Apply Conversion to 24 bit Integer (see datasheet)
			voltage += sINT24_TO_VG(sINT24, gain); //Apply Conversion from 24 bit to voltage value
	    }
	    voltage = voltage/conversionNumber; //Average Calculation
    	PRINTF("------------Conversion-------------\r\n");
    	PRINTF("\r\nVoltage RTD=%6fV, Temperature RTD=%3fC\r\n", voltage,((voltage/ISource)-R0)/(A*R0)); //Calculation of Temperature.
    	PRINTF("\r\n-----------------------------------\r\n");
    }

}



