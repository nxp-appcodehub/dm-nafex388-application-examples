/*
 * Copyright (c) 2013 - 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "NAFESDK.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define SSEL_HIGH(instance)     GPIO_PortSet(GPIO, instance->gpio_cs_port, 1 << instance->gpio_cs_pin)
#define SSEL_LOW(instance)      GPIO_PortClear(GPIO, instance->gpio_cs_port, 1 << instance->gpio_cs_pin)

#define CS_LOW                              (0)
#define CS_HIGH                             (1)

typedef enum
{
    kAnnapurnaCmd_Scsr = 0x2000,
    kAnnapurnaCmd_Sccr = 0x2001,
    kAnnapurnaCmd_Mcmr = 0x2002,
    kAnnapurnaCmd_Mccr = 0x2003,
} annapurna_cmd_t;


/*******************************************************************************
 * Variables
 ******************************************************************************/

static uint8_t s_spiTxDummy = 0x00;
static uint8_t s_spiRxDummy;


/*******************************************************************************
 * Code
 ******************************************************************************/
 
/*!
 * get sample remaining of DMA ping pong buffer.
 *
 * param instance instance of NAFE device
 * retval number of sample remaining 
 */
uint32_t NAFESDK_GetReadingsRemaining(nafe_t *instance)
{
    return instance->s_adcReadingsRemaining;
}

/*!
 * get data remaining of ring buffer
 *
 * param instance instance of NAFE device
 * retval number of sample remaining in ring buffer
 */
uint32_t NAFESDK_GetRBCount(nafe_t *instance)
{
    return RingBuffer_GetCount(instance->pAdcRingBuf);
}

/*!
 * read data in ring buffer
 *
 * param instance instance of NAFE device
 * param buf buffer pointer
 * param len length to read
 */
void NAFESDK_ReadFromRB(nafe_t *instance, uint8_t *buf, uint32_t len)
{
    RingBuffer_PopMult(instance->pAdcRingBuf, buf, len);
}

/*!
 * Initializes of LPC NAFESDK driver
 *
 * param instance instance of NAFE device
 * param spiClkFreq LPC spi clock input frq
 */
void NAFESDK_Init(nafe_t* instance, uint32_t spiClkFreq)
{
    spi_master_config_t masterConfig;

    SSEL_HIGH(instance);

    /* Setup SPI */
    SPI_MasterGetDefaultConfig(&masterConfig);
    masterConfig.sselNum = (spi_ssel_t)0; /* SPI peripheral SSEL not used, as this is done with GPIOs */
    masterConfig.polarity = kSPI_ClockPolarityActiveHigh;
    masterConfig.phase = kSPI_ClockPhaseSecondEdge;
    masterConfig.baudRate_Bps = instance->spi_clk_frq;
    SPI_MasterInit(instance->SPIx, &masterConfig, spiClkFreq);


}

/*!
 * spi polling transfer
 *
 * param instance instance of NAFE device
 * param tx_buf: send buffer pointer
 * param rx_buf: receive buffer pointer
 * param len: length of transfer
 * param cs: keep cs low or release cs pin
 */
static void _spi_polling_transfer(nafe_t *instance, uint8_t *tx_buf, uint8_t *rx_buf, uint32_t len, uint8_t cs)
{
    spi_transfer_t xfer;
    xfer.txData = tx_buf;
    xfer.rxData = rx_buf;
    xfer.dataSize = len;
    xfer.configFlags = 0;
    
    SSEL_LOW(instance);
    SPI_MasterTransferBlocking(instance->SPIx, &xfer);
    if(cs)
    {
        SSEL_HIGH(instance);
    }
}

/*!
 * send command to NAFE
 *
 * param instance instance of NAFE device
 * param cmd: 16 bit command or register addr
 * param dir: 0:WR, 1:RD
 * param buf: buffer pointer
 * param len: len of the buffer
 */
void NAFESDK_SendCmd(nafe_t *instance, uint16_t cmd, uint8_t dir, uint8_t *buf, uint32_t len)
{
    uint8_t tx_buf[2];
    uint16_t cmdWord = SPI_CMD_WORD(instance->addr, cmd, dir);
    tx_buf[0] = (cmdWord >> 8) & 0xFF;
    tx_buf[1] = (cmdWord >> 0) & 0xFF;
    if(len)
    {
        _spi_polling_transfer(instance, tx_buf, NULL, 2, CS_LOW);
        if(dir)
            _spi_polling_transfer(instance, NULL, buf, len, CS_HIGH);
        else
            _spi_polling_transfer(instance, buf, NULL, len, CS_HIGH);
    }
    else
    {
        _spi_polling_transfer(instance, tx_buf, NULL, 2, CS_HIGH);
    }
}


