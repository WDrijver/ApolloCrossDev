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
    char        filename[256];      // filename of the sound
    uint8_t     format;             // format of the sound (APOLLO_AIFF_FORMAT or APOLLO_WAV_FORMAT)   
    // Output Values
    uint8_t     *buffer;            // Buffer created by ApolloSound
    uint32_t    position;           // 32-Byte alignment position for start of the Sound data
    uint32_t    size;               // Size of the sound data in bytes
    uint16_t    period;             // Playback period for the sound (calculated from sample rate)
    uint8_t     channel;            // Channel assigned by ApolloPlaySound
    bool        stereo;             // true if sound is stereo, false if mono
    uint32_t    datarate;           // Data rate of the sound
    uint16_t    bitspersample;      // Bits per sample of the sound
    // Playback Values
    bool        loop;               // true if the sound should loop
    bool        fadein;             // true if the sound should fade in
    bool        fadeout;            // true if the sound should fade out
    uint16_t    volume_left;        // Left channel volume
    uint16_t    volume_right;       // Right channel volume
    uint16_t    pan;                // Pan value
    bool        staticchannel;      // if true the sound will play on ApolloSound->channel
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
    int32_t     width;
    int32_t     height;
    uint8_t     depth;
    uint32_t    palettesize;
    uint32_t    palette[256];
    
    // Display Values
    int16_t     modulo;
    bool        fullscreen;
    uint16_t    target;                    // 0 = SAGA Screen | 1 = SAGA PiP1 | 2 = SAGA PiP2 | 3 = SAGA Sprite
};
struct BMPFileHeader
{
    uint16_t	type;
    uint32_t	size;
    uint16_t	reserved1;
    uint16_t	reserved2;
    uint32_t	offset;
};
struct BMPDIBHeader_BITMAPCOREHEADER
{
    uint32_t	headersize;
    uint16_t	width;
    uint16_t	height;
    uint16_t	planes;
    uint16_t	bpp;
};  
struct BMPDIBHeader_BITMAPINFOHEADER
{   
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
struct BMPDIBHeader_BITMAPV4HEADER
{   
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
    uint32_t	redmask;
    uint32_t	greenmask;
    uint32_t	bluemask;
    uint32_t	alphamask;
    uint32_t	csType;
    uint32_t	endpointR[3];
    uint32_t	endpointG[3];
    uint32_t	endpointB[3];
    uint32_t	gammaR;
    uint32_t	gammaG;
    uint32_t	gammaB; 
};
struct BMPDIBHeader_BITMAPV5HEADER
{   
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
    uint32_t	redmask;
    uint32_t	greenmask;
    uint32_t	bluemask;
    uint32_t	alphamask;
    uint32_t	csType;
    uint32_t	endpointR[3];
    uint32_t	endpointG[3];
    uint32_t	endpointB[3];
    uint32_t	gammaR;
    uint32_t	gammaG;
    uint32_t	gammaB; 
    uint32_t	intent; 
    uint32_t	profiledata; 
    uint32_t	profilesize; 
    uint32_t	reserved; 
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

struct ApolloPointer
{
    bool    bLoaded;                 // Indicates if the pointer is loaded and ready to use
    UBYTE   data[64*32*2];           // Pointer Sprite pixel data (64x32 pixels, 2 bytes per pixel for color index (BYTE-1) and alpha value (BYTE-2))
    ULONG   colors[256];             // Pointer Sprite color palette (256 color indices, 24 bits per color in R8G8B8 format)
    FLOAT   hotspotX;                // X coordinate of the pointer's hotspot (e.g., 0 for top-left corner, 16 for center of a 32x32 pointer)
    FLOAT   hotspotY;                // Y coordinate of the pointer's hotspot (e.g., 0 for top-left corner, 16 for center of a 32x32 pointer) 
};

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
//void ApolloBackupWBScreen(struct ApolloPicture *picture);

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
void ApolloCopyLongs(_A0(uint8_t *s), _A1(uint8_t *d), _D3(ULONG size));
void ApolloFillLongs(_A1(uint8_t *d), _D4(ULONG value), _D3(ULONG size));

void ApolloEndianSwapWordBuffer(_A0(uint8_t *s), _D0(uint32_t l));
void ApolloEndianSwapLongBuffer(_A0(uint8_t *s), _D0(uint32_t l));
uint16_t ApolloSwapWord( _D0(uint16_t SwapWord));
uint32_t ApolloSwapLong( _D0(uint32_t SwapLong));
uint64_t ApolloSwapOcta( _D0(uint64_t SwapOcta));

// Apollo Picture Functions
void ApolloFillBitMap( _A0(uint8_t* dst), _D3(uint16_t w), _D4(uint16_t h), _D5(uint16_t d), _D6(uint32_t dstmod), _D7(uint32_t value) );
void ApolloFillBlock( _A0(uint8_t* dst), _D3(uint32_t c), _D4(uint32_t value));
void ApolloFillColor( _A0(uint8_t* dst), _D3(uint16_t w), _D4(uint16_t h), _D5(uint16_t d), _D6(uint32_t dstmod), _D7(uint32_t value) );

void ApolloCopyPicture( _A0(uint8_t *s), _A1(uint8_t *d), _D3(uint16_t width), _D4(uint16_t height), _D5(uint16_t spitch), _D6(uint16_t dpitch) );
void ApolloCopyPicture32(_A0(uint8_t *s), _A1(uint8_t *d), _D3(uint16_t width), _D4(uint16_t height), _D5(uint16_t spitch), _D6(uint16_t dpitch) ); 

// Apollo CPU Functions
uint32_t ApolloCPUTick();
void ApolloCPUDelay( _D0(uint32_t WaitTime));

// Apollo RHLOS Functions
extern UBYTE ApolloKeyboardToUnicode(UBYTE KeyboardAmiga);
void ApolloStartChannel(int channel);
void ApolloStopChannel(int channel);
void ApolloPlayFile(const char *filename, uint8_t *buffer, uint16_t offset, int channel, int volume_left, int volume_right, bool loop);

extern void ApolloBlitLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch), _D6(ULONG dpitch));
extern void ApolloCopyLoop( _A0(UWORD *s), _A1(UWORD *d), _D3(UWORD width), _D4(UWORD height), _D5(UWORD spitch), _D6(UWORD dpitch) );
extern void ApolloCopy32Loop( _A0(UWORD *s), _A1(UWORD *d), _D3(UWORD width), _D4(UWORD height), _D5(UWORD spitch), _D6(UWORD dpitch) );
extern void ApolloEndianSwap2Loop(_A0(UWORD *s), _D0(ULONG l));
extern void ApolloEndianSwap8Loop(_A0(UWORD *s), _D0(ULONG l));

