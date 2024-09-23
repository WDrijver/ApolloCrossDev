// Apollo V4 Library - Willem Drijver

#include "Apollo.h"

ULONG ApolloMemoryFree;

void ApolloLoad(const UBYTE *filename, UBYTE **buffer, ULONG *buffer_lenght, UWORD offset, bool endianswap)
{
	UBYTE *file_buffer = 0;						// raw input buffer	
	UBYTE *file_buffer_aligned = 0;				// 64-bit aligned input buffer	
	ULONG file_size = 0;    					// length of file
	ULONG file_read = 0;   						// bytes read from file
	FILE *file_handle = 0;          			// stream of file 

    #ifdef APOLLODEBUG
	ApolloDebugPutStr("ApolloLoad: Opening File = ");
	ApolloDebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLODEBUG
	   	ApolloDebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLODEBUG
		ApolloDebugPutStr( "=SUCCESS\n");
		#endif
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size=ftell(file_handle);							// retrieve filesize

	fseek(file_handle, offset, SEEK_SET);
	file_size-=offset;

	file_buffer = (UBYTE*)AllocMem(file_size+15, MEMF_ANY);
    file_buffer_aligned = (UBYTE*)(((ULONG)(file_buffer+15) & 0xFFFFFFF0));

    #ifdef APOLLODEBUG
    ApolloMemoryFree = AvailMem(MEMF_ANY); 
    sprintf(ApolloDebugText,"ApolloLoad: AllocMem = %3d Kb | AvailMem = %3d Mb\n", (file_size+15)>>10, ApolloMemoryFree>>20);
    ApolloDebugPutStr(ApolloDebugText);
    #endif

	#ifdef APOLLODEBUG
	ApolloDebugPutStr("ApolloLoad: Reading file = ");
	#endif

	file_read = fread(file_buffer_aligned, 1, file_size, file_handle);
    if(file_read != file_size)
    {
		#ifdef APOLLODEBUG
	   	ApolloDebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLODEBUG
	   	ApolloDebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLODEBUG
    ApolloDebugPutStr("ApolloLoad: Closing file = ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLODEBUG
	   	ApolloDebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLODEBUG
	   	ApolloDebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

	if (endianswap) ApolloEndianSwap8(file_buffer_aligned, file_size);

	*buffer_lenght = file_size;
	*buffer = file_buffer_aligned;

	#ifdef APOLLODEBUG
	sprintf(ApolloDebugText, "ApolloLoad: buffer = %d | aligned buffer =%d | filesize =%d | swap = %s\n", file_buffer, file_buffer_aligned, file_size, endianswap? "true":"false");
	ApolloDebugPutStr(ApolloDebugText);
	#endif
}

void ApolloShow(UBYTE *buffer, ULONG buffer_lenght, UWORD gfx_mode, UWORD gfx_modulo)
{
	#ifdef APOLLODEBUG
	sprintf(ApolloDebugText, "ApolloShow: SAGA Mode = %d | L = %d | M = %d\n", gfx_mode, buffer_lenght, gfx_modulo);
	ApolloDebugPutStr(ApolloDebugText);
	#endif

	*((volatile UWORD*)APOLLO_SAGAMODULO_REG)	= (UWORD)(gfx_modulo); 
	*((volatile UWORD*)APOLLO_SAGAMODE_REG) 	= (UWORD)(gfx_mode);
	*((volatile ULONG*)APOLLO_SAGADISPLAY_REG) 	= (ULONG)(buffer);
}

void ApolloPlay(UBYTE *buffer, ULONG buffer_lenght, UWORD channel, UWORD volume_left, UWORD volume_right, bool loop)
{
	// If channel is busy, stop channel
	if ( (channel < 4) && (*((volatile UWORD*)0xDFF002) & (1<<channel)) || (channel >=4) && (*((volatile UWORD*)0xDFF202) & (1<<channel)))
	{
        if (channel < 4)
        { 
            *((volatile UWORD*)0xDFF096) = (UWORD)(0x0000) + (1<<channel);                // DMACON = clear AUD0-3 (stop current stream)    
        } else {
            *((volatile UWORD*)0xDFF296) = (UWORD)(0x0000) + (1<<(channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
        }
	}

	#ifdef APOLLODEBUG
	sprintf(ApolloDebugText, "ApolloPlay: Channel = %d | L=%d | Vol-L = %d | Vol-R = %d | Loop = %s \n", channel, buffer_lenght, volume_left, volume_right, loop? "true":"false");
	ApolloDebugPutStr(ApolloDebugText);
	#endif

	*((volatile ULONG*)(0xDFF400 + (channel * 0x10))) = (ULONG)buffer;        	   			// Set Channel Pointer
	*((volatile ULONG*)(0xDFF404 + (channel * 0x10))) = (ULONG)(((buffer_lenght)/8)-32);	// Set Channel Music Lenght (in pairs of stereo sample = 2 * 2 * 16-bit = 64-bit chunksize = filesize in bytes / 8)
	
	UWORD volume = (volume_left << 8) + (volume_right);

	#ifdef APOLLODEBUG
	ApolloDebugPutHex("ApolloPlayFile: Volume", volume);
	#endif

	*((volatile UWORD*)(0xDFF408 + (channel * 0x10))) = (UWORD)volume;                // Set Channel Volume (0-FF / 0-FF) 

	if (loop)
	{
		*((volatile UWORD*)(0xDFF40A + (channel * 0x10))) = (UWORD)0x0005;            // %0101 = $05 - Sample 16bit (bit0 = 1) / OneShot Disabled (bit1 = 0) / Stereo Enabled (bit2 = 1)
	} else {
		*((volatile UWORD*)(0xDFF40A + (channel * 0x10))) = (UWORD)0x0007;            // %0111 = $07 - Sample 16bit (bit0 = 1) / OneShot Enabled (bit1 = 1) / Stereo Enabled (bit2 = 1)
	}
	*((volatile UWORD*)(0xDFF40C + (channel * 0x10))) = (UWORD)80;					// PERIOD=44.1 Khz
	if (channel < 4)
	{ 
		*((volatile UWORD*)0xDFF096) = (UWORD)(0x8000) + (1<<channel);                // DMACON = enable DMA and enable DMA for specific channel AUD0-3 (start current stream)    
	} else {
		*((volatile UWORD*)0xDFF296) = (UWORD)(0x8000) + (1<<(channel-4));            // DMACON = enable DMA and enable DMA for specific channel AUD4-15 (start current stream)       
	}
}

void ApolloCacheFile(const UBYTE *filename, UBYTE **cache, ULONG *lenght, UWORD file_offset)
{
	static ULONG file_size = 0;
	static ULONG file_read = 0;
	static FILE *file_handle = 0; 

    #ifdef APOLLODEBUG
	ApolloDebugPutStr("ApolloCacheFile: Opening File = ");
	ApolloDebugPutStr(filename);
	#endif

    file_handle = fopen(filename, "rb");
    if (!file_handle)
    {
		#ifdef APOLLODEBUG
	    ApolloDebugPutStr( "=ERROR\n");
		#endif
	    return;
    } else {
		#ifdef APOLLODEBUG
		ApolloDebugPutStr( "=SUCCESS\n");
		#endif
    }

    fseek(file_handle, 0, SEEK_END);						// goto end of file
    file_size=ftell(file_handle);							// retrieve filesize

	fseek(file_handle, file_offset, SEEK_SET);
	file_size-=file_offset;

    if (*cache == NULL) ApolloDebugPutStr( "ApolloCacheFile: *cache memory ERROR\n");

	#ifdef APOLLODEBUG
	ApolloDebugPutStr("ApolloCacheFile: Reading file -> ");
	#endif

	file_read = fread(*cache, 1, file_size, file_handle);
    if(file_read != file_size)
    {
		#ifdef APOLLODEBUG
	    ApolloDebugPutStr( "ERROR: cannot load file\n");
		#endif	
	    return;
    } else {
		#ifdef APOLLODEBUG
	    ApolloDebugPutStr( "SUCCESS: file loaded\n");
		#endif
    }

	#ifdef APOLLODEBUG
    	ApolloDebugPutStr("ApolloCacheFile: Closing file -> ");
	#endif

    if (fclose(file_handle) == EOF)
    {
		#ifdef APOLLODEBUG
	    ApolloDebugPutStr( "ERROR: cannot close file\n");
		#endif
    } else {
		#ifdef APOLLODEBUG
	    ApolloDebugPutStr( "SUCCESS: file closed\n");
		#endif
    }

    *lenght = file_size;
}


void ApolloFadeOut(UWORD channel, UWORD volume_start, UWORD volume_end)
{
	for (;volume_start > volume_end;volume_start--)
	{
		ApolloVolume(channel, volume_start, volume_start);
		ApolloWaitVBL();
	}
	ApolloStop(channel);
}

void ApolloStop(UWORD channel)
{
	#ifdef APOLLODEBUG
		ApolloDebugPutDec("ApolloStop: Stopping audio on channel", channel);
	#endif

	if (channel < 4)
	{ 
		*((volatile UWORD*)0xDFF096) = (UWORD)(0x0000) + (1<<channel);                // DMACON = clear AUD0-3 (stop current stream)    
	} else {
		*((volatile UWORD*)0xDFF296) = (UWORD)(0x0000) + (1<<(channel-4));            // DMACON2 = clear AUD4-15 (stop current stream)      
	}
}


void ApolloStart(UWORD channel)
{
	#ifdef APOLLODEBUG
		ApolloDebugPutDec("ApolloStop: Starting audio on channel", channel);
	#endif

	if (channel < 4)
	{ 
		*((volatile UWORD*)0xDFF096) = (UWORD)(0x8000) + (1<<channel);                // DMACON = enable AUD0-3 (start current stream)    
	} else {
		*((volatile UWORD*)0xDFF296) = (UWORD)(0x8000) + (1<<(channel-4));            // DMACON2 = enable AUD4-15 (start current stream)      
	}
}

void ApolloVolume(UWORD channel, UWORD volume_left, UWORD volume_right)
{
	UWORD volume = (volume_left << 8) + (volume_right);

	*((volatile UWORD*)(0xDFF408 + (channel * 0x10))) = (UWORD)volume;                // Set Channel Volume (0-FF / 0-FF) 
}


void ApolloSwap16(UBYTE *buffer, ULONG lenght)
{
	UBYTE 	*swapbyte;
	ULONG	i;

	for (i=0 ; i < lenght ; i+=2)
	{
		*swapbyte = *(buffer+i);
		*(buffer+i) = *(buffer+i+1);
		*(buffer+i+1) = *swapbyte;
	}
}

void ApolloSwap16Transparent(UBYTE *buffer, ULONG lenght)
{
	UBYTE 	*swapbyte;
	ULONG	i;

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

	ApolloDebugPutDec("lenght", lenght);

	for (i=2 ; i < lenght ; i++)
	{
		if (*(buffer+i) == 0x001F)
		{
			ApolloDebugPutStr("0X1F Replaced\n");
			*(buffer+i) = 0x2964;
		}
	}
}


void ApolloJoypad(ApolloJoypadState *JoypadState)
{
	UWORD * const Joypad_Pointer  = (UWORD*)0xDFF220;
	UWORD Joypad_Value;
	
	Joypad_Value = *Joypad_Pointer;

	#ifdef APOLLODEBUG
	if (Joypad_Value > 1) ApolloDebugPutHex("Joypad Value", Joypad_Value-1);
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
	UBYTE MouseButtonLeft_Value;	
	UWORD MouseButtonRight_Value;	
	UWORD MouseButtonMiddle_Value;

	// Initialize Mouse Buttons
	MouseState->Button_Left = false;
	MouseState->Button_Right = false;
	MouseState->Button_Middle = false;
	
	// Read Mouse Buttons 
	MouseButtonLeft_Value = *((volatile uint8_t*)APOLLO_MOUSE_BUTTON1);
	if ((MouseButtonLeft_Value & 0x40) == 0) MouseState->Button_Left = true;
	MouseButtonRight_Value= *((volatile uint16_t*)APOLLO_MOUSE_BUTTON2);
	if ((MouseButtonRight_Value & 0x400) == 0) MouseState->Button_Right = true;
	MouseButtonMiddle_Value= *((volatile uint16_t*)APOLLO_MOUSE_BUTTON3);
	if ((MouseButtonMiddle_Value & 0x400) == 0) MouseState->Button_Middle = true;

	// Read Mouse Movement	
	MouseState->MouseX_Value 		= *((signed char *)APOLLO_MOUSE_GET_X);
	MouseState->MouseY_Value 		= *((signed char *)APOLLO_MOUSE_GET_Y);
	MouseState->MouseX_Value_Delta 	= MouseState->MouseX_Value - MouseState->MouseX_Value_Old;
	MouseState->MouseY_Value_Delta 	= MouseState->MouseY_Value - MouseState->MouseY_Value_Old;
	MouseState->MouseX_Value_Old 	= MouseState->MouseX_Value;
	MouseState->MouseY_Value_Old 	= MouseState->MouseY_Value;

	// Correct Delta for BYTE overflow
	if (MouseState->MouseX_Value_Delta < -128) MouseState->MouseX_Value_Delta += 256;
	if (MouseState->MouseX_Value_Delta >  128) MouseState->MouseX_Value_Delta -= 256;
	if (MouseState->MouseY_Value_Delta < -128) MouseState->MouseY_Value_Delta += 256;
	if (MouseState->MouseY_Value_Delta >  128) MouseState->MouseY_Value_Delta -= 256;	

	// Check for Mouse Screen Boundaries
	if (MouseState->MouseX_Value_Delta != 0 || MouseState->MouseY_Value_Delta != 0)
	{
		if (MouseState->MouseX_Pointer + MouseState->MouseX_Value_Delta < 1)
		{
			MouseState->MouseX_Pointer = 1;
		} else {
			if (MouseState->MouseX_Pointer + MouseState->MouseX_Value_Delta > MouseState->MouseX_Pointer_Max)
			{
				MouseState->MouseX_Pointer = MouseState->MouseX_Pointer_Max - 1;
			} else {
				MouseState->MouseX_Pointer += MouseState->MouseX_Value_Delta;
			}
		}	

		if (MouseState->MouseY_Pointer + MouseState->MouseY_Value_Delta <1)
		{
			MouseState->MouseY_Pointer = 1;
		} else {
			if (MouseState->MouseY_Pointer + MouseState->MouseY_Value_Delta >  MouseState->MouseY_Pointer_Max)
			{
				MouseState->MouseY_Pointer = MouseState->MouseY_Pointer_Max - 1;
			} else {
				MouseState->MouseY_Pointer += MouseState->MouseY_Value_Delta;	
			}
		}

		// Reposition MousePointer Sprite
		*((volatile uint16_t*)APOLLO_POINTER_SET_X) = (uint16_t)(MouseState->MouseX_Pointer) + 16; 
		*((volatile uint16_t*)APOLLO_POINTER_SET_Y) = (uint16_t)(MouseState->MouseY_Pointer) +  8;  

		sprintf(ApolloDebugText,"Mouse_X = X-Pos:%4d X-New:%4d X-Old:%4d X-Delta:%4d | Mouse_Y = Y-Pos:%4d Y-New:%4d Y-Old:%4d Y-Delta:%4d\n",
				MouseState->MouseX_Pointer, MouseState->MouseX_Value, MouseState->MouseX_Value_Old, MouseState->MouseX_Value_Delta,
				MouseState->MouseY_Pointer, MouseState->MouseY_Value, MouseState->MouseY_Value_Old, MouseState->MouseY_Value_Delta);
		ApolloDebugPutStr(ApolloDebugText);
	} 

	// Translate Mouse Buttons to Mouse ButtonState
	MouseState->Button_State = 0;

	if( MouseState->Button_Left_Count > 0 ) MouseState->Button_Left_Count--;
	if( MouseState->Button_Right_Count > 0 ) MouseState->Button_Right_Count--;
	if( MouseState->Button_Middle_Count > 0 ) MouseState->Button_Middle_Count--;

	if( MouseState->Button_Left == false && MouseState->Button_Left_Old == true )
	{
		if( MouseState->Button_Left_Count == 0 )
		{
			MouseState->Button_State |= APOLLOMOUSE_LEFTCLICK;
			MouseState->Button_Left_Count = APOLLOMOUSE_DOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->Button_State |= APOLLOMOUSE_LEFTDOUBLECLICK;
		}
	}

	if( MouseState->Button_Left == true )
	{
		MouseState->Button_State |= APOLLOMOUSE_LEFTDOWN;
	}
	
	if( MouseState->Button_Right == false && MouseState->Button_Right_Old == true )
	{
		if( MouseState->Button_Right_Count == 0 )
		{
			MouseState->Button_State |= APOLLOMOUSE_RIGHTCLICK;
			MouseState->Button_Right_Count = APOLLOMOUSE_DOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->Button_State |= APOLLOMOUSE_RIGHTDOUBLECLICK;
		}
	}

	if( MouseState->Button_Right == true )
	{
		MouseState->Button_State |= APOLLOMOUSE_RIGHTDOWN;
	}

	if( MouseState->Button_Middle == false && MouseState->Button_Middle_Old == true )
	{
		if( MouseState->Button_Middle_Count == 0 )
		{
			MouseState->Button_State |= APOLLOMOUSE_MIDDLECLICK;
			MouseState->Button_Middle_Count = APOLLOMOUSE_DOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->Button_State |= APOLLOMOUSE_MIDDLEDOUBLECLICK;
		}
	}

	if( MouseState->Button_Middle == true )
	{
		MouseState->Button_State |= APOLLOMOUSE_MIDDLEDOWN;
	}

	MouseState->Button_Left_Old = MouseState->Button_Left;
	MouseState->Button_Right_Old = MouseState->Button_Right;
	MouseState->Button_Middle_Old = MouseState->Button_Middle;

	return;
}


void ApolloKeyboard(ApolloKeyBoardState *KeyboardState)
{
	UBYTE* const 	Keyboard_Pointer = (UBYTE*)0xBFEC01; 
	UBYTE			Keyboard_Raw, Keyboard_Now;

	Keyboard_Raw = *Keyboard_Pointer;														// retrieve RAW value from register
	Keyboard_Raw = ~Keyboard_Raw;															// not.b
	KeyboardState->Current_Key = (UBYTE) ((Keyboard_Raw>>1) | (Keyboard_Raw<<7));			// ror.b #1

	if ((KeyboardState->Current_Key < 127) && (KeyboardState->Current_Key!=KeyboardState->Previous_Key))
	{
		KeyboardState->Previous_Key = KeyboardState->Current_Key;							// Use only "Low" KB values (0-127)

		sprintf(ApolloDebugText,"Keyboard_Now: %d\n", Keyboard_Now);
		ApolloDebugPutStr(ApolloDebugText);

	} else {
		KeyboardState->Current_Key = 127;
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

void reg_w(ULONG reg, UWORD val)
{
    volatile UWORD *r = (void *)(0xdff000 + reg);

	*r = val;
}

UWORD reg_r(ULONG reg)
{
	volatile UWORD *r = (void *)(0xdff000 + reg);

	return *r;
}

void ApolloDebugInit(void)
{
	/* Set DTR, RTS, etc */
	volatile UBYTE * ciab_pra = (APTR)0xBFD000;
	volatile UBYTE * ciab_ddra = (APTR)0xBFD200;
	*ciab_ddra = 0xc0;  /* Only DTR and RTS are driven as outputs */
	*ciab_pra = 0;      /* Turn on DTR and RTS */

	/* Set the debug UART to 115200 */
	reg_w(SERPER, SERPER_BAUD(SERPER_BASE_PAL, 115200));
}

UWORD ApolloDebugPutChar(register UWORD chr)
{
	if (chr == '\n')
		ApolloDebugPutChar('\r');

	while ((reg_r(SERDATR) & SERDATR_TBE) == 0);
	reg_w(INTREQ, INTF_TBE);

	/* Output a UBYTE to the debug UART */
	reg_w(SERDAT, SERDAT_STP8 | SERDAT_DB8(chr));

	return 1;
}

UWORD ApolloDebugMayGetChar(void)
{
	UWORD c;

	if ((reg_r(SERDATR) & SERDATR_RBF) == 0)
	    return -1;

	c = SERDATR_DB8_of(reg_r(SERDATR));

	/* Clear RBF */
	reg_w(INTREQ, INTF_RBF);

	return c;
}

void ApolloDebugPutStr(register const UBYTE *buff)
{
	for (; *buff != 0; buff++)
		ApolloDebugPutChar(*buff);
}

void ApolloDebugPutDec(const UBYTE *what, ULONG val)
{
	UWORD i, num;
	ApolloDebugPutStr(what);
	ApolloDebugPutStr(": ");
	if (val == 0) {
	    ApolloDebugPutChar('0');
	    ApolloDebugPutChar('\n');
	    return;
	}

	for (i = 1000000000; i > 0; i /= 10) {
	    if (val == 0) {
	    	ApolloDebugPutChar('0');
	    	continue;
	    }

	    num = val / i;
	    if (num == 0)
	    	continue;

	    ApolloDebugPutChar("0123456789"[num]);
	    val -= num * i;
	}
	ApolloDebugPutChar('\n');
}

void ApolloDebugPutHexVal(ULONG val)
{
	UWORD i;
	for (i = 0; i < 8; i ++) {
		ApolloDebugPutChar("0123456789abcdef"[(val >> (28 - (i * 4))) & 0xf]);
	}
	ApolloDebugPutChar(' ');
}

void ApolloDebugPutHex(const UBYTE *what, ULONG val)
{
	ApolloDebugPutStr(what);
	ApolloDebugPutStr(": ");
	ApolloDebugPutHexVal(val);
	ApolloDebugPutChar('\n');
}