/*!
 * read NAFE data via SPI bus
 *
 * param instance instance of NAFE device
 * param buf: buffer pointer
 * param len: len of the buffer
 */
void NAFESDK_SPIRead(nafe_t *instance, uint8_t *buf, uint32_t len)
{
    _spi_polling_transfer(instance, NULL, buf, len, CS_HIGH);
}


/*!
 * write NAFE 16 bit register
 *
 * param instance instance of NAFE device
 * param reg_addr: 16 bit register addr
 * param val: value to be written
 */
void NAFESDK_WriteReg16B(nafe_t *instance, uint8_t reg_addr, uint16_t val)
{
    uint8_t tx_buf[2];
    tx_buf[0] = (val >> 8) & 0xFF;
    tx_buf[1] = (val >> 0) & 0xFF;
    NAFESDK_SendCmd(instance, reg_addr, CMD_WR, tx_buf, 2);
}


/*!
 * read NAFE 16 bit register
 *
 * param instance instance of NAFE device
 * param reg_addr: 16 bit register addr
 * retval the 16 bit register value
 */
uint16_t NAFESDK_ReadReg16B(nafe_t *instance, uint8_t reg_addr)
{
    uint8_t rx_buf[2];
    
    NAFESDK_SendCmd(instance, reg_addr, CMD_RD, rx_buf, 2);
    return (rx_buf[0]<<8) + rx_buf[1];
}

/*!
 * send set current pointer command
 *
 * param instance instance of NAFE device
 * param adc_chl: chl to be selected
 */
void NAFESDK_CmdSetCurrentPointer(nafe_t *instance, uint8_t adc_chl)
{
    NAFESDK_SendCmd(instance, (adc_chl & 0x0F), CMD_WR, NULL, 0);
}

/*!
 * send conversion abort command
 *
 * param instance instance of NAFE device
 */
void NAFESDK_CmdAbort(nafe_t *instance)
{
    NAFESDK_SendCmd(instance, CMD_ABORT, CMD_WR, NULL, 0);
}

/*!
 * send conversion reset command
 *
 * param instance instance of NAFE device
 */
void NAFESDK_CmdReset(nafe_t *instance)
{
    NAFESDK_SendCmd(instance, CMD_RESET, CMD_WR, NULL, 0);
}


    
static status_t prepareSpiDmaTransfer(SPI_Type *base,
                                      dma_handle_t *txHandle,
                                      dma_handle_t *rxHandle,
                                      void *nextDescTx,
                                      void *nextDescRx,
                                      spi_transfer_t *xfer,
                                      bool generateIntOnFirstSpiXfer)
{
    assert(!(NULL == xfer));

    status_t result = kStatus_Success;
    spi_config_t *spi_config_p;
    uint32_t srcAddr;
    uint32_t dstAddr;
    int bufferInc;

    if (NULL == xfer)
    {
        return kStatus_InvalidArgument;
    }

    /* Byte size is zero. */
    assert(!(xfer->dataSize == 0));
    if (xfer->dataSize == 0U)
    {
        return kStatus_InvalidArgument;
    }

    /* PINT0 to trigger DMA */
    INPUTMUX_AttachSignal(INPUTMUX, txHandle->channel, kINPUTMUX_PinInt0ToDma);

    dma_channel_config_t chConfig = {0};
    spi_config_p = (spi_config_t *)SPI_GetConfig(base);

    /* receive */
    SPI_EnableRxDMA(base, true);
    srcAddr = (uint32_t)&base->FIFORD;
    dstAddr = (uint32_t)((xfer->rxData) ? xfer->rxData : &s_spiRxDummy);
    bufferInc = (xfer->rxData) ? 1 : 0;
    DMA_PrepareChannelTransfer(
        &chConfig, (void *)srcAddr, (void *)dstAddr,
        DMA_CHANNEL_XFER(1 /* reload */, 0 /* clrTrig */, generateIntOnFirstSpiXfer /* intA */, 0 /* intB */,
                         1 /* width */, 0 /* srcInc */, bufferInc /* dstInc */, xfer->dataSize /* bytes */),
						 kDMA_PeripheralToMemory, &(dma_channel_trigger_t){kDMA_NoTrigger, kDMA_SingleTransfer, kDMA_NoWrap},
        nextDescRx);
    DMA_SubmitChannelTransfer(rxHandle, &chConfig);
    DMA_StartTransfer(rxHandle);

    /* transmit */
    SPI_EnableTxDMA(base, true);
    srcAddr = (uint32_t)((xfer->txData) ? xfer->txData : &s_spiTxDummy);
    dstAddr = (uint32_t)&base->FIFOWR;
    bufferInc = (xfer->txData) ? 1 : 0;
    DMA_PrepareChannelTransfer(
        &chConfig, (void *)srcAddr, (void *)dstAddr,
        DMA_CHANNEL_XFER(1 /* reload */, 1 /* clrTrig */, 0 /* intA */, 0 /* intB */, 1 /* width */,
                         bufferInc /* srcInc */, 0 /* dstInc */, xfer->dataSize /* bytes */),
        kDMA_MemoryToPeripheral, &(dma_channel_trigger_t){kDMA_RisingEdgeTrigger, kDMA_SingleTransfer, kDMA_NoWrap},
        nextDescTx);
    DMA_SubmitChannelTransfer(txHandle, &chConfig);

    /* Setup the control info.
     * Halfword writes to just the control bits (offset 0xE22) doesn't push anything into the FIFO.
     * And the data access type of control bits must be uint16_t, byte writes or halfword writes to FIFOWR
     * will push the data and the current control bits into the FIFO.
     */
    *((uint16_t *)((uint32_t)&base->FIFOWR) + 1) = (uint16_t)(SPI_FIFOWR_LEN(spi_config_p->dataWidth) >> 16U);

    return result;
}


