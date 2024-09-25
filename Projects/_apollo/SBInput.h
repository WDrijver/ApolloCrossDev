//============================================================================================
//SB: SBInput.h - DirectInput functions
//============================================================================================
#ifndef __SBINPUT_H
#define __SBINPUT_H

#include "SDL.h"

#define APOLLO_KB_NOKEY	127
#define APOLLO_KB_ESC	69
#define APOLLO_KB_F1	80
#define APOLLO_KB_F2	81
#define APOLLO_KB_F3	82
#define APOLLO_KB_F4	83
#define APOLLO_KB_F5	84
#define APOLLO_KB_F6	85
#define APOLLO_KB_F7	86
#define APOLLO_KB_F8	87
#define APOLLO_KB_F9	88
#define APOLLO_KB_F10	89
#define APOLLO_KB_F11	75
#define APOLLO_KB_F12	111

#define APOLLO_KB_DEL	70

#define APOLLO_KB_LEFT_SINGLE_QUOTE 0
#define APOLLO_KB_1		1
#define APOLLO_KB_2		2
#define APOLLO_KB_3		3
#define APOLLO_KB_4		4
#define APOLLO_KB_5		5
#define APOLLO_KB_6		6
#define APOLLO_KB_7		7
#define APOLLO_KB_8		8
#define APOLLO_KB_9		9
#define APOLLO_KB_0		10
#define APOLLO_KB_MINUS	11
#define APOLLO_KB_EQUAL	12
#define APOLLO_KB_BACKSPACE		65

#define APOLLO_KB_q		16
#define APOLLO_KB_w		17
#define APOLLO_KB_e		18
#define APOLLO_KB_r		19
#define APOLLO_KB_t		20
#define APOLLO_KB_y		21
#define APOLLO_KB_u		22
#define APOLLO_KB_i		23
#define APOLLO_KB_o		24
#define APOLLO_KB_p		25
#define APOLLO_KB_BRACKET_LEFT	26
#define APOLLO_KB_BRACKET_RIGHT	27

#define APOLLO_KB_a		32
#define APOLLO_KB_s		33
#define APOLLO_KB_d		34
#define APOLLO_KB_f		35
#define APOLLO_KB_g		36
#define APOLLO_KB_h		37
#define APOLLO_KB_j		38
#define APOLLO_KB_k		39
#define APOLLO_KB_l		40
#define APOLLO_KB_SEMICOLON		41
#define APOLLO_KB_SINGLE_QUOTE 	42
#define APOLLO_KB_BACKSLASH		43

#define APOLLO_KB_BLANK_KEY	48
#define APOLLO_KB_z		49
#define APOLLO_KB_x		50
#define APOLLO_KB_c		51
#define APOLLO_KB_v		52
#define APOLLO_KB_b		53
#define APOLLO_KB_n		54
#define APOLLO_KB_m		55
#define APOLLO_KB_COMMA	56
#define APOLLO_KB_POINT	57
#define APOLLO_KB_FORWARDSLASH	58

#define APOLLO_KB_SPACE 64
#define APOLLO_KB_CTRL 99
#define APOLLO_KB_CAPSLOCK_DOWN 98
#define APOLLO_KB_CAPSLOCK_UP -30

#define APOLLO_KB_LEFT_ARROW 79
#define APOLLO_KB_RIGHT_ARROW 78
#define APOLLO_KB_UP_ARROW 76
#define APOLLO_KB_DOWN_ARROW 77



#define DIK_RETURN SDLK_RETURN
#define DIK_ESCAPE SDLK_ESCAPE
#define DIK_DELETE SDLK_DELETE
#define DIK_NUMPADENTER SDLK_KP_ENTER
#define DIK_TAB SDLK_TAB
#define DIK_LCONTROL SDLK_LCTRL
#define DIK_RCONTROL SDLK_RCTRL
#define DIK_LSHIFT SDLK_LSHIFT
#define DIK_RSHIFT SDLK_RSHIFT
#define DIK_NUMPADPERIOD SDLK_KP_PERIOD
#define DIK_LALT SDLK_LALT
#define DIK_RALT SDLK_RALT
#define VK_LEFT SDLK_LEFT
#define VK_RIGHT SDLK_RIGHT
#define VK_HOME SDLK_HOME
#define VK_BACK SDLK_BACKSPACE
#define VK_DELETE SDLK_DELETE
#define VK_ESCAPE SDLK_ESCAPE
#define VK_RETURN SDLK_RETURN
#define VK_END SDLK_END
#define VK_TAB SDLK_TAB
#define VK_DOWN SDLK_DOWN
#define VK_UP SDLK_UP

