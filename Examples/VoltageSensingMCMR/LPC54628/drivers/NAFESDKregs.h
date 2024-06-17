/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019, 2023 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _NAFE13388REGS_H_
#define _NAFE13388REGS_H_


#include <stdint.h>
#include <stdbool.h>

#define SPI_CMD_WORD(addr, cmd, dir)    (((addr & 0x01) << 15) | (cmd << 1) | (dir<<14))
#define CMD_RD                          (1)
#define CMD_WR                          (0)


/* regs */
#define SYS_CONFIG0                 (0x30)
#define SYS_STATUS0                 (0x31)
#define CH_CONFIG0                  (0x20)
#define CH_CONFIG1                  (0x21)
#define CH_CONFIG2                  (0x22)
#define CH_CONFIG3                  (0x23)
#define CH_CONFIG4                  (0x24)
#define CH_STATUS0                  (0x35)
#define CH_STATUS1                  (0x36)
#define GPIDATA                     (0x29)
#define GPIOCONFIG0                 (0x2A)
#define GPIOCONFIG1                 (0x2B)
#define GPIOCONFIG2                 (0x2C)
#define GPIEDGEPOS                  (0x2D)
#define GPIEDGENEG                  (0x2E)
#define GPODATA                     (0x2F)
#define GLOBALALAMENABLE            (0x32)
#define GLOBALALARMINTERRUPT        (0x33)
#define DIETEMP                     (0x34)
#define SERIAL0                     (0x7C)
#define SERIAL1                     (0x7D)
#define PRODREV                     (0x7E)
#define PRODID                      (0x7F)
#define CHDATA0                     (0x40)
#define CHDATA15                    (0x4F)
#define CHCONFIGOVTHRS0             (0x50)
#define CHCONFIGOVTHRS15            (0x5F)
#define CHCONFIGUDRTHRS0            (0x60)
#define CHCONFIGUDRTHRS15           (0x6F)
#define GAIN1COEF0                  (0x80)
#define GAIN1COEF15                 (0x8F)
#define OFFSET1COEF0                (0x90)
#define OFFSET1COEF15               (0x9F)
#define INTSELFCAL0                 (0xA0)
#define INTSELFCAL15                (0xAF)
#define GAIN_COEF0                  (0x80)
#define OFFSET_COEF0                (0xA0)

/* commands */
#define CMD_SETPOINTER0             (0x00)
#define CMD_SETPOINTER15            (0x0F)
#define CMD_ABORT                   (0x10)
#define CMD_END                     (0x11)
#define CMD_CLEARALARM              (0x12)
#define CMD_CLEARDATA               (0x13)
#define CMD_RESET                   (0x14)
#define CMD_CLEARREG                (0x15)
#define CMD_RELOAD                  (0x16)
#define CMD_SCSR                    (0x2000)
#define CMD_SCCR                    (0x2001)
#define CMD_MCMR                    (0x2002)
#define CMD_MCCR                    (0x2003)
#define CMD_READDATA                (0x2004)
#define CMD_BURSTDATA               (0x2005)





#endif