// Apollo V4 Library - Willem Drijver

#ifdef __cplusplus
extern "C"{
#endif 

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <clib/exec_protos.h>
#include <exec/types.h>

// These Routines are checked for use in ApolloCrossDev, the rest must still be validated
#include "ApolloTakeOver.h"
#include "ApolloWaitVBL.h"
#include "ApolloEndianSwap8.h"
#include "ApolloCPUDelay.h"

#define AIFF_OFFSET				128
#define DDS_OFFSET				128
#define RAW_OFFSET				12

#define APOLLO_SAGAMODE_REG		0xDFF1F4	
#define APOLLO_SAGADISPLAY_REG	0xDFF1EC
#define APOLLO_SAGAMODULO_REG	0xDFF1E6

#define APOLLO_POINTER_SET_X    0xDFF1D0
#define APOLLO_POINTER_SET_Y    0xDFF1D2
#define APOLLO_POINTER_GET_X    0xDFE1D0
#define APOLLO_POINTER_GET_Y    0xDFE1D2

#define APOLLO_MOUSE_GET_X      0xDFF00B
#define APOLLO_MOUSE_GET_Y      0xDFF00A 
#define APOLLO_MOUSE_BUTTON1    0xBFE001
#define APOLLO_MOUSE_BUTTON2    0xDFF016
#define APOLLO_MOUSE_BUTTON3    0xDFF016		//TODO

#define APOLLO_POINTER_COL1     0xDFF3A2
#define APOLLO_POINTER_COL2     0xDFF3A4
#define APOLLO_POINTER_COL3     0xDFF3A6

#define APOLLO_POINTER_RED		0x0A00
#define APOLLO_POINTER_GREEN	0x00A0
#define APOLLO_POINTER_BLUE		0x000A

UBYTE    ApolloDebugText[120];

typedef struct
{
	UBYTE Current_Key;
	UBYTE Previous_Key;
} ApolloKeyBoardState;


typedef struct
{
	BYTE Joypad_X_Delta;
	BYTE Joypad_Y_Delta;
	bool Joypad_Start;
	bool Joypad_Back;
	bool Joypad_TR;
	bool Joypad_TL;
	bool Joypad_BR;
	bool Joypad_BL;
	bool Joypad_Y;
	bool Joypad_X;
	bool Joypad_B;
	bool Joypad_A;
	bool Joypad_Connect;
} ApolloJoypadState;

typedef struct
{
	WORD	MouseX_Pointer;
	WORD	MouseY_Pointer;
	WORD	MouseX_Pointer_Max;
	WORD	MouseY_Pointer_Max;

	WORD	MouseX_Value;
	WORD	MouseY_Value;
	WORD	MouseX_Value_Old;
	WORD	MouseY_Value_Old;
	WORD	MouseX_Value_Delta;
	WORD	MouseY_Value_Delta;

	bool	Button_Left;			// Actual state for Buttons	
	bool	Button_Right;
	bool	Button_Middle;
	
	bool	Button_Left_Old;		// Previous state for Buttons	
	bool	Button_Right_Old;
	bool	Button_Middle_Old;
	
	UWORD	Button_State;			// Interpreted Button Action (see table below)
	UWORD	Button_Left_Count;
	UWORD	Button_Right_Count;
	UWORD	Button_Middle_Count;
} ApolloMouseState;

#define APOLLOMOUSE_DOUBLECLICKCOUNTER	15
#define APOLLOMOUSE_LEFTCLICK			0x0001
#define APOLLOMOUSE_RIGHTCLICK			0x0002
#define APOLLOMOUSE_MIDDLECLICK			0x0004
#define APOLLOMOUSE_LEFTDOUBLECLICK		0x0008
#define APOLLOMOUSE_RIGHTDOUBLECLICK	0x0010
#define APOLLOMOUSE_MIDDLEDOUBLECLICK	0x0020
#define APOLLOMOUSE_LEFTDOWN			0x0040
#define APOLLOMOUSE_RIGHTDOWN			0x0080
#define APOLLOMOUSE_MIDDLEDOWN			0x0100

extern void ApolloLoad(const UBYTE *filename, UBYTE **buffer, ULONG *lenght, UWORD offset, bool endianswap);
extern void ApolloPlay(UBYTE *buffer, ULONG buffer_lenght, UWORD channel, UWORD volume_left, UWORD volume_right, bool loop);
extern void ApolloShow(UBYTE *buffer, ULONG buffer_lenght, UWORD gfx_mode, UWORD gfx_modulo);

extern void ApolloStop(UWORD channel);
extern void ApolloStart(UWORD channel);
extern void ApolloFadeOut(UWORD channel, UWORD volume_start, UWORD volume_end);
extern void ApolloVolume(UWORD channel, UWORD volume_left, UWORD volume_right);

extern void ApolloShowFile(const UBYTE *filename, UBYTE **buffer, UBYTE *buffer_lenght, UWORD offset, UWORD gfx_mode, UWORD gfx_modulo, bool endianswap);
extern void ApolloCacheFile(const UBYTE *filename, UBYTE **cache_base, ULONG *cache_offset, UWORD file_offset);

extern void ApolloSwap16(UBYTE *buffer, ULONG lenght);
extern void ApolloSwap16Transparent(UBYTE *buffer, ULONG lenght);
extern void ApolloSwapShadow(UWORD *buffer, ULONG lenght, UWORD shadow);
extern void ApolloWaitVBL();
extern void ApolloMouse(ApolloMouseState *MouseState);
extern void ApolloJoypad(ApolloJoypadState *JoypadState);
extern void ApolloKeyboard(ApolloKeyBoardState *KeyboardState);
extern UBYTE ApolloKeyboardToUnicode(UBYTE KeyboardAmiga);

#define INTENAR				0x1c
#define INTREQR				0x1e
#define INTENA				0x9a
#define INTREQ				0x9c

#define	SERPER_BASE_PAL		3546895
#define	SERPER_BASE_NTSC	3579545
#define SERPER				0x32
#define SERPER_BAUD(base, x)	((((base + (x)/2))/(x)-1) & 0x7fff)	/* Baud rate */
#define SERDATR				0x18
#define SERDATR_OVRUN		(1 << 15)	/* Overrun */
#define SERDATR_RBF			(1 << 14)	/* Rx Buffer Full */
#define SERDATR_TBE			(1 << 13)	/* Tx Buffer Empty */
#define SERDATR_TSRE		(1 << 12)	/* Tx Shift Empty */
#define SERDATR_RXD			(1 << 11)	/* Rx Pin */
#define SERDATR_STP9		(1 <<  9)	/* Stop bit (if 9 data) */
#define SERDATR_STP8		(1 <<  8)	/* Stop bit (if 8 data) */
#define SERDATR_DB9_of(x)	((x) & 0x1ff)	/* 9-bit data */
#define SERDATR_DB8_of(x)	((x) & 0xff)	/* 8-bit data */
#define ADKCON				0x9e
#define ADKCON_SETCLR		(1 << 15)
#define ADKCON_UARTBRK		(1 << 11)	/* Force break */
#define SERDAT				0x30
#define SERDAT_STP9			(1 << 9)
#define SERDAT_STP8			(1 << 8)
#define SERDAT_DB9(x)		((x) & 0x1ff)
#define SERDAT_DB8(x)		((x) & 0xff)

#define BPLCON0				0x100
#define BPL1DAT				0x110
#define COLOR00				0x180

void reg_w(ULONG reg, UWORD val);
UWORD reg_r(ULONG reg);

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

extern void		ApolloDebugInit(void);
extern UWORD 	ApolloDebugPutChar(register UWORD chr);
extern UWORD 	ApolloDebugMayGetChar(void);

extern void 	ApolloDebugPutStr(register const UBYTE *buff);
extern void 	ApolloDebugPutHex(const UBYTE *what, ULONG val);
extern void 	ApolloDebugPutDec(const UBYTE *what, ULONG val);
extern void 	ApolloDebugPutHexVal(ULONG val);

#ifdef __cplusplus
}
#endif