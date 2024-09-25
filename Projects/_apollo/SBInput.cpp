//============================================================================================
//SB: SBInput.cpp : Input part of the SB Library
//============================================================================================

#include "SBDatatypes.h"
#include "SBList.h"
#include "SBInput.h"
#include "SBDebug.h"
#include "SBUIDefine.h"
#include <assert.h>
#include "../PrecompiledInclude.h"


SBInput*	SBInput::mpInput;

SBInputMouseState	MouseStateOld;

// ApolloMouse Variables
uint8_t 			MouseButtonLeft_Value;	
uint16_t 			MouseButtonRight_Value;	

signed char  		Mouse_X = 0, Mouse_Y = 0, Mouse_X_Old = 0, Mouse_Y_Old = 0, Mouse_X_Delta = 0, Mouse_Y_Delta = 0;

extern bool			bEnableAnimatedPointer;
extern bool 		bUINeedRefresh;

//--------------------------------------------------------------------------------------------
// SBInput::
//--------------------------------------------------------------------------------------------
// Constructor:
//--------------------------------------------------------------------------------------------
SBInput::SBInput( )
{
	int		i;

	memset( &MouseStateOld, 0, sizeof( MouseStateOld ));

	Mouse_X = *((signed char *)APOLLO_MOUSE_GET_X);
	Mouse_Y = *((signed char *)APOLLO_MOUSE_GET_Y);

	Mouse_X_Old = Mouse_X;
	Mouse_Y_Old = Mouse_Y;

	hasFocus = false;
	
	useSysKeyboard = false;
	bufferedSysKeyboard = false;

	useSysMouse = false;
	bufferedSysMouse = false;

    //SDL_EnableUNICODE(1);

	mpInput = this;
}

SBInput::SBInput( UWORD devices )
{
	// Call standard constructor
	SBInput();

	// Set as active
	SBInput::ApplicationIsActive( true );
}

//--------------------------------------------------------------------------------------------
// Destructor:
//--------------------------------------------------------------------------------------------
SBInput::~SBInput()
{
	UCHAR	cpt;

#if 0
	if( useSysKeyboard == true )
	{
		if( hasFocus == true )
			systemKeyboard->Unacquire();

		systemKeyboard->Release();
	}

	if( useSysMouse == true )
	{
		if( hasFocus == true )
			systemMouse->Unacquire();

		systemMouse->Release();
	}
#endif
}

//--------------------------------------------------------------------------------------------
// ApplicationIsActive: Inform the input wether the application is active or not
//--------------------------------------------------------------------------------------------
bool SBInput::ApplicationIsActive( bool isActive )
{
     error = SBINPUT_NO_ERROR;
     
	// Check if we gain focus
	if( ( isActive == true ) && ( hasFocus == false ) )
    	hasFocus = true;

	if( ( isActive == false ) && ( hasFocus == true ) )
		hasFocus = false;

	return true;
}

/* [WD] Redundant 
//--------------------------------------------------------------------------------------------
// AllocateSystemDevices: Allocate some system devices that are still not allocated
//--------------------------------------------------------------------------------------------
bool SBInput::AllocateSystemDevices( UWORD devices)
{
  	// DebugPutStr( "SBInput::AllocateSystemDevices: \n");
    // Allocating a system keyboard buffered
  	
	// DebugPutDec("System Mouse", useSysMouse);
	// DebugPutDec("Buffered Mouse", bufferedSysMouse);
	// DebugPutDec("System Keyboard", useSysKeyboard);
	// DebugPutDec("Buffered Keyboard", bufferedSysKeyboard);

	return true;
} */

//--------------------------------------------------------------------------------------------
// DeAllocateSystemDevices: Desallocate a system device
//--------------------------------------------------------------------------------------------
bool SBInput::DeAllocateSystemDevices( UWORD devices )
{
    if( (devices & SBINPUT_SYSTEMMOUSE_BUFFERED) || (devices & SBINPUT_SYSTEMMOUSE_UNBUFFERED))
        useSysMouse = false;

    if( (devices & SBINPUT_SYSTEMKEYBOARD_BUFFERED) || (devices & SBINPUT_SYSTEMKEYBOARD_UNBUFFERED))
        useSysKeyboard = false;

    return true;
}

