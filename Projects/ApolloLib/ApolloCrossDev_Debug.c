/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

/* Serial debug output of ApolloLib: ApolloDebugPutChar() writes straight to
 * SERDAT at 115200 baud and makes no OS call, so it also works without a
 * console. The Debug_Info()/Debug_Warn()/... macros in ApolloCrossDev_Debug.h
 * print through it. The command tracer and the AllocMem/FreeMem wrappers
 * (DBG_CMD, DBG_MEM) belong to device drivers and are not part of ApolloLib.
 */
#include "ApolloCrossDev_Debug.h"

#include <exec/types.h>
#include <exec/io.h>
#include <exec/errors.h>
#include <inline/exec.h>

// Global Variables
char ApolloDebugMessage[256];


void WriteRegisterWord(uint32_t reg, uint16_t val)
{
    volatile uint16_t *r = (void *)(reg);
	*r = val;
}

uint16_t ReadRegisterWord(uint32_t reg)
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
	WriteRegisterWord(SERPER, SERPER_BAUD(SERPER_BASE_PAL, 115200));		/* Set the debug UART to 115200 */
}

int ApolloDebugPutChar(register int chr)
{
	if (chr == '\n') ApolloDebugPutChar('\r');
	while ((ReadRegisterWord(SERDATR) & SERDATR_TBE) == 0);
	WriteRegisterWord(INTREQ, INTF_TBE);
	WriteRegisterWord(SERDAT, SERDAT_STP8 | SERDAT_DB8(chr));				/* Output a char to the debug UART */
	return 1;
}

int ApolloDebugMayGetChar(void)
{
	int c;
	if ((ReadRegisterWord(SERDATR) & SERDATR_RBF) == 0) return -1;
	c = SERDATR_DB8_of(ReadRegisterWord(SERDATR));
	WriteRegisterWord(INTREQ, INTF_RBF);									/* Acknowledge the receive */	
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
	int started = 0;
	for (i = 1000000000; i > 0; i /= 10)
	{
	    num = val / i;
	    if (num == 0 && !started && i > 1) continue;			/* Skip leading zeros only */
	    started = 1;
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
