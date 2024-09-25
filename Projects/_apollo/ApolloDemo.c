// Apollo V4 Demo for Hardware Display, Scrolling, Mouse and PiP
// Willem Drijver

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "Apollo.h"  

#include "clib/exec_protos.h"

#define BG_X_SIZE           848
#define BG_Y_SIZE           ((CACHEBLOCK)/(SAGA_BYTES)) * (SAGA_Y_SIZE)
#define BG_OFFSET           12
#define BG_Y_MIN            1
#define BG_Y_MAX            BG_Y_MIN + BG_Y_SIZE - SAGA_Y_SIZE
#define BG_X_MIN            1
#define BG_X_MAX            BG_X_MIN + BG_X_SIZE - SAGA_X_SIZE
#define BG_MODULO         	(BG_X_SIZE*2)-(SAGA_X_SIZE*2)

#define UI_X_SIZE           848 
#define UI_Y_SIZE           160
#define UI_OFFSET           128
#define UI_XSTART           16
#define UI_XSTOP            UI_X_SIZE + 16
#define UI_YSTART           0
#define UI_YSTOP            UI_Y_SIZE
#define UI_MODULO           0                                // (((SAGA_X_SIZE) - (HUD_X_SIZE) - 32) * 2)

#define	SAGA_SCROLL			16
#define SCROLL_UP_ZONE      5
#define SCROLL_DOWN_ZONE    BG_Y_SIZE - 5
#define SCROLL_LEFT_ZONE    5
#define SCROLL_RIGHT_ZONE   BG_X_SIZE - 5

#define CACHEBLOCK          200 * 1024 * 1024



void ApolloDemo()
{
    // Sound Variables
    char		*MusicBuffer1, *MusicBuffer2, *MusicBuffer3, *MusicBuffer4 ;
	uint32_t	MusicLength1, MusicLength2, MusicLength3, MusicLength4;
   	
    ApolloLoad("Data/Sounds/snd_014.aiff", &MusicBuffer1, &MusicLength1, 0);
   	ApolloLoad("Data/Sounds/snd_030.aiff", &MusicBuffer2, &MusicLength2, 0);
    ApolloLoad("Data/Sounds/snd_032.aiff", &MusicBuffer3, &MusicLength3, 0);
    ApolloLoad("Data/Sounds/snd_035.aiff", &MusicBuffer4, &MusicLength4, 0); 
    
    // Cache Variables
    uint8_t        *BG_Buffer_U	= (unsigned short*)malloc((CACHEBLOCK)+15);
    uint8_t        *BG_Buffer = (unsigned short*)((uint32_t)(BG_Buffer_U +15) & 0xFFFFFFF0);
    
       for (int i=0 ; i < ((CACHEBLOCK)/(SAGA_BYTES)) ; i++)
    {
        ApolloFillLoop((unsigned short*)BG_Buffer + (i * (SAGA_PIXELS)), SAGA_X_SIZE, SAGA_Y_SIZE, 0, 0xFFFF - (i*0x1111));  
        DebugPutDec("Filling Background", i);
    }

    ApolloShow(SAGA_MODE, BG_MODULO, BG_Buffer);

    // Background Variables
    uint32_t        BG_Lenght = 0; 
    uint32_t        BG_Y_Position = 1, BG_X_Position = 1;
    int8_t          BG_X_Delta  = 0, BG_Y_Delta = 0;
 
    // ApolloKeyboard Variables
    unsigned char   Keyboard_Key = 0;
    
    // ApolloMouse Variables
    ApolloMouseState    MouseState = {0};

    // ApolloJoypad Variables
    ApolloJoypadState   JoypadState = {0};


    // Main Demo Loop
    while (MouseState.Mouse_Button1 == false)
    {
        ApolloWaitVBL();

        ApolloKeyboard(&Keyboard_Key);
                
        ApolloMouse(&MouseState);
        
        ApolloJoypad(&JoypadState);
        
	    if (JoypadState.Joypad_A) ApolloPlay(0, 0x7F, 0x7F, false, MusicBuffer1, MusicLength1, AIFF_OFFSET);
	    if (JoypadState.Joypad_B) ApolloPlay(0, 0x7F, 0x7F, false, MusicBuffer2, MusicLength2, AIFF_OFFSET);
        if (JoypadState.Joypad_X) ApolloPlay(0, 0x7F, 0x7F, false, MusicBuffer3, MusicLength3, AIFF_OFFSET);
        if (JoypadState.Joypad_Y) ApolloPlay(0, 0x7F, 0x7F, false, MusicBuffer4, MusicLength4, AIFF_OFFSET);

        BG_Y_Delta = JoypadState.Joypad_Y_Delta; 
        BG_X_Delta = JoypadState.Joypad_X_Delta; 

        if (JoypadState.Joypad_Y_Delta !=0) BG_Y_Delta = JoypadState.Joypad_Y_Delta; else BG_Y_Delta = MouseState.Mouse_Y_Delta;
        if (JoypadState.Joypad_X_Delta !=0) BG_X_Delta = JoypadState.Joypad_X_Delta; else BG_X_Delta = MouseState.Mouse_X_Delta;
        
        if (BG_Y_Delta != 0 && BG_Y_Position + (BG_Y_Delta * SAGA_SCROLL) >= BG_Y_MIN && BG_Y_Position + (BG_Y_Delta * SAGA_SCROLL) <= (BG_Y_MAX))
        {
            BG_Buffer += (BG_Y_Delta * BG_X_SIZE * 2) * SAGA_SCROLL;
            BG_Y_Position += (BG_Y_Delta * SAGA_SCROLL);
            *((volatile uint32_t*)0xDFF1EC) = (uint32_t)(BG_Buffer);
        } 
        if (BG_X_Delta != 0 && BG_X_Position + (BG_X_Delta * SAGA_SCROLL) >= BG_X_MIN && BG_X_Position + (BG_X_Delta * SAGA_SCROLL) <= (BG_X_MAX))
        {
            BG_Buffer += (BG_X_Delta * 2 * SAGA_SCROLL);
            BG_X_Position += (BG_X_Delta * SAGA_SCROLL);
            *((volatile uint32_t*)0xDFF1EC) = (uint32_t)(BG_Buffer);
        }

        if (BG_Y_Delta > 0 || BG_X_Delta > 0)
        {
          sprintf(ApolloDebugText,"BG_X_Position: %6d | BG_Y_Position: %6d | SAGA_SCREEN: %3d\n", BG_X_Position, BG_Y_Position, 1 + (BG_Y_Position/SAGA_Y_SIZE));
		  DebugPutStr(ApolloDebugText);
        }
    }
}
