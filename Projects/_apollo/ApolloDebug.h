/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */
#ifdef __cplusplus
extern "C"{
#endif 

#ifndef AMIGA_HWREG_H
#define AMIGA_HWREG_H

#include <exec/types.h>

/** Interrupts */
#define INTENAR			0x1c
#define INTREQR			0x1e
#define INTENA			0x9a
#define INTREQ			0x9c

/** Screen/Serial Debug **/

#define SERPER_BASE_PAL		3546895
#define SERPER_BASE_NTSC	3579545
#define SERPER			0x32
#define   SERPER_BAUD(base, x)	((((base + (x)/2))/(x)-1) & 0x7fff)	/* Baud rate */
#define SERDATR			0x18
#define   SERDATR_OVRUN		(1 << 15)	/* Overrun */
#define   SERDATR_RBF		(1 << 14)	/* Rx Buffer Full */
#define   SERDATR_TBE		(1 << 13)	/* Tx Buffer Empty */
#define   SERDATR_TSRE		(1 << 12)	/* Tx Shift Empty */
#define   SERDATR_RXD		(1 << 11)	/* Rx Pin */
#define   SERDATR_STP9		(1 <<  9)	/* Stop bit (if 9 data) */
#define   SERDATR_STP8		(1 <<  8)	/* Stop bit (if 8 data) */
#define   SERDATR_DB9_of(x)	((x) & 0x1ff)	/* 9-bit data */
#define   SERDATR_DB8_of(x)	((x) & 0xff)	/* 8-bit data */
#define ADKCON			0x9e
#define   ADKCON_SETCLR		(1 << 15)
#define   ADKCON_UARTBRK	(1 << 11)	/* Force break */
#define SERDAT			0x30
#define   SERDAT_STP9		(1 << 9)
#define   SERDAT_STP8		(1 << 8)
#define   SERDAT_DB9(x)		((x) & 0x1ff)
#define   SERDAT_DB8(x)		((x) & 0xff)

#define BPLCON0			0x100
#define BPL1DAT			0x110
#define COLOR00			0x180

void reg_w(ULONG reg, UWORD val);
UWORD reg_r(ULONG reg);

#endif /* AMIGA_HWREG_H */

#ifndef HARDWARE_INTBITS_H
#define HARDWARE_INTBITS_H

#define INTB_TBE          0
#define INTF_TBE     (1L<<0)
#define INTB_DSKBLK       1
#define INTF_DSKBLK  (1L<<1)
#define INTB_SOFTINT      2
#define INTF_SOFTINT (1L<<2)
#define INTB_PORTS        3
#define INTF_PORTS   (1L<<3)
#define INTB_COPER        4
#define INTF_COPER   (1L<<4)
#define INTB_VERTB        5
#define INTF_VERTB   (1L<<5)
#define INTB_BLIT         6
#define INTF_BLIT    (1L<<6)
#define INTB_AUD0         7
#define INTF_AUD0    (1L<<7)
#define INTB_AUD1         8
#define INTF_AUD1    (1L<<8)
#define INTB_AUD2         9
#define INTF_AUD2    (1L<<9)
#define INTB_AUD3         10
#define INTF_AUD3    (1L<<10)
#define INTB_RBF          11
#define INTF_RBF     (1L<<11)
#define INTB_DSKSYNC      12
#define INTF_DSKSYNC (1L<<12)
#define INTB_EXTER        13
#define INTF_EXTER   (1L<<13)
#define INTB_INTEN        14
#define INTF_INTEN   (1L<<14)
#define INTB_SETCLR       15
#define INTF_SETCLR  (1L<<15)

#endif /* HARDWARE_INTBITS_H */

extern void DebugInit(void);
extern int DebugPutChar(register int chr);
extern int DebugMayGetChar(void);

extern void DebugPutStr(register const char *buff);
extern void DebugPutHex(const char *what, ULONG val);
extern void DebugPutDec(const char *what, ULONG val);
extern void DebugPutHexVal(ULONG val);

#ifdef __cplusplus
}
#endif