//--------------------------------------------------------------------------------------------
// [WD] ApolloMouse
//--------------------------------------------------------------------------------------------
void SBInput::GetSystemMouseState( SBInputMouseState *MouseState )
{
	// Initialize Mouse Buttons
	MouseState->mouseZ = 0;
	MouseState->button1 = false;
	MouseState->button2 = false;
	MouseState->button3 = false;
	
	// Read Mouse Buttons 
	MouseButtonLeft_Value	=  *((volatile uint8_t*)APOLLO_MOUSE_BUTTON1);
	if ((MouseButtonLeft_Value & 0x40) == 0) MouseState->button1 = true;

	MouseButtonRight_Value	= *((volatile uint16_t*)APOLLO_MOUSE_BUTTON2);
	if ((MouseButtonRight_Value & 0x400) == 0) MouseState->button2 = true;

	// Read Mouse Movement	
	Mouse_X = *((signed char *)APOLLO_MOUSE_GET_X);
	Mouse_Y = *((signed char *)APOLLO_MOUSE_GET_Y);

	Mouse_X_Delta = Mouse_X - Mouse_X_Old;
	Mouse_Y_Delta = Mouse_Y - Mouse_Y_Old;
	
	Mouse_X_Old = Mouse_X;
	Mouse_Y_Old = Mouse_Y;

	if (Mouse_X_Delta < -128) Mouse_X_Delta += 256;
	if (Mouse_X_Delta >  128) Mouse_X_Delta -= 256;
	if (Mouse_Y_Delta < -128) Mouse_Y_Delta += 256;
	if (Mouse_Y_Delta >  128) Mouse_Y_Delta -= 256;	

	if (Mouse_X_Delta != 0 || Mouse_Y_Delta != 0)
	{
		if (MouseState->mouseX + Mouse_X_Delta < 1)
		{
			MouseState->mouseX = 1;
		} else {
			if (MouseState->mouseX + Mouse_X_Delta > SAGA_X_SIZE)
			{
				MouseState->mouseX = SAGA_X_SIZE - 1;
			} else {
				MouseState->mouseX += Mouse_X_Delta;
			}
		}	

		if (MouseState->mouseY + Mouse_Y_Delta <1)
		{
			MouseState->mouseY = 1;
		} else {
			if (MouseState->mouseY + Mouse_Y_Delta > SAGA_Y_SIZE)
			{
				MouseState->mouseY = SAGA_Y_SIZE - 1;
			} else {
				MouseState->mouseY += Mouse_Y_Delta;	
			}
		}

		// Update Apollo Hardware Mousepointer 
		if (!bEnableAnimatedPointer || bUINeedRefresh)
		{
			*((volatile uint16_t*)APOLLO_POINTER_SET_X) = (uint16_t)(MouseState->mouseX) + 16; 
			*((volatile uint16_t*)APOLLO_POINTER_SET_Y) = (uint16_t)(MouseState->mouseY) +  8;  
		} else {
			*((volatile uint16_t*)APOLLO_POINTER_SET_X) = 0;
			*((volatile uint16_t*)APOLLO_POINTER_SET_Y) = 0;		
		}

		/*sprintf(ApolloDebugMessage,"Mouse_X = X-Pos:%4d X-New:%4d X-Old:%4d X-Delta:%4d | Mouse_Y = Y-Pos:%4d Y-New:%4d Y-Old:%4d Y-Delta:%4d\n",
				MouseState->mouseX, Mouse_X, Mouse_X_Old, Mouse_X_Delta, MouseState->mouseY, Mouse_Y, Mouse_Y_Old, Mouse_Y_Delta);
		DebugPutStr(ApolloDebugMessage);*/
	} 

	// Translate Mouse Buttons to Mouse ButtonState
	MouseState->ButtonState = 0;

	if( MouseState->LeftCount > 0 ) MouseState->LeftCount--;
	if( MouseState->RightCount > 0 ) MouseState->RightCount--;
	if( MouseState->MiddleCount > 0 ) MouseState->MiddleCount--;

	if( MouseState->button1 == false && MouseStateOld.button1 == true )
	{
		if( MouseState->LeftCount == 0 )
		{
			MouseState->ButtonState |= SBUIMOUSE_LEFTCLICK;
			MouseState->LeftCount = APOLLODOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->ButtonState |= SBUIMOUSE_LEFTDOUBLECLICK;
		}
	}

	if( MouseState->button1 == true )
	{
		MouseState->ButtonState |= SBUIMOUSE_LEFTDOWN;
	}
	
	if( MouseState->button2 == false && MouseStateOld.button2 == true )
	{
		if( MouseState->RightCount == 0 )
		{
			MouseState->ButtonState |= SBUIMOUSE_RIGHTCLICK;
			MouseState->RightCount = APOLLODOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->ButtonState |= SBUIMOUSE_RIGHTDOUBLECLICK;
		}
	}

	if( MouseState->button2 == true )
	{
		MouseState->ButtonState |= SBUIMOUSE_RIGHTDOWN;
	}

	if( MouseState->button3 == false && MouseStateOld.button3 == true )
	{
		if( MouseState->MiddleCount == 0 )
		{
			MouseState->ButtonState |= SBUIMOUSE_MIDDLECLICK;
			MouseState->MiddleCount = APOLLODOUBLECLICKCOUNTER;
		}
		else
		{
			MouseState->ButtonState |= SBUIMOUSE_MIDDLEDOUBLECLICK;
		}
	}

	if( MouseState->button3 == true && MouseStateOld.button3 == false )
	{
		MouseState->ButtonState |= SBUIMOUSE_MIDDLEDOWN;
	}

	memcpy( &MouseStateOld, MouseState, sizeof(SBInputMouseState) );

	return;
}