static void stopAdcRead(nafe_t* instance)
{
    instance->s_xferIrqsRemaining = 0;
    if (instance->s_spi_dma_handleTx.base != NULL)
        DMA_AbortTransfer(&instance->s_spi_dma_handleTx);
    instance->s_spi_dma_handleTx.base = NULL;
    if (instance->s_spi_dma_handleRx.base != NULL)
        DMA_AbortTransfer(&instance->s_spi_dma_handleRx);
    instance->s_spi_dma_handleRx.base = NULL;
    SSEL_HIGH(instance);
}


static void SPI_RxDMACallback(dma_handle_t *handle, void *userData, bool transferDone, uint32_t intmode)
{
    nafe_t *instance = (nafe_t *)userData;
    
    /* Return if transfer not done */
    if (!transferDone)
        return;
    /* Return if called when no transfer is active */
    if (instance->s_xferIrqsRemaining == 0)
        return;


    /* Only store requested number of ADC reads */
    int numReadingsToStore = instance->s_adcReadingsPerIrq;
    /* < 0 would indicate infinite mode */
    if (instance->s_adcReadingsRemaining > 0)
    {
        if (numReadingsToStore > instance->s_adcReadingsRemaining)
            numReadingsToStore = instance->s_adcReadingsRemaining;
        instance->s_adcReadingsRemaining -= numReadingsToStore;
    }

    /* Save samples to ringbuffer */
    if (RingBuffer_GetFree(instance->pAdcRingBuf) < numReadingsToStore * ADC_READ_SIZE)
    {
        /* Ran out of space, stop capturing */
        stopAdcRead(instance);

        return;
    }
    /* Store ADC data, only store requested number of ADC reads */
    uint8_t *pAdcData = (intmode == kDMA_IntA) ? instance->s_adcPingPongBuf[0] : instance->s_adcPingPongBuf[1];
    RingBuffer_InsertMult(instance->pAdcRingBuf, pAdcData, numReadingsToStore * ADC_READ_SIZE);

    /* Remaining < 0 indicates infinite mode */
    if (instance->s_xferIrqsRemaining < 0)
        return;

    /* Remaining > 0, update remaining and check if we need to stop the transfer */
    if (--instance->s_xferIrqsRemaining == 0)
    {
        stopAdcRead(instance);
    }

}


static void startContinousConversion(nafe_t *instance, annapurna_cmd_t cmd, int chainLength, int burstSize)

