/*
 * Copyright (C) 2011, The AROS Development Team.  All rights reserved.
 * Author: Jason S. McMullan <jason.mcmullan@gmail.com>
 *
 * Licensed under the AROS PUBLIC LICENSE (APL) Version 1.1
 */

#ifdef __cplusplus
extern "C"{
#endif 

#ifndef _DEBUG_H
#define _DEBUG_H

#ifdef __INTELLISENSE__
#define asm(x)
#endif

#include <exec/execbase.h>
#include <proto/exec.h>
#include <exec/types.h>
#include <stdarg.h>

// Global Variables
extern char ApolloDebugMessage[256];
#include <stdint.h>


#define DBG_INFO  1
#define DBG_WARN  2
#define DBG_ERROR 4
#define DBG_FLAG  8
#define DBG_TRACE 16
#define DBG_CMD   32
#define DBG_MEM   64

#ifndef RawDoFmt
APTR __stdargs RawDoFmt(CONST_STRPTR formatString, APTR dataStream, VOID (*putChProc)(), APTR putChData);
#endif

int ApolloDebugPutChar(register int chr);


static void bug_putch(APTR data asm("a3"), UBYTE ch asm("d0"))
{
    (void)data;
    ApolloDebugPutChar(ch);
}

static inline void bug_print(const char *type, const char *project, const char *file, const char *func, LONG line, const char *fmt, ...)
{
    struct ExecBase *SysBase = *(struct ExecBase **)4UL;
    va_list args;
    ULONG header[4];

    header[0] = (ULONG)project;
    header[1] = (ULONG)file;
    header[2] = (ULONG)func;
    header[3] = (ULONG)line;
    Forbid();
    switch (type[0]) {
        case 'I': RawDoFmt((CONST_STRPTR)"[%-20s][%-20s][%-20s][%5ld] [INFO ] ", header, (APTR)bug_putch, NULL); break;
        case 'W': RawDoFmt((CONST_STRPTR)"\033[93m[%-20s][%-20s][%-20s][%5ld] [WARN ] \033[0m", header, (APTR)bug_putch, NULL); break;
        case 'E': RawDoFmt((CONST_STRPTR)"\033[91m[%-20s][%-20s][%-20s][%5ld] [ERROR]\033[0m ", header, (APTR)bug_putch, NULL); break;
        case 'T': RawDoFmt((CONST_STRPTR)"\033[90m[%-20s][%-20s][%-20s][%5ld] [TRACE] \033[0m", header, (APTR)bug_putch, NULL); break;
        case 'F': RawDoFmt((CONST_STRPTR)"\033[6m\033[95m[%-20s][%-20s][%-20s][%5ld] [FLAG ] \033[0m", header, (APTR)bug_putch, NULL); break;
        default:  RawDoFmt((CONST_STRPTR)"[%-20s][%-20s][%-20s][%5ld] [BUG  ]  ", header, (APTR)bug_putch, NULL); break;
    }   
    va_start(args, fmt);
    RawDoFmt((CONST_STRPTR)fmt, (APTR)args, (APTR)bug_putch, NULL);
    va_end(args);
    Permit();
}

#define bug(type, format, args...) bug_print(type, PROJECT_NAME, BASE_FILE_NAME, __func__, __LINE__, format, ##args)

#endif

#if DEBUG & DBG_INFO
#define Debug_Info(...) bug("INFO", __VA_ARGS__)
#else
#define Debug_Info(...)
#endif

#if DEBUG & DBG_WARN
#define Debug_Warn(...) bug("WARN", __VA_ARGS__)
#else
#define Debug_Warn(...)
#endif

#if DEBUG & DBG_ERROR
#define Debug_Error(...) bug("ERROR", __VA_ARGS__)
#else
#define Debug_Error(...)
#endif

#if DEBUG & DBG_TRACE
#define Debug_Trace(...) bug("TRACE", __VA_ARGS__)
#else
#define Debug_Trace(...)
#endif

#if DEBUG & DBG_FLAG
#define Debug_Flag(...) bug("FLAG", __VA_ARGS__)
#else
#define Debug_Flag(...)
#endif

#if DEBUG & DBG_CMD
void traceCommand(struct IOStdReq *req);
#else
#define traceCommand
#endif

#if DEBUG & DBG_MEM
void * DebugAllocMem(char *file, int line, ULONG byteSize, ULONG attributes);
#undef AllocMem
#define AllocMem(x,y) DebugAllocMem(__FILE__,__LINE__,x,y)

void DebugFreeMem(char *file, int line, void *memBlock, ULONG byteSize);
#undef FreeMem
#define FreeMem(x,y) DebugFreeMem(__FILE__,__LINE__,x,y)
#endif


/** Screen/Serial Debug **/

#define SERDAT  0xDFF030
#define ADKCON  0xDFF09E
#define SERDATR 0xDFF018
#define SERPER  0xDFF032
#define INTREQ  0xDFF09C

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

void WriteRegisterWord(uint32_t reg, uint16_t val);
uint16_t ReadRegisterWord(uint32_t reg);

extern void ApolloDebugInit(void);
extern int  ApolloDebugPutChar(register int chr);
extern int  ApolloDebugMayGetChar(void);
extern void ApolloDebugPutStr(register const char *buff);
extern void ApolloDebugPutHex(const char *what, uint32_t val);
extern void ApolloDebugPutDec(const char *what, uint32_t val);
extern void ApolloDebugPutHexVal(uint32_t val);

#define BUG_RESET "\033[0m"
#define BUG_BOLD "\033[1m"
#define BUG_DIM "\033[2m"
#define BUG_ITALIC "\033[3m"
#define BUG_UNDERLINE "\033[4m"
#define BUG_BLINK "\033[5m"
#define BUG_BLINK2 "\033[6m"
#define BUG_REVERSE "\033[7m"
#define BUG_HIDDEN "\033[8m"
#define BUG_STRIKE "\033[9m"

#define BUG_BLACK "\033[0;30m"
#define BUG_RED "\033[0;31m"
#define BUG_GREEN "\033[0;32m"
#define BUG_YELLOW "\033[0;33m"
#define BUG_BLUE "\033[0;34m"
#define BUG_MAGENTA "\033[0;35m"
#define BUG_CYAN "\033[0;36m"
#define BUG_GREY "\033[0;37m"

#define BUG_BLACK2 "\033[0;90m"
#define BUG_RED2 "\033[0;91m"
#define BUG_GREEN2 "\033[0;92m"
#define BUG_YELLOW2 "\033[0;93m"
#define BUG_BLUE2 "\033[0;94m"
#define BUG_MAGENTA2 "\033[0;95m"
#define BUG_CYAN2 "\033[0;96m"
#define BUG_GREY2 "\033[0;97m"

#ifdef __cplusplus
}



#endif /* _DEBUG_H */