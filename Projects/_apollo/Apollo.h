// Apollo V4 SAGA libraries
// Willem Drijver

#ifdef __cplusplus
extern "C"{
#endif 

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "ApolloDebug.h"

#include "clib/exec_protos.h"

// Apollo Audi (ARNE)
#define APOLLO_AIFF_OFFSET		48

// Apollo Video (ISABELLE)
#define APOLLO_SAGA_GFXMODE		0xDFF1F4	// Bit[8-15]=SAGA Display Resolution + Bit[0-7]=Color Format 
#define APOLLO_SAGA_POINTER		0xDFF1EC	// Chunky Bitmap Pointer  
#define APOLLO_SAGA_MODULO		0xDFF1E6	// Chunky Bitmap Modulo (Bytes skipped after each Row)

#define APOLLO_SAGA_PIP_GFXMODE 0xDFF3DC	// Bit[8-15]=SAGA Display Resolution + Bit[0-7]=Color Format
#define APOLLO_SAGA_PIP_POINTER 0xDFF3D8	// Chunky Bitmap Pointer
#define APOLLO_SAGA_PIP_MODULO	0xDFF3DE	// Chunky Bitmap Modulo (Bytes skipped after each Row)

#define APOLLO_SAGA_PIP_X_START 0xDFF3D0	// X-Start-Position 
#define APOLLO_SAGA_PIP_Y_START 0xDFF3D2	// Y-Start-Position
#define APOLLO_SAGA_PIP_X_STOP	0xDFF3D4	// X-Stop-Position
#define APOLLO_SAGA_PIP_Y_STOP	0xDFF3D6	// Y-Stop Position
#define APOLLO_SAGA_PIP_CLRKEY	0xDFF3E0	// ColorKey | Bit[15]=Enable Bit[8-11]=Red Bit[4-7]=Green Bit[0-3]=Blue | Enable -> PiP is only shown on Colorkey pixels
#define APOLLO_SAGA_PIP_DMAROWS	0xDFF3E2	// DMA Row Fetch in Bytes = Number of Bytes per Row 

#define APOLLO_SAGA_PIP_TRANS8				// Transparency Color for 8-Bit RGB-Indexed = ????
#define APOLLO_SAGA_PIP_TRANS15 			// Transparency Color for 15-Bit 1R5G5B5	= 0 11111 00000 11111
#define APOLLO_SAGA_PIP_TRANS16 0xF81F		// Transparency Color for 16-Bit R5G6B5		= 11111 000000 11111

// Apollo SAGA Display Resolutions
#define APOLLO_SAGA_640_400		0x04
#define APOLLO_SAGA_640_480		0x05
#define APOLLO_SAGA_800_600		0x0C
#define APOLLO_SAGA_848_480		0x0F
#define APOLLO_SAGA_960_540		0x07
#define APOLLO_SAGA_1024_768	0x0D
#define APOLLO_SAGA_1280_720	0x0A

// Apollo SAGA Color Format
#define APOLLO_SAGA_8_INDEX		0x01
#define APOLLO_SAGA_16_R5G6B5	0x02
#define APOLLO_SAGA_15_1R5G5B5	0x03
#define APOLLO_SAGA_24_R8G8B8	0x04		// Not Supported on PiP
#define APOLLO_SAGA_32_A8R8G8B8	0x05		// Not Supported on PiP
#define APOLLO_SAGA_YUV_888		0x06		// Not SUpported on PiP
#define APOLLO_SAGA_YUV_444		0x0E		// Not SUpported on PiP




// Apollo Hardware Sprite Pointer
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

void ApolloLoad(const char *filename, uint8_t **buffer, int*position, uint32_t *lenght, uint16_t offset);
bool ApolloPlay(int channel, int volume_left, int volume_right, bool loop, bool fadein, uint8_t *buffer, uint32_t lenght, uint16_t period, int *channel_chosen);
void ApolloStop(int channel);
void ApolloStart(int channel);
void ApolloFadeIn(int channel, int volume_start, int volume_end);
void ApolloFadeOut(int channel, int volume_start, int volume_end);
void ApolloVolume(int channel, int volume_left, int volume_right);
void ApolloWaitVBL();

#ifdef __cplusplus
}
#endif