// Max number of joysticks that we want to manage
#define MAX_JOYSTICKS	16

// Defining what you can get as devices
#define	SBINPUT_SYSTEMKEYBOARD_BUFFERED		0x01
#define SBINPUT_SYSTEMKEYBOARD_UNBUFFERED	0x02
#define SBINPUT_SYSTEMMOUSE_BUFFERED		0x04
#define SBINPUT_SYSTEMMOUSE_UNBUFFERED		0x08
#define SBINPUT_JOYSTICK_BUFFERED			0x10
#define SBINPUT_JOYSTICK_UNBUFFERED			0x20
#define	SBINPUT_EXCLUSIVE_MOUSE				0x40

#define SBINPUT_KILLBUFFER					0x00000001

// Kind of errors we can encounter
#define SBINPUT_NO_ERROR							0x0000
#define SBINPUT_CANT_INITIALIZE_DINPUT				0x0001
#define SBINPUT_CANT_FOCUS							0x0002
#define SBINPUT_CANT_UNFOCUS						0x0003
#define SBINPUT_CANNOT_CREATE_CONCURRENT_DEVICES	0x0004
#define SBINPUT_CANT_ALLOCATE_SYSTEMKEYBOARD		0x0005
#define SBINPUT_CANT_ALLOCATE_SYSTEMMOUSE			0x0006
#define SBINPUT_SETFORMAT_SYSTEMKEYBOARD			0x0007
#define SBINPUT_SETCOOPERATIVE_SYSTEMKEYBOARD		0x0008
#define SBINPUT_SETFORMAT_SYSTEMMOUSE				0x0009
#define SBINPUT_SETCOOPERATIVE_SYSTEMMOUSE			0x000a
#define SBINPUT_CANT_RETRIEVE_DATA					0x000b
#define SBINPUT_CANT_CREATE_DEVICE					0x000c
#define SBINPUT_CANT_SET_DATA_FORMAT				0x000d
#define SBINPUT_COOPERATIVE_JOYSTICK				0x000e
#define SBINPUT_CANT_FIND_DEVICE					0x000f
#define SBINPUT_CANT_ACQUIRE_JOYSTICK				0x0010
#define SBINPUT_JOYSTICK_INPUT_LOST					0x0011
#define SBINPUT_CANT_SET_AXIS_RANGE					0x0012
#define SBINPUT_CANT_SET_AXIS_DEADZONE				0x0013
#define SBINPUT_SETPROPERTY_SYSTEMMOUSE				0x0014
#define SBINPUT_SETPROPERTY_SYSTEMKEYBOARD			0x0015
#define SBINPUT_NO_HWND								0x0016
#define SBINPUT_NO_DATA								0x0017

#define SBINPUT_CANT_ALLOC_JOYSTICK					0x00ff

// Define structures to handle device state
typedef struct
{
	SWORD	mouseX;
	SWORD	mouseY;
	SWORD	mouseZ;
	bool	button1;					// Actual state for Button 1	
	bool	button2;					// Actual state for Button 2
	bool	button3;					// Actual state for Button 3
	UWORD	ButtonState		= 0;		// Interpreted Button Action (see table below)
	UWORD	LeftCount		= 0;				
	UWORD	LeftDefault		= 15;
	UWORD	RightCount		= 0;
	UWORD	RightDefault	= 15;
	UWORD	MiddleCount		= 0;
	UWORD	MiddleDefault	= 15;
} SBInputMouseState;

/* Mouse Button Actions 
SBUIMOUSE_LEFTCLICK			0x0001
SBUIMOUSE_RIGHTCLICK		0x0002
SBUIMOUSE_MIDDLECLICK		0x0004
SBUIMOUSE_LEFTDOUBLECLICK	0x0008
SBUIMOUSE_RIGHTDOUBLECLICK	0x0010
SBUIMOUSE_MIDDLEDOUBLECLICK	0x0020
SBUIMOUSE_LEFTDOWN			0x0040
SBUIMOUSE_RIGHTDOWN			0x0080
SBUIMOUSE_MIDDLEDOWN		0x0100
*/

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
} SBInputJoypadState;

typedef struct
{
	//UCHAR	keys[ SDLK_LAST ];			// [WD] LEGACY
	int8_t KeyboardCurrentKey;
	int8_t KeyboardPreviousKey;
} SBInputKeyboardState;