//--------------------------------------------------------------------------------------------
// [WD] ApolloJoypad
//--------------------------------------------------------------------------------------------
void SBInput::GetSystemJoypadState( SBInputJoypadState *JoypadState )
{
	uint16_t * const Joypad_Pointer  = (uint16_t*)0xDFF220;
	uint16_t Joypad_Value;
	
	Joypad_Value = *Joypad_Pointer;

	//if (Joypad_Value > 1) // DebugPutHex("Joypad Value", Joypad_Value-1);

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

//--------------------------------------------------------------------------------------------
// [WD] ApolloKeyboard
//--------------------------------------------------------------------------------------------
void SBInput::GetSystemKeyboardState( SBInputKeyboardState *KeyboardState )
{
	unsigned char * const 	Keyboard_Pointer = (unsigned char *)0xBFEC01; 
	unsigned char			Keyboard_Raw;

	Keyboard_Raw = *Keyboard_Pointer;															// retrieve RAW value from register
	Keyboard_Raw = ~Keyboard_Raw;																// not.b
	KeyboardState->KeyboardCurrentKey = (uint8_t) ((Keyboard_Raw>>1) | (Keyboard_Raw<<7));		// ror.b #1

    if (KeyboardState->KeyboardCurrentKey!=KeyboardState->KeyboardPreviousKey && KeyboardState->KeyboardCurrentKey != 127)      // Has Key been Pressed (>0 && !=127) or Released (<0)?
    {
        if (KeyboardState->KeyboardCurrentKey > 0)                                              // Yes, but is it a Press (>0) or Release (<0)
        {
            KeyboardState->KeyboardPreviousKey = KeyboardState->KeyboardCurrentKey;             // Press, so we can update Previous Key Value with Current Key Value
        } else {
            if ((KeyboardState->KeyboardCurrentKey + 128) == KeyboardState->KeyboardPreviousKey) KeyboardState->KeyboardPreviousKey = 127; // Release, so we can inactivate Previous Key Value (127)
            //KeyboardState->KeyboardCurrentKey = 127;                                            // Release, so we can inactivate Current Key Value (127)
        }
    } else {
        KeyboardState->KeyboardCurrentKey = 127;                                                // No, then we report No Current Key Value (127), but keep Previous Key Value for Repeat Event
    }    
   
    //if (KeyboardState->KeyboardCurrentKey !=127 || KeyboardState->KeyboardPreviousKey !=127)
	//{
    //   sprintf(ApolloDebugMessage,"SBInput::GetSystemKeyboardState: KeyboardState->KeyboardCurrentKey %d | KeyboardPreviousKey %d\n", KeyboardState->KeyboardCurrentKey, KeyboardState->KeyboardPreviousKey );
    //    DebugPutStr(ApolloDebugMessage);
    //}
}

//=========================================================================================
// GB     : Scan2Ascii
//-----------------------------------------------------------------------------------------
// Convert a scan code into ascii
//-----------------------------------------------------------------------------------------
// Input  : ulScanCode    : the scan code to convert
//          lpAsciiBuffer : buffer recieving the ascii values
//-----------------------------------------------------------------------------------------
// Output : Number of characters converted into 'lpAscii'
//-----------------------------------------------------------------------------------------
// Caller : 
//=========================================================================================
/*SLONG SBInput::Scan2Ascii( ULONG ulScanCode, UWORD *lpAsciiBuffer )
{
    // XXX non sono sicuro che sia questo quello che si vuol fare qui

	// Map the scan code to convert into a virtual key
	if( char *name = SDL_GetKeyName((SDLKey)ulScanCode)) {
        strcpy((char *)lpAsciiBuffer, name);
        return strlen(name);
    }
    
	return 0;	
}*/


//=========================================================================================
// GB     : Scan2Unicode
//-----------------------------------------------------------------------------------------
// Convert a scan code into UNICODE
//-----------------------------------------------------------------------------------------
// Input  : ulScanCode          : the scan code to convert
//          lpUnicodeBuffer     : the buffer recieving the converted value
//          ulUnicodeBufferSize : the size of 'lpUnicodeBuffer'
//          ShiftBehaviour shiftBehaviour
//-----------------------------------------------------------------------------------------
// Output : Number of characters contained into lpUnicode
//-----------------------------------------------------------------------------------------
// Caller : 
//=========================================================================================
SLONG SBInput::Scan2Unicode( ULONG ulScanCode, UWORD *lpUnicodeBuffer, ULONG ulUnicodeBufferSize, ShiftBehaviour shiftBehaviour )
{
    if (shiftBehaviour == UPPERCASE)
        lpUnicodeBuffer[0] = toupper(ulScanCode);
    else
        lpUnicodeBuffer[0] = ulScanCode;
    return 1;
#ifdef TODO
	static HKL		keyboardLayout;		// Keyboard layout
	static UCHAR	keyboardState[256];	// Keyboard state
	ULONG			virtualKey;			// Virtual kay value
	char			asciiBuffer[3];		// An ASCII buffer

	// Get the keyboard layout
	keyboardLayout = GetKeyboardLayout(0);
	// Get the keyboard state
	if( GetKeyboardState( keyboardState ) == FALSE )
		return 0;
	// Map the scan code to convert into a virtual key
	if( ( virtualKey = MapVirtualKeyEx( ulScanCode, 1, keyboardLayout ) ) == 0 )
		return 0;

	// Analyse the the caps behaviour to perform
	switch( shiftBehaviour )
	{
	case UPPERCASE:
		// Enfore the caps lock
		keyboardState[VK_CAPITAL] = 1;
		break;

	case LOWERCASE:
		// Enforce the caps to unlocked
		keyboardState[VK_CAPITAL] = 0;
		keyboardState[VK_SHIFT]   = 0;
		break;
	}
	
	// Initialize the ascii buffer
	memset( asciiBuffer, 0, sizeof(asciiBuffer) );
	// Convert the into ASCII
	ToAscii( virtualKey, ulScanCode, keyboardState, (LPWORD) asciiBuffer, 0 );
	ToAscii( virtualKey, ulScanCode, keyboardState, (LPWORD) asciiBuffer, 0 );
	// Ignore all characters abowe the first one in the asciiBuffer
	asciiBuffer[1] = 0;	
	// Convert the ASCII into UNICODE
	return MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, asciiBuffer, -1, lpUnicodeBuffer, ulUnicodeBufferSize );
#endif
}



