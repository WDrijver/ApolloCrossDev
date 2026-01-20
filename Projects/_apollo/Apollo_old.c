// Apollo V4 SAGA libraries
// Willem Drijver

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "Apollo.h"
#include "ApolloDebug.h"
#include "clib/exec_protos.h"

char ApolloDebugText[120];

extern ULONG ApolloMemoryFree;

void ApolloLoad(const char *filename, uint8_t **buffer, uint32_t *lenght, uint16_t offset)
{
	static uint8_t *file_buffer = 0;						// raw input buffer	
	static uint8_t *file_buffer_aligned = 0;				// 64-bit aligned input buffer	
	
	static unsigned long file_size = 0;    					// length of file
	static unsigned long file_read = 0;   					// bytes read from file
	static FILE *file_handle = 0;          					// stream of file 

    #ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloLoad: Opening File = ");
	DebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLODEBUG_APOLLO
	   	DebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
		DebugPutStr( "=SUCCESS\n");
		#endif
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size=ftell(file_handle);							// retrieve filesize

	fseek(file_handle, offset, SEEK_SET);
	file_size-=offset;

	#ifdef APOLLODEBUG_APOLLO
    sprintf(ApolloDebugText,"ApolloLoad: File size = %d BYTES\n", file_size);
    DebugPutStr(ApolloDebugText);
	#endif

	file_buffer = (uint8_t*)AllocVec(file_size+15, MEMF_ANY);
    
    #ifdef APOLLODEBUG_MEMORY
    ApolloMemoryFree = AvailMem(MEMF_ANY); 
    sprintf(ApolloDebugText,"# ApolloLoad               | D= -%3d | M= %3d |\n", (file_size+15)>>20, ApolloMemoryFree>>20);
    DebugPutStr(ApolloDebugText);
    #endif

    if (!file_buffer)
    {
        #ifdef APOLLODEBUG_APOLLO
    	DebugPutStr( "ApolloLoad: buffer memory ERROR\n");
		#endif
		return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
		DebugPutStr( "ApolloLoad: buffer memory allocated\n");
		#endif
    }

	file_buffer_aligned = (uint8_t*)(((uint32_t)(file_buffer+15) & 0xFFFFFFF0));

	#ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloLoad: Reading file -> ");
	#endif

	file_read = fread(file_buffer_aligned, 1, file_size, file_handle);
    if(file_read != file_size)
    {
		#ifdef APOLLODEBUG_APOLLO
	   	DebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
	   	DebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLODEBUG_APOLLO
    DebugPutStr("ApolloLoad: Closing file -> ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLODEBUG_APOLLO
	   	DebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLODEBUG_APOLLO
	   	DebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

	*lenght = file_size;
	*buffer = file_buffer_aligned;

	#ifdef APOLLODEBUG_APOLLO
	sprintf(ApolloDebugText, "ApolloLoad: buffer = %d | aligned buffer =%d | filesize =%d |\n", file_buffer, file_buffer_aligned, file_size);
	DebugPutStr(ApolloDebugText);
	#endif
}

void ApolloShowFile(const char *filename, uint8_t **buffer_draw, uint8_t **buffer_live, uint32_t lenght, uint16_t offset, uint16_t gfx_mode, uint16_t gfx_modulo, bool endianswap)
{
	static unsigned long file_size = 0;
	static unsigned long file_read = 0;
	static FILE *file_handle = 0; 

	uint8_t *buffer_temp;

    #ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloShowFile: Opening File = ");
	DebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
		DebugPutStr( "=SUCCESS\n");
		#endif
    }

	fseek(file_handle, offset, SEEK_SET);

    if (*buffer_draw == NULL) DebugPutStr( "ApolloShowFile: buffer memory ERROR\n");

	#ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloShowFile: Reading file -> ");
	#endif

	file_read = fread(*buffer_draw, 1, lenght, file_handle);
    if(file_read != lenght)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLODEBUG_APOLLO
    	DebugPutStr("ApolloShowFile: Closing file -> ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

	if (endianswap) ApolloEndianSwap8Loop((UWORD*)*buffer_draw, (ULONG)lenght);

	buffer_temp = *buffer_live;
	*buffer_live = *buffer_draw;
	*buffer_draw = buffer_temp;

	*((volatile uint16_t*)0xDFF1E6) = (uint16_t)(gfx_modulo); 
	*((volatile uint16_t*)0xDFF1F4) = (uint16_t)(gfx_mode);
	*((volatile uint32_t*)0xDFF1EC) = (uint32_t)(*buffer_live);
}

void ApolloPlayFile(const char *filename, uint8_t *buffer, uint16_t offset, int channel, int volume_left, int volume_right, bool loop)
{
	static unsigned long file_size = 0;
	static unsigned long file_read = 0;
	static FILE *file_handle = 0; 

    #ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloPlayFile: Opening File = ");
	DebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
		DebugPutStr( "=SUCCESS\n");
		#endif
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size=ftell(file_handle);							// retrieve filesize

	fseek(file_handle, offset, SEEK_SET);
	file_size-=offset;

    if (buffer == NULL) DebugPutStr( "ApolloPlayFile: buffer memory ERROR\n");

	#ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloPlayFile: Reading file -> ");
	#endif

	file_read = fread(buffer, 1, file_size, file_handle);
    if(file_read != file_size)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLODEBUG_APOLLO
    	DebugPutStr("ApolloPlayFile: Closing file -> ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

	// If channel is busy, stop channel
	if ( (channel < 4) && (*((volatile uint16_t*)0xDFF002) & (1<<channel)) || (channel >=4) && (*((volatile uint16_t*)0xDFF202) & (1<<channel)))
	{
        if (channel < 4)
        { 
            *((volatile uint16_t*)0xDFF096) = (uint16_t)(0x0000) + (1<<channel);                // DMACON = clear AUD0-3 (stop current stream)    
        } else {
            *((volatile uint16_t*)0xDFF296) = (uint16_t)(0x0000) + (1<<(channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
        }
	}

	#ifdef APOLLODEBUG_APOLLO
	sprintf(ApolloDebugText, "ApolloPlayFile: Channel = %d | L=%d | O=%d | Vol-L = %d | Vol-R = %d | Loop = %d \n", channel, file_size, offset, volume_left, volume_right, loop);
	DebugPutStr(ApolloDebugText);
	#endif

	*((volatile uint32_t*)(0xDFF400 + (channel * 0x10))) = (uint32_t)buffer+offset;        	   	// Set Channel Pointer
	*((volatile uint32_t*)(0xDFF404 + (channel * 0x10))) = (uint32_t)(((file_size-offset)/8)-32);	// Set Channel Music Lenght (in pairs of stereo sample = 2 * 2 * 16-bit = 64-bit chunksize = filesize in bytes / 8)
	
	int volume = (volume_left << 8) + (volume_right);

	#ifdef APOLLODEBUG_APOLLO
	DebugPutHex("ApolloPlayFile: Volume", volume);
	#endif

	*((volatile uint16_t*)(0xDFF408 + (channel * 0x10))) = (uint16_t)volume;                // Set Channel Volume (0-FF / 0-FF) 

	if (loop)
	{
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0005;            // %0101 = $05 - Sample 16bit (bit0 = 1) / OneShot Disabled (bit1 = 0) / Stereo Enabled (bit2 = 1)
	} else {
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0007;            // %0111 = $07 - Sample 16bit (bit0 = 1) / OneShot Enabled (bit1 = 1) / Stereo Enabled (bit2 = 1)
	}
	*((volatile uint16_t*)(0xDFF40C + (channel * 0x10))) = (uint16_t)80;					// PERIOD=44.1 Khz
	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<channel);                // DMACON = enable DMA and enable DMA for specific channel AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(channel-4));            // DMACON = enable DMA and enable DMA for specific channel AUD4-15 (start current stream)       
	}
}

void ApolloCacheFile(const char *filename, uint8_t **cache, uint32_t *lenght, uint16_t file_offset)
{
	static unsigned long file_size = 0;
	static unsigned long file_read = 0;
	static FILE *file_handle = 0; 

    #ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloCacheFile: Opening File = ");
	DebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
		DebugPutStr( "=SUCCESS\n");
		#endif
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size=ftell(file_handle);							// retrieve filesize

	fseek(file_handle, file_offset, SEEK_SET);
	file_size-=file_offset;

    if (*cache == NULL) DebugPutStr( "ApolloCacheFile: *cache memory ERROR\n");

	#ifdef APOLLODEBUG_APOLLO
	DebugPutStr("ApolloCacheFile: Reading file -> ");
	#endif

	file_read = fread(*cache, 1, file_size, file_handle);
    if(file_read != file_size)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLODEBUG_APOLLO
    	DebugPutStr("ApolloCacheFile: Closing file -> ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLODEBUG_APOLLO
	    DebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

    *lenght = file_size;
}


bool ApolloPlay(int channel, int volume_left, int volume_right, bool loop, uint8_t *buffer, uint32_t lenght, uint16_t offset)
{
	// Check if channel is free
	if ( (channel < 4) && (*((volatile uint16_t*)0xDFF002) & (1<<channel)) || (channel >=4) && (*((volatile uint16_t*)0xDFF202) & (1<<channel)))
	{
		return false;
	}

	#ifdef APOLLODEBUG_APOLLO
		sprintf(ApolloDebugText, "ApolloPlay: Channel = %d | L=%d | O=%d | Vol-L = %d | Vol-R = %d | Loop = %d \n", channel, lenght, offset, volume_left, volume_right, loop);
		DebugPutStr(ApolloDebugText);
	#endif

	*((volatile uint32_t*)(0xDFF400 + (channel * 0x10))) = (uint32_t)buffer+offset;        	   	// Set Channel Pointer
	*((volatile uint32_t*)(0xDFF404 + (channel * 0x10))) = (uint32_t)(((lenght-offset)/8)-32);	// Set Channel Music Lenght (in pairs of stereo sample = 2 * 2 * 16-bit = 64-bit chunksize = filesize in bytes / 8)
	
	ApolloVolume(channel, volume_left, volume_right);

	if (loop)
	{
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0005;            // %0101 = $05 - Sample 16bit (bit0 = 1) / OneShot Disabled (bit1 = 0) / Stereo Enabled (bit2 = 1)
	} else {
		*((volatile uint16_t*)(0xDFF40A + (channel * 0x10))) = (uint16_t)0x0007;            // %0111 = $07 - Sample 16bit (bit0 = 1) / OneShot Enabled (bit1 = 1) / Stereo Enabled (bit2 = 1)
	}
	*((volatile uint16_t*)(0xDFF40C + (channel * 0x10))) = (uint16_t)80;					// PERIOD=44.1 Khz
	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<channel);                // DMACON = enable DMA and enable DMA for specific channel AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(channel-4));            // DMACON = enable DMA and enable DMA for specific channel AUD4-15 (start current stream)       
	}
}

void ApolloFadeOut(int channel, int volume_start, int volume_end)
{
	for (;volume_start > volume_end;volume_start--)
	{
		ApolloVolume(channel, volume_start, volume_start);
		ApolloWaitVBL();
	}
	ApolloStop(channel);
}

void ApolloStop(int channel)
{
	#ifdef APOLLODEBUG_APOLLO
		DebugPutDec("ApolloStop: Stopping audio on channel", channel);
	#endif

	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x0000) + (1<<channel);                // DMACON = clear AUD0-3 (stop current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x0000) + (1<<(channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
	}
}