extern void ApolloUncompressLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch) );
extern void ApolloUncompressVectorLoop( _A0(UWORD* s), _A1(UWORD* i), _A2(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch));
void ApolloFadeOut(int channel, int volume_start, int volume_end);
void ApolloShowFile(const char *filename, uint8_t **buffer_draw, uint8_t **buffer_live, uint32_t lenght, uint16_t offset, uint16_t gfx_mode, uint16_t gfx_modulo, bool endianswap);
void ApolloCacheFile(const char *filename, uint8_t **cache, uint32_t *lenght, uint16_t file_offset);
bool ApolloPlay(int channel, int volume_left, int volume_right, bool loop, uint8_t *buffer, uint32_t lenght, uint16_t offset);
void ApolloVolume(int channel, int volume_left, int volume_right);
extern void ApolloTakeOver( void );
extern void ApolloForbid( void );
extern void ApolloDisable( void );
extern void ApolloTimer( void );
extern void ApolloFillLoop( _A0(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG dpitch), _D7(UWORD fc) );
extern void ApolloBlitBlueAlphaUILoop( _A0(UWORD* src), _A1(UWORD* dst), _A2(UWORD* alpha), _D3(UWORD width), _D4(UWORD height), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(ULONG apitch));
extern void ApolloBlitRedAlphaBGLoop( _A0(UWORD* src), _A1(UWORD* dst), _A2(UWORD* alpha), _D3(UWORD width), _D4(UWORD height), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(ULONG apitch));
extern void ApolloBlitGreenAlphaBGLoop( _A0(UWORD* src), _A1(UWORD* dst), _A2(UWORD* alpha), _D3(UWORD width), _D4(UWORD height), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(ULONG apitch));
extern void ApolloBlitBlueAlphaBGLoop( _A0(UWORD* src), _A1(UWORD* dst), _A2(UWORD* alpha), _D3(UWORD width), _D4(UWORD height), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(ULONG apitch));
extern void ApolloBlitAlphaConstLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(UWORD al));
extern void ApolloBlitAlphaKeyTransLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D4(UWORD h), _D5(ULONG spitch), _D6(ULONG dpitch), _D7(UWORD al), _A2(UWORD ac));
extern void ApolloBlitAlphaHorizontalLineLoop( _A0(UWORD* s), _A1(UWORD* d), _D3(UWORD w), _D6(ULONG ac), _D7(UWORD al));

extern void ApolloLoadPointer(struct ApolloPointer *sprite_pointer, struct ApolloPicture *sprite_bitmap);
extern void ApolloShowPointer(struct ApolloPointer *sprite_pointer);

extern ULONG ApolloGetMouseDelta(_A0(UWORD *Mouse_Old));

#ifdef __cplusplus
}
#endif
