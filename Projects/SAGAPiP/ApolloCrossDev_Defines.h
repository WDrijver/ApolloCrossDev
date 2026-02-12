// Apollo V4 SAGA libraries
// Willem Drijver

// Include Basic C Headers
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>

// Include Basic Amiga Headers
#include "exec/types.h"
#include <exec/ports.h>
#include <exec/io.h>

#include "clib/exec_protos.h"
#include "clib/graphics_protos.h"
#include "clib/lowlevel_protos.h"
#include "clib/intuition_protos.h"
#include "clib/keymap_protos.h"
#include "clib/asl_protos.h"
#include "clib/dos_protos.h"
#include "clib/gadtools_protos.h"
#include "clib/input_protos.h"
#include "clib/alib_protos.h"

#include <devices/input.h>
#include <graphics/rastport.h>
#include <intuition/intuition.h>

#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/gadtools.h>
#include <proto/input.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/keymap.h>
#include <proto/asl.h>

// Apollo Debug
#ifdef APOLLO_DEBUG
 #define AD(x) x
 #ifdef APOLLO_DEBUGEXTRA
  #define ADX(x) x
 #else
  #define ADX(x)
 #endif
#else
 #define AD(x)
 #define ADX(x) 
#endif

// Apollo Audi (ARNE)
#define APOLLO_PAL_CLOCK        3546895   // PAL Clock Frequency in Hz
#define APOLLO_NTSC_CLOCK       3579545   // NTSC Clock Frequency in Hz

// Apollo File Header Markers
#define APOLLO_RAW_FORMAT       0x0 
#define APOLLO_AIFF_FORMAT      0x1
#define APOLLO_DDS_FORMAT	    0x2
#define APOLLO_BMP_FORMAT	    0x3
#define APOLLO_WAV_FORMAT       0x4

// Apollo Video (ISABELLE)
#define APOLLO_SAGA_GFXMODE		0xDFF1F4	// Bit[8-15]=SAGA Display Resolution + Bit[0-7]=Color Format 
#define APOLLO_SAGA_POINTER		0xDFF1EC	// Chunky Bitmap Pointer  
#define APOLLO_SAGA_MODULO		0xDFF1E6	// Chunky Bitmap Modulo (Bytes skipped after each Row)

#define APOLLO_SAGA_PLANAR_COL  0xDFF380    // Index Planar = Bit[24-31] Color Number | Bit[16-23] Red | Bit[8-15] Green | Bit[0-7] Blue
#define APOLLO_SAGA_CHUNKY_COL  0xDFF388    // Index Chunky = Bit[24-31] Color Number | Bit[16-23] Red | Bit[8-15] Green | Bit[0-7] Blue
#define APOLLO_SAGA_PIPCHK_COL  0xDFF38C    // Index PiP    = Bit[24-31] Color Number | Bit[16-23] Red | Bit[8-15] Green | Bit[0-7] Blue

#define APOLLO_SAGA_PIP_GFXMODE 0xDFF3DC	// Bit[8]= Enable 0xF81F Transparency + Bit[0-7]=Color Format (only modes 0x01,0x02 and 0x03)
#define APOLLO_SAGA_PIP_POINTER 0xDFF3D8	// Chunky Bitmap Pointer
#define APOLLO_SAGA_PIP_MODULO	0xDFF3DE	// Chunky Bitmap Modulo (Bytes skipped after each Row)

#define APOLLO_SAGA_PIP_X_START 0xDFF3D0	// X-Start-Position 
#define APOLLO_SAGA_PIP_Y_START 0xDFF3D2	// Y-Start-Position
#define APOLLO_SAGA_PIP_X_STOP	0xDFF3D4	// X-Stop-Position
#define APOLLO_SAGA_PIP_Y_STOP	0xDFF3D6	// Y-Stop Position
#define APOLLO_SAGA_PIP_CLRKEY	0xDFF3E0	// ColorKey | Bit[15]=Enable Bit[8-11]=Red Bit[4-7]=Green Bit[0-3]=Blue | Enable -> PiP is only shown on Colorkey pixels
#define APOLLO_SAGA_PIP_DMAROWS	0xDFF3E2	// DMA Row Fetch in Bytes = Number of Bytes per Row 