{
    /* Setup number of ADC readings to perform, rounded up to make multiple of instance->s_adcReadingsPerIrq */
    instance->s_xferIrqsRemaining = (instance->s_adcReadingsRemaining + instance->s_adcReadingsPerIrq - 1) / instance->s_adcReadingsPerIrq;


    /* Setup xfer for initial command */
    uint16_t cmdWord = ((instance->addr & 0x01) << 15) | cmd << 1;
    instance->s_apCmd[0] = (cmdWord >> 8) & 0xFF;
    instance->s_apCmd[1] = (cmdWord >> 0) & 0xFF;
    instance->s_spi_m_xfer.txData = instance->s_apCmd;
    instance->s_spi_m_xfer.rxData = NULL;
    instance->s_spi_m_xfer.dataSize = sizeof(instance->s_apCmd);
    instance->s_spi_m_xfer.configFlags = 0;

    /* Build descriptor chains */
    for (int i = 0; i < chainLength; i++)
    {
        void *nextDesc;
        int clrTrig;
        int doGenerateInt;

        /* Create looped chain of descriptors for Tx, DMA trigger is cleared on end of burst */
        nextDesc = (i + 1 < chainLength) ? &instance->s_nextDescTx[i + 1] : &instance->s_nextDescTx[0];
        clrTrig = (((i + 1) % burstSize) == 0) ? 1 : 0;
        DMA_SetupDescriptor(&instance->s_nextDescTx[i],
                            DMA_CHANNEL_XFER(1 /* reload */, clrTrig /* clrTrig */, 0 /* intA */, 0 /* intB */,
                                             1 /* width */, 0 /* srcInc */, 0 /* dstInc */, ADC_READ_SIZE /* bytes */),
                            &s_spiTxDummy, (void *)&instance->SPIx->FIFOWR, nextDesc);

        /* Create looped ping/pong descriptor chain for Rx, interrupts are only generated at half and end of chain */
        nextDesc = (i + 1 < chainLength) ? &instance->s_nextDescRxPing[i + 1] : &instance->s_nextDescRxPong[0];
        doGenerateInt = (i + 1 == chainLength) ? 1 : 0;
        DMA_SetupDescriptor(&instance->s_nextDescRxPing[i],
                            DMA_CHANNEL_XFER(1 /* reload */, 0 /* clrTrig */, doGenerateInt /* intA */, 0 /* intB */,
                                             1 /* width */, 0 /* srcInc */, 1 /* dstInc */, ADC_READ_SIZE /* bytes */),
                            (void *)&instance->SPIx->FIFORD, &instance->s_adcPingPongBuf[0][i * ADC_READ_SIZE], nextDesc);

        /* Generate intB at end of rx pong chain */
        nextDesc = (i + 1 < chainLength) ? &instance->s_nextDescRxPong[i + 1] : &instance->s_nextDescRxPing[0];
        doGenerateInt = (i + 1 == chainLength) ? 1 : 0;
        DMA_SetupDescriptor(&instance->s_nextDescRxPong[i],
                            DMA_CHANNEL_XFER(1 /* reload */, 0 /* clrTrig */, 0 /* intA */, doGenerateInt /* intB */,
                                             1 /* width */, 0 /* srcInc */, 1 /* dstInc */, ADC_READ_SIZE /* bytes */),
                            (void *)&instance->SPIx->FIFORD, &instance->s_adcPingPongBuf[1][i * ADC_READ_SIZE], nextDesc);
    }

    /* Flush ADC ringbuffer */
    RingBuffer_Flush(instance->pAdcRingBuf);

    /* Flush FIFO to avoid triggering DMA on stale data */
    instance->SPIx->FIFOCFG |= SPI_FIFOCFG_EMPTYTX_MASK | SPI_FIFOCFG_EMPTYRX_MASK;
    /* Flush through EMPTYTX/EMPTYRX is not completely flushing FIFO, requires FIFO to be disabled & enabled again */
    instance->SPIx->FIFOCFG &= ~(SPI_FIFOCFG_DMATX_MASK | SPI_FIFOCFG_DMARX_MASK);
    instance->SPIx->FIFOCFG |= (SPI_FIFOCFG_DMATX_MASK | SPI_FIFOCFG_DMARX_MASK);

    /* Create new DMA handles to be able to override DMA callback */
    DMA_CreateHandle(&instance->s_spi_dma_handleTx, DMA0, instance->spi_dma_tx_chl);
    DMA_CreateHandle(&instance->s_spi_dma_handleRx, DMA0, instance->spi_dma_rx_chl);
    DMA_SetCallback(&instance->s_spi_dma_handleRx, SPI_RxDMACallback, instance);

    /* Prepare DMA transfer */
    prepareSpiDmaTransfer(instance->SPIx, &instance->s_spi_dma_handleTx, &instance->s_spi_dma_handleRx, &instance->s_nextDescTx[0], &instance->s_nextDescRxPing[0],
                          &instance->s_spi_m_xfer, 0);

    /* Start SPI transfer */
    SSEL_LOW(instance);
    DMA_DoChannelSoftwareTrigger(DMA0, instance->spi_dma_tx_chl);
    
}