void ApolloStart(int channel)
{
	#ifdef APOLLODEBUG_APOLLO
		DebugPutDec("ApolloStop: Starting audio on channel", channel);
	#endif

	if (channel < 4)
	{ 
		*((volatile uint16_t*)0xDFF096) = (uint16_t)(0x8000) + (1<<channel);                // DMACON = enable AUD0-3 (start current stream)    
	} else {
		*((volatile uint16_t*)0xDFF296) = (uint16_t)(0x8000) + (1<<(channel-4));            // DMACON2 = enable AUD4-15 (start current stream)      
	}
}

void ApolloVolume(int channel, int volume_left, int volume_right)
{
	int volume = (volume_left << 8) + (volume_right);

	#ifdef APOLLODEBUG_APOLLO
		DebugPutHex("ApolloPlay: Volume", volume);
	#endif

	*((volatile uint16_t*)(0xDFF408 + (channel * 0x10))) = (uint16_t)volume;                // Set Channel Volume (0-FF / 0-FF) 
}

void ApolloShow(uint16_t gfx_mode, uint16_t gfx_modulo, uint8_t *buffer)
{
	#ifdef APOLLODEBUG_APOLLO
		sprintf(ApolloDebugText, "ApolloShow: gfxmode = %x | gfx_modulo = % d | buffer = %d |\n", gfx_mode, gfx_modulo, (uint32_t)(buffer));
		DebugPutStr(ApolloDebugText);
	#endif
	*((volatile uint16_t*)0xDFF1E6) = (uint16_t)(gfx_modulo); 
	*((volatile uint16_t*)0xDFF1F4) = (uint16_t)(gfx_mode);
	*((volatile uint32_t*)0xDFF1EC) = (uint32_t)(buffer);
}