#define APOLLO_SAGA_PIP_TRANSON 0x0100      // Bit[8] APOLLO_SAGA_PIP_GFXMODE Enable Transparency

#define APOLLO_SAGA_PIP_TRANS8				// Transparency Color for 8-Bit RGB-Indexed = ????
#define APOLLO_SAGA_PIP_TRANS15 			// Transparency Color for 15-Bit 1R5G5B5 = 0 11111 00000 11111
#define APOLLO_SAGA_PIP_TRANS16 0xF81F		// Transparency Color for 16-Bit R5G6B5	= 11111 000000 11111

// Apollo SAGA Display Resolutions
#define APOLLO_SAGA_304_224		0x0900      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  

#define APOLLO_SAGA_320_200		0x0100      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_320_240		0x0200      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_320_256		0x0300      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE 

#define APOLLO_SAGA_480_270		0x0800      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE 

#define APOLLO_SAGA_640_200		0x1000      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_640_360		0x0B00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_640_400		0x0400      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_640_480		0x0500      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_640_512		0x0600      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  

#define APOLLO_SAGA_720_405		0x1500      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_720_576		0x0E00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  

#define APOLLO_SAGA_800_600		0x0C00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_848_480		0x0F00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE

#define APOLLO_SAGA_960_540		0x0700      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE

#define APOLLO_SAGA_1024_768	0x0D00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE

#define APOLLO_SAGA_1280_720	0x0A00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_1280_800	0x1300      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_1280_1024	0x1200      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE

#define APOLLO_SAGA_1440_900	0x1400      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_1920_1080	0x1100      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE

// Apollo SAGA Color Format
#define APOLLO_SAGA_8_INDEX		0x0001      // Bit[0-7] APOLLO_SAGA_PIP_GFXMODE & APOLLO_SAGA_PIP_GFXMODE 
#define APOLLO_SAGA_16_R5G6B5	0x0002      // Bit[0-7] APOLLO_SAGA_PIP_GFXMODE & APOLLO_SAGA_PIP_GFXMODE 
#define APOLLO_SAGA_15_1R5G5B5	0x0003      // Bit[0-7] APOLLO_SAGA_PIP_GFXMODE & APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_24_R8G8B8	0x0004		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_SAGA_32_A8R8G8B8	0x0005		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_SAGA_YUV_888		0x0006		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_SAGA_YUV_444		0x000E		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_ATARI_1BIT		0x0008		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_ATARI_2BIT		0x0009		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_ATARI_4BIT		0x000A		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_ATARI_8BIT		0x000B		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP

// Apollo Picture Return Codes
#define APOLLO_PICTURE_OK       0x00
#define APOLLO_PICTURE_W_ERROR  0x01
#define APOLLO_PICTURE_H_ERROR  0x02
#define APOLLO_PICTURE_D_ERROR  0x03
#define APOLLO_PICTURE_LOADERR  0x04
#define APOLLO_PICTURE_NOHEADER 0x05
#define APOLLO_PICTURE_PLANEERR 0x06
#define APOLLO_PICTURE_COMPRERR 0x07
#define APOLLO_PICTURE_MEMERROR 0x09
#define APOLLO_PICTURE_OPENERR  0x0A
#define APOLLO_PICTURE_CLOSEERR 0x0B

// Apollo Sound Return Codes
#define APOLLO_SOUND_OK         0x00
#define APOLLO_SOUND_NOCHANNEL  0x01
#define APOLLO_SOUND_ENDOFFILE  0x03
#define APOLLO_SOUND_LOADERROR  0x04
#define APOLLO_SOUND_NOHEADER   0x05
#define APOLLO_SOUND_MEMERROR   0x09
#define APOLLO_SOUND_OPENERR    0x0A
#define APOLLO_SOUND_CLOSEERR   0x0B
#define APOLLO_SOUND_COMPRERR   0x0C

// Apollo Hardware Sprite Pointer
#define APOLLO_POINTER_SET_X    0xDFF1D0
#define APOLLO_POINTER_SET_Y    0xDFF1D2
#define APOLLO_POINTER_GET_X    0xDFE1D0
#define APOLLO_POINTER_GET_Y    0xDFE1D2