/*!
 * send CSSR command
 *
 * param instance instance of NAFE device
 */
void startScsr(nafe_t *instance)
{
    //uint8_t buf[3];

    uint8_t tx_buf[2];
    uint16_t cmdWord = SPI_CMD_WORD(instance->addr, kAnnapurnaCmd_Scsr, CMD_RD);
    tx_buf[0] = (cmdWord >> 8) & 0xFF;
    tx_buf[1] = (cmdWord >> 0) & 0xFF;
    _spi_polling_transfer(instance, tx_buf, NULL, 2, CS_LOW);
}


/*!
 * start SCCR
 *
 * param instance instance of NAFE device
 * param numReads: how many number of reading to be requested
 */
void startSccr(nafe_t *instance, int numReads)
{
    /* Store number of requested ADC readings */
    instance->s_adcReadingsRemaining = numReads;
    /* Number of ADC readings per irq */
    instance->s_adcReadingsPerIrq = MAX_NUM_ADC_READS_PER_DMA_INT;

    startContinousConversion(instance, kAnnapurnaCmd_Sccr, instance->s_adcReadingsPerIrq, 1);
}

/*!
 * start MCMR conversion
 *
 * param instance instance of NAFE device
 * param channelMask: channel enable mask
 * param drdybPinSeq: For drdybPinSeq == 1, set length of DMA chain to number of enabled channels. For drdybPinSeq == 0, use burst size of 1
 */
void startMcmr(nafe_t *instance, uint16_t channelMask, bool drdybPinSeq)
{
    int numEnabledChannels = 0;
    int burstSize;

    /* Determine number of enabled channels */
    for (int i = 0; i < 16; i++)
    {
        if (channelMask & (1 << i))
            numEnabledChannels++;
    }

    /* Determine burstSize based on drdybPinSeq and numEnabledChannels;
       For drdybPinSeq == 1, set length of DMA chain to number of enabled channels.
       For drdybPinSeq == 0, use burst size of 1
    */
    burstSize = drdybPinSeq ? numEnabledChannels : 1;

    /* Store number of requested ADC readings */
    instance->s_adcReadingsRemaining = numEnabledChannels;
    /* Number of ADC readings per irq */
    instance->s_adcReadingsPerIrq = numEnabledChannels;

    startContinousConversion(instance, kAnnapurnaCmd_Mcmr, instance->s_adcReadingsPerIrq, burstSize);
}


/*!
 * send conversion MCCR command
 *
 * param instance instance of NAFE device
 * param numReads: how many number of reading to be requested
 * param channelMask: channel enable mask
 * param drdybPinSeq: For drdybPinSeq == 1, set length of DMA chain to number of enabled channels. For drdybPinSeq == 0, use burst size of 1
 */
void startMccr(nafe_t *instance, int numReads, uint16_t channelMask, bool drdybPinSeq)
{
    int numEnabledChannels = 0;
    int chainLength;
    int burstSize;

    /* Determine number of enabled channels */
    for (int i = 0; i < 16; i++)
    {
        if (channelMask & (1 << i))
            numEnabledChannels++;
    }


    /* Determine burstSize and chainLength */
    if (drdybPinSeq){
    	burstSize = numEnabledChannels;
    	chainLength = (MAX_NUM_ADC_READS_PER_DMA_INT / numEnabledChannels) * numEnabledChannels;
    }
    else{
    	burstSize = 1;
    	chainLength = MAX_NUM_ADC_READS_PER_DMA_INT;
    }


    /* Store number of requested ADC readings */
    instance->s_adcReadingsRemaining = numReads * numEnabledChannels;
    /* Number of ADC readings per irq */
    instance->s_adcReadingsPerIrq = chainLength;

    startContinousConversion(instance, kAnnapurnaCmd_Mccr, instance->s_adcReadingsPerIrq, burstSize);
}