void ApolloSwap16(uint8_t *buffer, uint32_t lenght)
{
	uint8_t 	*swapbyte;
	uint32_t	i;

	for (i=0 ; i < lenght ; i+=2)
	{
		*swapbyte = *(buffer+i);
		*(buffer+i) = *(buffer+i+1);
		*(buffer+i+1) = *swapbyte;
	}
}

void ApolloSwap16Transparent(uint8_t *buffer, uint32_t lenght)
{
	uint8_t 	*swapbyte;
	uint32_t	i;

	for (i=0 ; i < lenght ; i+=2)
	{
		*swapbyte = *(buffer+i);
		*(buffer+i) = *(buffer+i+1);
		*(buffer+i+1) = *swapbyte;
		if (*(buffer+i) == 0x07 && *(buffer+i+1) == 0xC0)
		{
			*(buffer+i) = 0xF8;
			*(buffer+i+1) = 0x1F;
		}
	}
}

void ApolloSwapShadow(UWORD *buffer, ULONG lenght, UWORD shadow)
{
	ULONG	i;

	lenght >>=1;

	DebugPutDec("lenght", lenght);

	for (i=2 ; i < lenght ; i++)
	{
		if (*(buffer+i) == 0x001F)
		{
			DebugPutStr("0X1F Replaced\n");
			*(buffer+i) = 0x2964;
		}
	}
}


