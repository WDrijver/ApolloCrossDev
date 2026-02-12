/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifdef __cplusplus
extern "C"{
#endif 

#include "ApolloCrossDev_Defines.h"

// Global Variables
extern char ApolloDebugMessage[200];

/** Screen/Serial Debug **/

#define SERPER_BASE_PAL		3546895
#define SERPER_BASE_NTSC	3579545

#define SERPER_BAUD(base, x)    ((((base + (x)/2))/(x)-1) & 0x7fff)	/* Baud rate */

#define SERDATR_OVRUN		    (1 << 15)	    /* Overrun */
#define SERDATR_RBF		        (1 << 14)	    /* Rx Buffer Full */
#define SERDATR_TBE		        (1 << 13)	    /* Tx Buffer Empty */
#define SERDATR_TSRE		    (1 << 12)	    /* Tx Shift Empty */
#define SERDATR_RXD		        (1 << 11)	    /* Rx Pin */
#define SERDATR_STP9		    (1 <<  9)	    /* Stop bit (if 9 data) */
#define SERDATR_STP8		    (1 <<  8)	    /* Stop bit (if 8 data) */
#define SERDATR_DB9_of(x)	    ((x) & 0x1ff)	/* 9-bit data */
#define SERDATR_DB8_of(x)	    ((x) & 0xff)	/* 8-bit data */
#define ADKCON_SETCLR		    (1 << 15)
#define ADKCON_UARTBRK	        (1 << 11)	    /* Force break */
#define SERDAT_STP9		        (1 <<  9)
#define SERDAT_STP8		        (1 <<  8)
#define SERDAT_DB9(x)		    ((x) & 0x1ff)
#define SERDAT_DB8(x)		    ((x) & 0xff)

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


void reg_w(uint32_t reg, uint16_t val);
uint16_t reg_r(uint32_t reg);

extern void ApolloDebugInit(void);
extern int  ApolloDebugPutChar(register int chr);
extern int  ApolloDebugMayGetChar(void);
extern void ApolloDebugPutStr(register const char *buff);
extern void ApolloDebugPutHex(const char *what, uint32_t val);
extern void ApolloDebugPutDec(const char *what, uint32_t val);
extern void ApolloDebugPutHexVal(uint32_t val);

#ifdef __cplusplus
}
#endif