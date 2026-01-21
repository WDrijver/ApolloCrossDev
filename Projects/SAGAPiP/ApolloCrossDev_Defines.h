// Apollo V4 SAGA libraries
// Willem Drijver

// Apollo Audi (ARNE)
#define APOLLO_AIFF_OFFSET		48

// Apollo Video (ISABELLE)
#define APOLLO_SAGA_GFXMODE		0xDFF1F4	// Bit[8-15]=SAGA Display Resolution + Bit[0-7]=Color Format 
#define APOLLO_SAGA_POINTER		0xDFF1EC	// Chunky Bitmap Pointer  
#define APOLLO_SAGA_MODULO		0xDFF1E6	// Chunky Bitmap Modulo (Bytes skipped after each Row)

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
#define APOLLO_SAGA_640_400		0x0400      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_640_480		0x0500      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_800_600		0x0C00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_848_480		0x0F00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_960_540		0x0700      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_1024_768	0x0D00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE
#define APOLLO_SAGA_1280_720	0x0A00      // Bit[8-15] APOLLO_SAGA_PIP_GFXMODE

// Apollo SAGA Color Format
#define APOLLO_SAGA_8_INDEX		0x0001      // Bit[0-7] APOLLO_SAGA_PIP_GFXMODE & APOLLO_SAGA_PIP_GFXMODE 
#define APOLLO_SAGA_16_R5G6B5	0x0002      // Bit[0-7] APOLLO_SAGA_PIP_GFXMODE & APOLLO_SAGA_PIP_GFXMODE 
#define APOLLO_SAGA_15_1R5G5B5	0x0003      // Bit[0-7] APOLLO_SAGA_PIP_GFXMODE & APOLLO_SAGA_PIP_GFXMODE  
#define APOLLO_SAGA_24_R8G8B8	0x0004		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_SAGA_32_A8R8G8B8	0x0005		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_SAGA_YUV_888		0x0006		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP
#define APOLLO_SAGA_YUV_444		0x000E		// Bit[0-7] APOLLO_SAGA_PIP_GFXMODE - Not Supported on PiP

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