void ApolloWaitVBL()
{
	asm volatile(
		"1: 		btst	#5,0xdff01f			\n"
		"			beq		1b					\n"
		"			move.w 	#0x0020,0xdff09c	\n"
	);
}


void ApolloJoypad(ApolloJoypadState *JoypadState)
{
	uint16_t * const Joypad_Pointer  = (uint16_t*)0xDFF220;
	uint16_t Joypad_Value;
	
	Joypad_Value = *Joypad_Pointer;

	#ifdef APOLLODEBUG_APOLLO
	if (Joypad_Value > 1) DebugPutHex("Joypad Value", Joypad_Value-1);
	#endif

	if ((Joypad_Value & 0x8000) == 0x8000) JoypadState->Joypad_X_Delta = 1;
	else if ((Joypad_Value & 0x4000) == 0x4000) JoypadState->Joypad_X_Delta = -1;
	else JoypadState->Joypad_X_Delta = 0;

	if ((Joypad_Value & 0x2000) == 0x2000) JoypadState->Joypad_Y_Delta = 1;
	else if ((Joypad_Value & 0x1000) == 0x1000) JoypadState->Joypad_Y_Delta = -1;
	else JoypadState->Joypad_Y_Delta = 0;
			
	if ((Joypad_Value & 0x0400) == 0x0400) JoypadState->Joypad_Start = true; else JoypadState->Joypad_Start = false;
	if ((Joypad_Value & 0x0200) == 0x0200) JoypadState->Joypad_Back = true; else JoypadState->Joypad_Back = false;
			
	if ((Joypad_Value & 0x0100) == 0x0100) JoypadState->Joypad_TR = true; else JoypadState->Joypad_TR = false;
	if ((Joypad_Value & 0x0080) == 0x0080) JoypadState->Joypad_TL = true; else JoypadState->Joypad_TL = false;
	if ((Joypad_Value & 0x0040) == 0x0040) JoypadState->Joypad_BR = true; else JoypadState->Joypad_BR = false;
	if ((Joypad_Value & 0x0020) == 0x0020) JoypadState->Joypad_BL = true; else JoypadState->Joypad_BL = false;
	if ((Joypad_Value & 0x0010) == 0x0010) JoypadState->Joypad_Y = true; else JoypadState->Joypad_Y = false;
	if ((Joypad_Value & 0x0008) == 0x0008) JoypadState->Joypad_X = true; else JoypadState->Joypad_X = false;
	if ((Joypad_Value & 0x0004) == 0x0004) JoypadState->Joypad_B = true; else JoypadState->Joypad_B = false;
	if ((Joypad_Value & 0x0002) == 0x0002) JoypadState->Joypad_A = true; else JoypadState->Joypad_A = false;
	if ((Joypad_Value & 0x0001) == 0x0001) JoypadState->Joypad_Connect = true; else JoypadState->Joypad_Connect = false;
}


