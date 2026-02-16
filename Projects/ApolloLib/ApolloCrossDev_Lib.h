#ifdef __cplusplus
extern "C"{
#endif 

// ApolloCrossDev Library
// 6-2-2026

// Include Apollo Headers
#include "ApolloCrossDev_Defines.h"
#include "ApolloCrossDev_Debug.h"

// Apollo Structures ######################################

struct ApolloFile
{
    // Input Values
    char        *filename;
    uint8_t     *buffer;
    uint32_t    size;
    uint32_t    offset;
};
struct ApolloSound
{
    // Input Values
    char        filename[256];
    uint8_t     format;
    // Output Values
    uint8_t     *buffer;
    uint32_t    position;
    uint32_t    size;
    uint16_t    period;
    uint8_t     channel;
    bool        stereo;
    uint32_t    datarate;
    uint16_t    bitspersample;
    // Playback Values
    bool        loop;
    bool        fadein;
    bool        fadeout;
    uint16_t    volume_left;
    uint16_t    volume_right;
};
struct ApolloPicture
{
    // Input Values
    char        filename[256];
    uint8_t     format;
    bool        endian;
    // Return Values
    uint8_t     *buffer;
    uint32_t    position;
    uint32_t    size;
    uint16_t    width;
    uint16_t    height;
    uint8_t     depth;
    uint32_t    palette;
    // Display Values
    int16_t     modulo;
    bool        fullscreen;
};
struct BMPHeader
{
    uint16_t	type;
    uint32_t	size;
    uint16_t	reserved1;
    uint16_t	reserved2;
    uint32_t	offset;

    uint32_t	headersize;
    int32_t		width;
    int32_t		height;
    uint16_t	planes;
    uint16_t	bpp;
    uint32_t	compression;
    uint32_t	sizeimage;
    int32_t		hppm;
    int32_t		vppm;
    uint32_t	palette;
    uint32_t	colorimportant;
};
struct DDSHeader
{
    uint32_t    dwMagic;
    uint32_t    dwSize;
    uint32_t    dwFlags;
    uint32_t    dwHeight;
    uint32_t    dwWidth;
    uint32_t    dwPitchOrLinearSize;
    uint32_t    dwDepth;
    uint32_t    dwMipMapCount;
    uint32_t    dwReserved1[11];

    // DDPIXELFORMAT
    uint32_t    ddpfSize;
    uint32_t    ddpfFlags;
    uint32_t    ddpfFourCC;
    uint32_t    ddpfRGBBitCount;
    uint32_t    ddpfRBitMask;
    uint32_t    ddpfGBitMask;
    uint32_t    ddpfBBitMask;
    uint32_t    ddpfABitMask;

    // DDCAPS2
    uint32_t    ddsCaps1;
    uint32_t    ddsCaps2;
    uint32_t    ddsCapsReserved[2];
    uint32_t    dwReserved2;
};
struct AIFFHeader
{
    uint32_t    file_id;
    uint32_t    chunk_size;
};

struct WAVHeader
{
    uint32_t    file_id;
    uint32_t    file_size;
    uint32_t    wave_id;
    uint32_t    fmt_id;
    uint32_t    fmt_size;
    uint16_t    audio_format;
    uint16_t    num_channels;
    uint32_t    sample_rate;
    uint32_t    byte_rate;
    uint16_t    block_align;
    uint16_t    bits_per_sample;
    uint32_t    data_id;
    uint32_t    data_size;
};

typedef struct
{
	uint8_t Current_Key;
	uint8_t Previous_Key;
} ApolloKeyBoardState;


typedef struct
{
	UWORD Joypad_Value;
    BYTE Joypad_LeftX_Delta;
	BYTE Joypad_LeftY_Delta;
    BYTE Joypad_RightX_Delta;
	BYTE Joypad_RightY_Delta;
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
	UBYTE	Button_Middle;
	
	bool	Button_Left_Old;		// Previous state for Buttons	
	bool	Button_Right_Old;
	bool	Button_Middle_Old;
	
	uint16_t	Button_State;			// Interpreted Button Action (see table below)
	uint16_t	Button_Left_Count;
	uint16_t	Button_Right_Count;
	uint16_t	Button_Middle_Count;
} ApolloMouseState;



// ApolloCrossDev C Functions ######################################

// Apollo File Functions
uint8_t ApolloLoadFile(struct ApolloFile *file);

// Apollo Memory Functions

// Apollo Sound Functions 
uint8_t ApolloAllocSound( struct ApolloSound *sound);
void ApolloFreeSound( struct ApolloSound *sound);
uint8_t ApolloLoadSound(struct ApolloSound *sound);
uint8_t ApolloPlaySound(struct ApolloSound *sound);
void ApolloStopSound(struct ApolloSound *sound);
void ApolloStartSound(struct ApolloSound *sound);
void ApolloVolumeSound(struct ApolloSound *sound);
void ApolloFadeInSound(struct ApolloSound *sound);
void ApolloFadeOutSound(struct ApolloSound *sound);

// Apollo Picture Functions 
uint8_t ApolloAllocPicture( struct ApolloPicture *picture);
void ApolloFreePicture( struct ApolloPicture *picture);
uint8_t ApolloLoadPicture(struct ApolloPicture *picture);
uint8_t ApolloShowPicture(struct ApolloPicture *picture);
void ApolloShowPiP( struct ApolloPicture *picture);
void ApolloHidePiP();
void ApolloShowPattern(uint8_t *buffer, uint16_t width, uint16_t height, uint8_t depth);
void ApolloBackupWBScreen(struct ApolloPicture *picture);

// Apollo CPU Functions
void ApolloWaitVBL();

// Apollo HID Functions
void ApolloMouse(ApolloMouseState *MouseState);
void ApolloJoypad(ApolloJoypadState *JoypadState);
void ApolloKeyboard(ApolloKeyBoardState *KeyboardState);
uint8_t ApolloKeyboardToUnicode(uint8_t KeyboardAmiga);

// ApolloCrossDev Assembler Functions ######################################

// Apollo Memory Functions
void ApolloCopyBlock( _A0(uint8_t *s), _A1(uint8_t *d), _D3(ULONG size));
void ApolloCopyBlock32(_A0(uint8_t *s), _A1(uint8_t *d), _D3(ULONG size)); 
void ApolloEndianSwapWordBuffer(_A0(uint8_t *s), _D0(uint32_t l));
void ApolloEndianSwapLongBuffer(_A0(uint8_t *s), _D0(uint32_t l));
uint16_t ApolloSwapWord( _D0(uint16_t SwapWord));
uint32_t ApolloSwapLong( _D0(uint32_t SwapLong));
uint64_t ApolloSwapOcta( _D0(uint64_t SwapOcta));

// Apollo Picture Functions
void ApolloFill( _A0(uint8_t* dst), _D3(uint16_t w), _D4(uint16_t h), _D5(uint16_t d), _D6(uint32_t dstmod), _D7(uint32_t value) );
void ApolloCopyPicture( _A0(uint8_t *s), _A1(uint8_t *d), _D3(uint16_t width), _D4(uint16_t height), _D5(uint16_t spitch), _D6(uint16_t dpitch) );
void ApolloCopyPicture32(_A0(uint8_t *s), _A1(uint8_t *d), _D3(uint16_t width), _D4(uint16_t height), _D5(uint16_t spitch), _D6(uint16_t dpitch) ); 

// Apollo CPU Functions
uint32_t ApolloCPUTick();
void ApolloCPUDelay( _D0(uint32_t WaitTime));


#ifdef __cplusplus
}
#endif