//=========================================================================================
// SB: SBInput::Scan2Unicode
//-----------------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------------
// Input  : ULONG ulScanCode
//          UWORD *lpUnicodeBuffer
//          UCHAR* pKeys
//          ULONG ulUnicodeBufferSize
//-----------------------------------------------------------------------------------------
// Output : SLONG 
//-----------------------------------------------------------------------------------------
// Caller : 
//=========================================================================================
/*SLONG SBInput::Scan2Unicode( ULONG ulScanCode, UWORD *lpUnicodeBuffer, UCHAR* pKeys, ULONG ulUnicodeBufferSize )
{
    char *value = SDL_GetKeyName((SDLKey)ulScanCode);
    bool uppercase = false;

    if (pKeys[ SDLK_LSHIFT ] || pKeys[SDLK_RSHIFT])
        uppercase = true;

    if (ulUnicodeBufferSize > 0) {
        if (ulUnicodeBufferSize > 1)
            lpUnicodeBuffer[1] = 0;
        if (strlen(value) == 1) {
            if (!uppercase)
                lpUnicodeBuffer[0] = *value;
            else
                lpUnicodeBuffer[0] = toupper(*value);            

            return 1;
        }
        else switch (ulScanCode) {
            case SDLK_SPACE:
                lpUnicodeBuffer[0] = ' ';
                return 1;
            default:
                return 0;
        }
    }

    return 0;
}*/


//=========================================================================================
// GB     : Scan2Virtual
//-----------------------------------------------------------------------------------------
// Convert a scna code into Win32 virtual key
//-----------------------------------------------------------------------------------------
// Input  : ulScanCode : the scan code to convert
//-----------------------------------------------------------------------------------------
// Output : The corresponding virtual key (0 is none)
//-----------------------------------------------------------------------------------------
// Caller : 
//=========================================================================================
ULONG SBInput::Scan2Virtual( ULONG ulScanCode )
{
#ifdef TODO
	static HKL		keyboardLayout;		// Keyboard layout
	static UCHAR	keyboardState[256];	// Keyboard state

	// Get the keyboard layout
	keyboardLayout = GetKeyboardLayout( 0 );
	// Get the keyboard state
	if( GetKeyboardState( keyboardState ) == FALSE )
		return 0;
	// Map the scan code to convert into a virtual key
	return MapVirtualKeyEx( ulScanCode, 1, keyboardLayout );
#endif
    return ulScanCode;
}

