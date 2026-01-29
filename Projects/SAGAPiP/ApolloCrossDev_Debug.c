/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#include "ApolloCrossDev_Debug.h"

void reg_w(uint32_t reg, uint16_t val)
{
    volatile uint16_t *r = (void *)(reg);
	*r = val;
}

uint16_t reg_r(uint32_t reg)
{
	volatile uint16_t *r = (void *)(reg);
	return *r;
}

void ApolloDebugInit(void)
{
	volatile UBYTE * ciab_pra = (APTR)0xBFD000;					/* Set DTR, RTS, etc */
	volatile UBYTE * ciab_ddra = (APTR)0xBFD200;
	*ciab_ddra = 0xc0;  										/* Only DTR and RTS are driven as outputs */
	*ciab_pra = 0;      										/* Turn on DTR and RTS */
	reg_w(SERPER, SERPER_BAUD(SERPER_BASE_PAL, 115200));		/* Set the debug UART to 115200 */
}

int ApolloDebugPutChar(register int chr)
{
	if (chr == '\n') ApolloDebugPutChar('\r');
	while ((reg_r(SERDATR) & SERDATR_TBE) == 0);
	reg_w(INTREQ, INTF_TBE);
	reg_w(SERDAT, SERDAT_STP8 | SERDAT_DB8(chr));				/* Output a char to the debug UART */
	return 1;
}

int ApolloDebugMayGetChar(void)
{
	int c;
	if ((reg_r(SERDATR) & SERDATR_RBF) == 0) return -1;
	c = SERDATR_DB8_of(reg_r(SERDATR));
	reg_w(INTREQ, INTF_RBF);									/* Acknowledge the receive */	
	return c;
}

void ApolloDebugPutStr(register const char *buff)
{
	for (; *buff != 0; buff++) ApolloDebugPutChar(*buff);
}

void ApolloDebugPutDec(const char *what, uint32_t val)
{
	int i, num;
	ApolloDebugPutStr(what);
	ApolloDebugPutStr(": ");
	if (val == 0) {
	    ApolloDebugPutChar('0');
	    ApolloDebugPutChar('\n');
	    return;
	}
	for (i = 1000000000; i > 0; i /= 10)
	{
	    if (val == 0)
		{
	    	ApolloDebugPutChar('0');
	    	continue;
	    }
	    num = val / i;
	    if (num == 0) continue;
	    ApolloDebugPutChar("0123456789"[num]);
	    val -= num * i;
	}
	ApolloDebugPutChar('\n');
}

void ApolloDebugPutHexVal(uint32_t val)
{
	int i;
	for (i = 0; i < 8; i ++)
	{
		ApolloDebugPutChar("0123456789abcdef"[(val >> (28 - (i * 4))) & 0xf]);
	}
	ApolloDebugPutChar(' ');
}

void ApolloDebugPutHex(const char *what, uint32_t val)
{
	ApolloDebugPutStr(what);
	ApolloDebugPutStr(": ");
	ApolloDebugPutHexVal(val);
	ApolloDebugPutChar('\n');
}