void ApolloMouse(ApolloMouseState *MouseState)
{
	uint8_t		MouseButtonLeft_Value;
	uint16_t 	MouseButtonRight_Value;	
	
	MouseButtonLeft_Value	=  *((volatile uint8_t*)APOLLO_MOUSE_BUTTON1);
	if ((MouseButtonLeft_Value & 0x40) == 0) MouseState->Mouse_Button1 = true;

	MouseButtonRight_Value	= *((volatile uint16_t*)APOLLO_MOUSE_BUTTON2);
	if ((MouseButtonRight_Value & 0x400) == 0) MouseState->Mouse_Button2 = true;

	MouseState->Mouse_X_New = *((signed char *)APOLLO_MOUSE_GET_X);
	MouseState->Mouse_Y_New = *((signed char *)APOLLO_MOUSE_GET_Y);

	MouseState->Mouse_X_Delta = MouseState->Mouse_X_New - MouseState->Mouse_X_Old;
	MouseState->Mouse_Y_Delta = MouseState->Mouse_Y_New - MouseState->Mouse_Y_Old;
	
	MouseState->Mouse_X_Old = MouseState->Mouse_X_New;
	MouseState->Mouse_Y_Old = MouseState->Mouse_Y_New;

	if (MouseState->Mouse_X_Delta < -128) MouseState->Mouse_X_Delta += 256;
	if (MouseState->Mouse_X_Delta >  128) MouseState->Mouse_X_Delta -= 256;
	if (MouseState->Mouse_Y_Delta < -128) MouseState->Mouse_Y_Delta += 256;
	if (MouseState->Mouse_Y_Delta >  128) MouseState->Mouse_Y_Delta -= 256;	

	if (MouseState->Mouse_X_Delta != 0 || MouseState->Mouse_Y_Delta != 0)
	{
		if (MouseState->Mouse_X_Position + MouseState->Mouse_X_Delta < 1)
		{
			MouseState->Mouse_X_Position = 1;
		} else {
			if (MouseState->Mouse_X_Position + MouseState->Mouse_X_Delta > SAGA_X_SIZE)
			{
				MouseState->Mouse_X_Position = SAGA_X_SIZE - 1;
			} else {
				MouseState->Mouse_X_Position += MouseState->Mouse_X_Delta;
			}
		}	

		if (MouseState->Mouse_Y_Position + MouseState->Mouse_Y_Delta <1)
		{
			MouseState->Mouse_Y_Position = 1;
		} else {
			if (MouseState->Mouse_Y_Position + MouseState->Mouse_Y_Delta > SAGA_Y_SIZE)
			{
				MouseState->Mouse_Y_Position = SAGA_Y_SIZE - 1;
			} else {
				MouseState->Mouse_Y_Position += MouseState->Mouse_Y_Delta;	
			}
		}

		*((volatile uint16_t*)APOLLO_POINTER_SET_X) = (uint16_t)(MouseState->Mouse_X_Position) + 16; 
		*((volatile uint16_t*)APOLLO_POINTER_SET_Y) = (uint16_t)(MouseState->Mouse_Y_Position) +  8;  

		#ifdef APOLLODEBUG_APOLLO
		sprintf(ApolloDebugText,"Mouse_X: %4d | Mouse_X_Delta: %4d | Mouse_Y: %4d | Mouse_Y_Delta: %4d\n",
				MouseState->Mouse_X_Position, MouseState->Mouse_X_Delta, MouseState->Mouse_Y_Position, MouseState->Mouse_Y_Delta);
		DebugPutStr(ApolloDebugText);
		#endif
	} 
}


