/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2023-2024 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "fsl_iocon.h"
#include <stdbool.h>
#include <stdio.h>
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

static uint32_t tic(void)
{
    SysTick_Config(0x00FFFFFF);
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    return SysTick->VAL;
}

static uint32_t toc(uint32_t tic_ticks)
{
    return tic_ticks - SysTick->VAL;
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
    uint8_t buf[3];
    float voltages[7];
    int32_t adc_res;
    float sINT24;
    float gains[7];
    uint32_t ticks[7];
    float time;
    float time_tot;

    /* Init board hardware. */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    //CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    /* Init board hardware. */
    /* attach 12 MHz clock to FLEXCOMM0 (debug console) */
    CLOCK_AttachClk(BOARD_DEBUG_UART_CLK_ATTACH);

    DMA_Init(DMA0);
    INPUTMUX_Init(INPUTMUX);
    BOARD_InitPins();
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

    // Ch0
    NAFESDK_CmdSetCurrentPointer(&instance,0); // Set Pointer to ch0
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x1711); // AI1P to AICOM - GAIN 0.2x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x28); // SINC4 - 12000Sps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x4400); // Delay 16.493us
    gains[0]=0.2;

    // Ch1
    NAFESDK_CmdSetCurrentPointer(&instance,1); // Set Pointer to ch1
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x7131); // AI1N to AICOM - GAIN 0.4x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x1028); // SINC4 - 12000Sps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x4400); // Delay 16.493us
    gains[1]=0.4;

    // Ch2
    NAFESDK_CmdSetCurrentPointer(&instance,2); // Set Pointer to ch2
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x2711); // AI2P to AICOM - GAIN 0.2x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x18);  // SINC4 - 24000Sps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x4400); // Delay 16.493us
    gains[2]=0.2;

    // Ch3
    NAFESDK_CmdSetCurrentPointer(&instance,3); // Set Pointer to ch3
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x2731); // AI1N to AICOM - GAIN 0.4x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x1018); // SINC4 - 24000Sps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x4400); // Delay 16.493us
    gains[3]=0.4;

    // Ch4
    NAFESDK_CmdSetCurrentPointer(&instance,4); // Set Pointer to ch4
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x3731); // AI3P to AICOM - GAIN 0.4x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x1038); // SINC4 - 6000Sps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x5000); // Delay 33.4us
    gains[4]=0.4;

    // Ch5
    NAFESDK_CmdSetCurrentPointer(&instance,5);
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x7391); // AI3N to AICOM - GAIN 2x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x4038); // SINC4 - 6000ps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x5000); // Delay 33.4us
    gains[5]=2;

    // Ch6
    NAFESDK_CmdSetCurrentPointer(&instance,6);
    NAFESDK_WriteReg16B(&instance, CH_CONFIG0, 0x44F1); // AI4P to AI4N - GAIN 16x
    NAFESDK_WriteReg16B(&instance, CH_CONFIG1, 0x70A1); // SINC4+1 - 50ps
    NAFESDK_WriteReg16B(&instance, CH_CONFIG2, 0x4400); // Delay 16.493us
    gains[6]=16;

    while(1)
    {
    	PRINTF("\r\n->Click any button to start one Conversion sequences<-\r\n");
    	GETCHAR(); // Wait a keyboard import to start a conversion sequence.

    	for (int i=0;i<7;i++){
			NAFESDK_CmdSetCurrentPointer(&instance,i); // Set Pointer to ch i (0 -> 6)
			ticks[i] = tic(); // Start counter to monitor the duration for each conversion
			startScsr(&instance); // Start a single channel conversion (cmd ID 0x2000)

			while(GPIO_PinRead(GPIO, instance.gpio_rdy_port, instance.gpio_rdy_pin)==0); /* wait for RDY pin to high */
			ticks[i] = toc(ticks[i]); // Stop counter to monitor the duration for each conversion
			NAFESDK_SPIRead(&instance, buf, 3); // Read a 3 Byte (24 bit) ADC conversion


			adc_res = (buf[0]<<16) + (buf[1]<<8) + (buf[2]<<0); // convert 24 bit result into int32_t format
			sINT24 = ADC_READING_TO_sINT24(adc_res); //Apply Conversion to 24 bit Integer (see datasheet)
			voltages[i] = sINT24_TO_VG(sINT24, gains[i]); //Apply Conversion from 24 bit to voltage value
    	}

    	PRINTF("------------Conversion-------------\r\n");
    	PRINTF("\r\nCh0:%6fV, Ch1:%0.6fV, Ch2:%6fV, Ch3:%6fV, Ch4:%6fV, Ch5:%6fV, Ch6:%6fV\r\n", voltages[0],voltages[1],voltages[2],voltages[3],voltages[4],voltages[5],voltages[6]);
    	time_tot = 0;
    	for (int i=0;i<7;i++){
    		time = (ticks[i]) / (SystemCoreClock / 1000.F);
    		PRINTF("Elapsed Time for ch%d = %3fms\r\n", i ,time);
    		time_tot += time;
    	}
    	PRINTF("Total Elapsed Time = %3fms\r\n", time_tot);
    	PRINTF("\r\n-----------------------------------\r\n");
    }

}



