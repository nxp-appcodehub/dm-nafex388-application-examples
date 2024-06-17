/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NAFESDK_H_
#define _NAFESDK_H_


#include <NAFESDKregs.h>
#include "fsl_spi.h"
#include "fsl_gpio.h"
#include "fsl_dma.h"
#include "fsl_pint.h"
#include "pin_mux.h"
#include "fsl_inputmux.h"
#include "fsl_ctimer.h"
#include "ring_buffer.h"


#define ADC_READ_SIZE                       (3)     /* 24-bit samples */
#define MAX_NUM_ADC_READS_PER_DMA_INT       (16)    /* sample per DMA interrupt */

typedef struct
{
    SPI_Type            *SPIx;
    uint32_t            gpio_cs_port;
    uint32_t            gpio_cs_pin;
    uint32_t            gpio_rdy_port;
    uint32_t            gpio_rdy_pin;
    uint32_t            addr;
    uint32_t            spi_clk_frq;
    uint32_t            spi_dma_tx_chl;
    uint32_t            spi_dma_rx_chl;
    dma_handle_t        s_spi_dma_handleTx;
    dma_handle_t        s_spi_dma_handleRx;
    spi_transfer_t      s_spi_m_xfer;
    uint8_t             s_apCmd[2];
    RINGBUFF_T          *pAdcRingBuf;                                                       /* Size must be power of 2 */
    uint8_t             s_adcPingPongBuf[2][MAX_NUM_ADC_READS_PER_DMA_INT * ADC_READ_SIZE];             /* data buf for DMA use */
    volatile int        s_xferIrqsRemaining;                                                            /* <0: infinite mode, ==0: inactive, >0: #irqs remaining */
    volatile int        s_adcReadingsRemaining;
    volatile int        s_adcReadingsPerIrq;
    DMA_ALLOCATE_LINK_DESCRIPTORS(s_nextDescTx, MAX_NUM_ADC_READS_PER_DMA_INT);                         /* Chained descriptors for tx */
    DMA_ALLOCATE_LINK_DESCRIPTORS(s_nextDescRxPing, MAX_NUM_ADC_READS_PER_DMA_INT);                     /* Chained descriptors for rx */
    DMA_ALLOCATE_LINK_DESCRIPTORS(s_nextDescRxPong, MAX_NUM_ADC_READS_PER_DMA_INT);                     /* Chained descriptors for rx */
}nafe_t;


void NAFESDK_Init(nafe_t* instance, uint32_t spiClkFreq);
void NAFESDK_WriteReg16B(nafe_t *instance, uint8_t reg_addr, uint16_t val);
uint16_t NAFESDK_ReadReg16B(nafe_t *instance, uint8_t reg_addr);
void NAFESDK_CmdAbort(nafe_t *instance);
void NAFESDK_CmdReset(nafe_t *instance);
void NAFESDK_SPIRead(nafe_t *instance, uint8_t *buf, uint32_t len);
void NAFESDK_SendCmd(nafe_t *instance, uint16_t cmd, uint8_t dir, uint8_t *buf, uint32_t len);
void NAFESDK_CmdSetCurrentPointer(nafe_t *instance, uint8_t adc_chl);

void startScsr(nafe_t *instance);
void startSccr(nafe_t *instance, int numReads);
void startMccr(nafe_t *instance, int numReads, uint16_t channelMask, bool drdybPinSeq);
void startMcmr(nafe_t *instance, uint16_t channelMask, bool drdybPinSeq);

uint32_t NAFESDK_GetReadingsRemaining(nafe_t *instance);
uint32_t NAFESDK_GetRBCount(nafe_t *instance);
void NAFESDK_ReadFromRB(nafe_t *instance, uint8_t *buf, uint32_t len);
    
#endif



