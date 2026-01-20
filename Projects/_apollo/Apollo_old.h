// Apollo V4 SAGA libraries
// Willem Drijver

#ifdef __cplusplus
extern "C"{
#endif 

#include <stdio.h>

#include "ApolloDebug.h"
#include "ApolloFill.h"
#include "ApolloBlit.h"
#include "ApolloBlitAlphaConst.h"
#include "ApolloBlitAlphaKeyTrans.h"
#include "ApolloBlitAlphaHorizontalLine.h"
#include "ApolloBlitBlueAlphaUI.h"
#include "ApolloBlitRedAlphaBG.h"
#include "ApolloBlitGreenAlphaBG.h"
#include "ApolloBlitBlueAlphaBG.h"
#include "ApolloCopy32.h"
#include "ApolloRegParam.h"
#include "ApolloCPUTick.h"
#include "ApolloCPUTime.h"
#include "ApolloCPUDelay.h"
#include "ApolloCPUDelayLowPri.h"
#include "ApolloCacheCounters.h"
#include "ApolloEndianSwap2.h"
#include "ApolloEndianSwap8.h"
#include "ApolloSwapWord.h"
#include "ApolloSwapLong.h"
#include "ApolloSwapOcta.h"
#include "ApolloUncompress.h"
#include "ApolloUncompressVector.h"
#include "ApolloCopy.h"
#include "ApolloTakeOver.h"
#include "ApolloTimer.h"
#include "ApolloForbid.h"
#include "ApolloPermit.h"
#include "ApolloAvailMem.h"

#define AIFF_OFFSET		128
#define DDS_OFFSET		128

#define SAGA_MODE_848   0x0F02
#define SAGA_MODE_1280  0x0A02

#define SAGA_X_SIZE     848
#define SAGA_Y_SIZE     480
#define SAGA_X_SIZE_F	848.0f
#define SAGA_Y_SIZE_F	480.0f

#define SAGA_X_HUD		848
#define SAGA_Y_HUD		80

#define SAGA_PIXELS     SAGA_X_SIZE * SAGA_Y_SIZE
#define SAGA_BYTES      SAGA_PIXELS * SAGA_BYTEPP

#define SAGA_BYTEPP		2
#define SAGA_BITPP		SAGA_BYTEPP * 8

#define APOLLO_POINTER_SET_X    0xDFF1D0
#define APOLLO_POINTER_SET_Y    0xDFF1D2
#define APOLLO_POINTER_GET_X    0xDFE1D0
#define APOLLO_POINTER_GET_Y    0xDFE1D2

#define APOLLO_MOUSE_GET_X      0xDFF00B
#define APOLLO_MOUSE_GET_Y      0xDFF00A 
#define APOLLO_MOUSE_BUTTON1    0xBFE001
#define APOLLO_MOUSE_BUTTON2    0xDFF016

#define APOLLO_POINTER_COL0     0xDFF3A0        // CLUT[4] SAGA Pointer Color Table
#define APOLLO_POINTER_COL1     0xDFF3A2
#define APOLLO_POINTER_COL2     0xDFF3A4
#define APOLLO_POINTER_COL3     0xDFF3A6

#define APOLLO_POINTER_DATA     0xDFF800        // PLANAR[1] SAGA Pointer Bitmap [16x16]
#define APOLLO_POINTER_BITMAP   {0xFC00,0xFC00,0xFE00,0x8200,0x8600,0xFA00,0x8C00,0xF400,0x8600,0xFA00,0x9300,0xFD00,0x6980,0x6E80,0x04C0,0x0740,0x0260,0x03A0,0x0140,0x01C0,0x0080,0x0080,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
#define APOLLO_POINTER_RED		0x0A00
#define APOLLO_POINTER_GREEN	0x00A0
#define APOLLO_POINTER_BLUE		0x000A

#define APOLLO_POINTER_BLACK    0x0000
#define APOLLO_POINTER_ORANGE   0x0FCA
#define APOLLO_POINTER_RED2     0x0D22

extern char    ApolloDebugText[120];

typedef struct
{
	int8_t Joypad_X_Delta;
	int8_t Joypad_Y_Delta;
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
	signed short    Mouse_X_New;
	signed short    Mouse_Y_New;
	signed short    Mouse_X_Old;
	signed short    Mouse_Y_Old;
	signed short    Mouse_X_Delta;
	signed short    Mouse_Y_Delta;
	unsigned short	Mouse_X_Position;
	unsigned short	Mouse_Y_Position;
	signed short    Mouse_Scroll;
	bool	        Mouse_Button1;					// Actual state for Button 1	
	bool	        Mouse_Button2;					// Actual state for Button 2
	bool	        Mouse_Button3;					// Actual state for Button 3
	unsigned short	ButtonState;            		// Interpreted Button Action (see table below)
	unsigned short	LeftCount;				
	unsigned short	LeftDefault;
	unsigned short	RightCount;
	unsigned short	RightDefault;
	unsigned short	MiddleCount;
	unsigned short	MiddleDefault;
} ApolloMouseState;

extern void ApolloLoad(const char *filename, uint8_t **buffer, uint32_t *lenght, uint16_t offset);
extern bool ApolloPlay(int channel, int volume_left, int volume_right, bool loop, uint8_t *buffer, uint32_t lenght, uint16_t offset);
extern void ApolloStop(int channel);
extern void ApolloStart(int channel);
extern void ApolloFadeOut(int channel, int volume_start, int volume_end);
extern void ApolloVolume(int channel, int volume_left, int volume_right);
extern void ApolloShow(uint16_t gfx_mode, uint16_t gfx_modulo, uint8_t *buffer);
extern void ApolloShowFile(const char *filename, uint8_t **buffer_draw, uint8_t **buffer_live, uint32_t lenght, uint16_t offset, uint16_t gfx_mode, uint16_t gfx_modulo, bool endianswap);
extern void ApolloPlayFile(const char *filename, uint8_t *buffer, uint16_t offset, int channel, int volume_left, int volume_right, bool loop);
extern void ApolloCacheFile(const char *filename, uint8_t **cache_base, uint32_t *cache_offset, uint16_t file_offset);
extern void ApolloSwap16(uint8_t *buffer, uint32_t lenght);
extern void ApolloSwap16Transparent(uint8_t *buffer, uint32_t lenght);
extern void ApolloSwapShadow(UWORD *buffer, ULONG lenght, UWORD shadow);

extern void ApolloWaitVBL();

extern void ApolloMouse(ApolloMouseState *MouseState);

extern void ApolloJoypad(ApolloJoypadState *JoypadState);

extern void ApolloKeyboard(unsigned char *Keyboard_Key);

extern UBYTE ApolloKeyboardToUnicode(UBYTE KeyboardAmiga);

#ifdef __cplusplus
}
#endif