void ApolloKeyboard(unsigned char *Keyboard_Key)
{
	unsigned char * const 	Keyboard_Pointer = (unsigned char *)0xBFEC01; 
	unsigned char			Keyboard_Raw, Keyboard_Now;

	Keyboard_Raw = *Keyboard_Pointer;										// retrieve RAW value from register
	Keyboard_Raw = ~Keyboard_Raw;											// not.b
	Keyboard_Now = (uint8_t) ((Keyboard_Raw>>1) | (Keyboard_Raw<<7));		// ror.b #1



	if ((Keyboard_Now < 127) && (Keyboard_Now!=Keyboard_Key))
	{
		*Keyboard_Key = Keyboard_Now;					// Use only "Low" KB values (0-127)
		#ifdef APOLLODEBUG_APOLLO
		sprintf(ApolloDebugText,"Keyboard_Now: %d\n", Keyboard_Now);
		DebugPutStr(ApolloDebugText);
		#endif
	} else {
		*Keyboard_Key = 127;
	}
}


UBYTE ApolloKeyboardToUnicode(UBYTE KeyboardAmiga)
{
	UBYTE KeyboardUnicode;

	switch (KeyboardAmiga)
	{
		case 0x0:	return  96;		// '
		case 0x1:	return  49;		// 1
		case 0x2:	return  50;		// 2
		case 0x3:	return  51;		// 3
		case 0x4:	return  52;		// 4
		case 0x5:	return  53;		// 5
		case 0x6:	return  54;		// 6
		case 0x7:	return  55;		// 7
		case 0x8:	return  56;		// 8
		case 0x9:	return  57;		// 9
		case 0xA:	return  48;		// 0
		case 0xB:	return  45;		// -
		case 0xC:	return  61;		// =
		case 0xD:	return  92;		// Backslash

		case 0xF:	return  48;		// Numpad 0
		case 0x10:	return  81;		// Q
		case 0x11:	return  87;		// W
		case 0x12:	return  69;		// E
		case 0x13:	return  82;		// R
		case 0x14:	return  84;		// T
		case 0x15:	return  89;		// Y
		case 0x16:	return  85;		// U
		case 0x17:	return  73;		// I
		case 0x18:	return  79;		// O
		case 0x19:	return  80;		// P
		case 0x1A:	return  91;		// [
		case 0x1B:	return  93;		// ]

		case 0x1D:	return  49;		// Numpad 1
		case 0x1E:	return  50;		// Numpad 2
		case 0x1F:	return  51;		// Numpad 3

		case 0x20:	return  65;		// A
		case 0x21:	return  83;		// S
		case 0x22:	return  68;		// D
		case 0x23:	return  70;		// F
		case 0x24:	return  71;		// G
		case 0x25:	return  72;		// H
		case 0x26:	return  74;		// J
		case 0x27:	return  75;		// K
		case 0x28:	return  76;		// L
		case 0x29:	return  59;		// ;
		case 0x2A:	return  39;		// '

		case 0x2D:	return  52;		// Numpad 4
		case 0x2E:	return  53;		// Numpad 5
		case 0x2F:	return  54;		// Numpad 6

		case 0x31:	return  90;		// Z
		case 0x32:	return  88;		// X
		case 0x33:	return  67;		// C
		case 0x34:	return  86;		// V
		case 0x35:	return  66;		// B
		case 0x36:	return  78;		// N
		case 0x37:	return  77;		// M
		case 0x38:	return  44;		// ,
		case 0x39:	return  46;		// .
		case 0x3A:	return  47;		// Forwardslash
		
		case 0x3C:	return  46;		// Numpad .
		case 0x3D:	return  55;		// Numpad 7
		case 0x3E:	return  56;		// Numpad 8
		case 0x3F:	return  57;		// Numpad 9

		case 0x40:	return  32;		// Space
		case 0x41:	return   8;		// Backspace
		case 0x42:	return   9;		// Tab
		case 0x43:	return  13;		// Enter (CR)
		case 0x44:	return  13;		// Return (CR)

		case 0x45:	return  27;		// Esc
		case 0x46:	return 127;		// Del

		case 0x4A:	return  45;		// -

		case 0x5A:	return  40;		// (
		case 0x5B:	return  41;		// )
		case 0x5C:	return  47;		// Forwardslash
		case 0x5D:	return  42;		// *
	}

	return 0;
}