// Warning : this SBInputJoystickState must match the DIJOYSTATE structure
typedef struct {
    SLONG    X;					// X-axis information
	SLONG    Y;					// Y-axis information
	SLONG    Z;					// Z-axis information
	SLONG    Rx;					// X-axis rotation information
    SLONG    Ry;					// Y-axis rotation information
	SLONG    Rz;					// Z-axis rotation information
	SLONG    Slider[2];			// Additional axis information
    ULONG   POV[4];				// Position of direction controllers 
    UBYTE    Buttons[32];		// Buttons state
} SBInputJoystickState;


typedef enum {

	DONT_CARE,
	UPPERCASE,
	LOWERCASE,

} ShiftBehaviour;


// The class to handle all direct input things
class SBInput
{

private:
	// Pointer to the instance of this class

	static SBInput*	mpInput;

	// Window handler of the application
	
//	HWND hWndInput;

	// Some direct input data

//	LPDIRECTINPUT7			lpDirectInput;
//	LPDIRECTINPUTDEVICE7	systemKeyboard;
//	LPDIRECTINPUTDEVICE7	systemMouse;

	SBList<SBString>		deviceList;					// A list to handle enumerated devices

	// Some buffers for the buffered input

//	DIDEVICEOBJECTDATA		mouseBuffer;
//	DIDEVICEOBJECTDATA		keyboardBuffer;
//	DIDEVICEOBJECTDATA		joystickBuffer[MAX_JOYSTICKS];

	// Some booleans state values

	bool		useSysKeyboard;			// Does we will use the system keyboard
	bool		bufferedSysKeyboard;	// Does the system keyboard will be buffered
	bool		useSysMouse;			// Does we will use the system mouse
	bool		bufferedSysMouse;		// Does the system mouse will be buffered
	bool		hasFocus;				// Are we in an acquired mode

	// Error related data

	UCHAR		error;
	SBString	errorCause;

	// Reserved function called in the enumeration callback

	void	AddJoystick								( int );	// Adds a joystick in the current SBInput object
	friend	class SBThreadedInput;

	bool	GetMouseData					( SDL_Event &objectData );
	bool	GetKeyboardData					( SDL_Event &bjectData );

public:

	// The standard C++ kitchen

	SBInput();								// Standard constructor that will only initialized direct input
	SBInput(UWORD devices);	// Construct a SBInput object and request some systems devices
	~SBInput();												// Release the whole thing

	// Retrieve the singleton instance of this object

	static SBInput*	GetInputManager( void )	{ return mpInput; };

	// Big Picture management function

	bool	ApplicationIsActive		( bool isActive );		// Tell the SBInput if we have the focus
	// System device management function

	bool	AllocateSystemDevices	( UWORD devices );							// Allocate some systems devices to work with
	bool	DeAllocateSystemDevices	( UWORD devices );										// Desallocate some systems devices

	void	GetSystemMouseState		( SBInputMouseState *MouseState);			// [WD] ApolloMouse
	void	GetSystemJoypadState	( SBInputJoypadState *JoypadState);			// [WD] ApolloJoypad
	void	GetSystemKeyboardState	( SBInputKeyboardState *KeyboardState);		// [WD] ApolloKeyboard

	/* Some functions to deal with joystick (more to come)

	SBList<SBString>*	GetAllJoysticks				( void );															// Get a list of all devices that are joysticks
	UCHAR				AllocateJoystick			( SBString joyName, UWORD flags );					// Allocate a joystick device to work with
//	bool				SetJoystickAxisProperties	( UCHAR joyNumber, ULONG Object, SLONG Min, SLONG Max, ULONG Value );
	bool				DeAllocateJoystick			( UCHAR joyNumber );												// DeAllocate a joystick device
	bool				GetJoystickState			( UCHAR joyNumber, SBInputJoystickState *state, bool *pStillData=NULL ); */

	// Tool function to convert scan code to ascii, ...
	static SLONG	Scan2Ascii		( ULONG ulScanCode, UWORD *lpAsciiBuf );												// Convert a scan code into ascii
	static SLONG	Scan2Unicode	( ULONG ulScanCode, UWORD *lpUBuf, ULONG ulUBufSize=2, ShiftBehaviour = DONT_CARE );	// Convert a scan code into unicode
	static SLONG	Scan2Unicode	( ULONG ulScanCode, UWORD *lpUBuf, UCHAR* pKeyboard, ULONG ulUBufSize=2 );				// Convert a scan code into unicode
	static ULONG	Scan2Virtual	( ULONG ulScanCode );																	// Convert a scna code into virtual key

};

#endif