#define APOLLO_MOUSE_GET_X      0xDFF00B
#define APOLLO_MOUSE_GET_Y      0xDFF00A 
#define APOLLO_MOUSE_BUTTON1    0xBFE001
#define APOLLO_MOUSE_BUTTON23   0xDFF016
#define APOLLO_MOUSE_WHEEL      0xDFF212

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

#define APOLLO_POINTER_COL0     0xDFF3A0        // CLUT[4] SAGA Pointer Color Table
#define APOLLO_POINTER_COL1     0xDFF3A2
#define APOLLO_POINTER_COL2     0xDFF3A4
#define APOLLO_POINTER_COL3     0xDFF3A6

#define APOLLO_POINTER_DATA     0xDFF800        // PLANAR[1] SAGA Pointer Bitmap [16x16]
#define APOLLO_POINTER_BITMAP   {0xFC00,0xFC00,0xFE00,0x8200,0x8600,0xFA00,0x8C00,0xF400,0x8600,0xFA00,0x9300,0xFD00,0x6980,0x6E80,0x04C0,0x0740,0x0260,0x03A0,0x0140,0x01C0,0x0080,0x0080,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};

// Register Passing Macros
#define REGP(reg, p)	p __asm(#reg)

#define _A0(param) REGP(a0, param)
#define _A1(param) REGP(a1, param)
#define _A2(param) REGP(a2, param)
#define _A3(param) REGP(a3, param)
#define _A4(param) REGP(a4, param)
#define _A5(param) REGP(a5, param)
#define _A6(param) REGP(a6, param)
#define _A7(param) REGP(a7, param)

#define _D0(param) REGP(d0, param)
#define _D1(param) REGP(d1, param)
#define _D2(param) REGP(d2, param)
#define _D3(param) REGP(d3, param)
#define _D4(param) REGP(d4, param)
#define _D5(param) REGP(d5, param)
#define _D6(param) REGP(d6, param)
#define _D7(param) REGP(d7, param)

#define _FP0(param) REGP(fp0, param)
#define _FP1(param) REGP(fp1, param)
#define _FP2(param) REGP(fp2, param)
#define _FP3(param) REGP(fp3, param)
#define _FP4(param) REGP(fp4, param)
#define _FP5(param) REGP(fp5, param)
#define _FP6(param) REGP(fp6, param)
#define _FP7(param) REGP(fp7, param)


// Amiga Registers
#define INTENAR			0xDFF01C
#define INTREQR			0xDFF01E
#define INTENA			0xDFF09A
#define INTREQ			0xDFF09C

#define BPLCON0			0xDFF100
#define BPL1DAT			0xDFF110
#define COLOR00			0xDFF180
#define COLOR01			0xDFF182
#define COLOR02			0xDFF184
#define COLOR03			0xDFF186
#define COLOR04			0xDFF188
#define COLOR05			0xDFF18A
#define COLOR06			0xDFF18C
#define COLOR07			0xDFF18E
#define COLOR08			0xDFF190
#define COLOR09			0xDFF192
#define COLOR10			0xDFF194
#define COLOR11			0xDFF196
#define COLOR12			0xDFF198
#define COLOR13			0xDFF19A
#define COLOR14			0xDFF19C
#define COLOR15			0xDFF19E
#define COLOR16         0xDFF1A0
#define COLOR17         0xDFF1A2
#define COLOR18         0xDFF1A4
#define COLOR19         0xDFF1A6
#define COLOR20         0xDFF1A8
#define COLOR21         0xDFF1AA
#define COLOR22         0xDFF1AC
#define COLOR23         0xDFF1AE
#define COLOR24         0xDFF1B0
#define COLOR25         0xDFF1B2
#define COLOR26         0xDFF1B4
#define COLOR27         0xDFF1B6
#define COLOR28         0xDFF1B8
#define COLOR29         0xDFF1BA
#define COLOR30         0xDFF1BC
#define COLOR31         0xDFF1BE                       

#define SERDAT			0xDFF030
#define ADKCON			0xDFF09E
#define SERDATR			0xDFF018
#define SERPER			0xDFF032



