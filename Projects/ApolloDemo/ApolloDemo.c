// Apollo V4 Demo for Hardware Display, Scrolling, Mouse and PiP
// Willem Drijver

#include "stdint.h"
#include "stdlib.h"
#include "stdbool.h"
#include "../_apollo/Apollo.h"  

#include "clib/exec_protos.h"

#define BG_X_SIZE           3136
#define BG_Y_SIZE           1984
#define BG_SIZE             (BG_X_SIZE * BG_Y_SIZE * 2)
#define BG_Y_MIN            1
#define BG_Y_MAX            BG_Y_MIN + BG_Y_SIZE - SAGA_Y_SIZE
#define BG_X_MIN            1
#define BG_X_MAX            BG_X_MIN + BG_X_SIZE - SAGA_X_SIZE
#define BG_MODULO         	(BG_X_SIZE*2)-(SAGA_X_SIZE*2)
#define BG_OFFSET           0

#define SAGA_X_SIZE         848
#define SAGA_Y_SIZE         480
#define SAGA_BPP            2
#define SAGA_SIZE           (SAGA_X_SIZE * SAGA_Y_SIZE * SAGA_BPP)
#define SAGA_MODE           0x0F02  // SAGA 848x480x16-bits
#define SAGA_MODULO         0
#define SAGA_OFFSET         0

#define UI_X_SIZE           848 
#define UI_Y_SIZE           480
#define UI_OFFSET           0
#define UI_XSTART           16
#define UI_XSTOP            UI_X_SIZE + 16
#define UI_YSTART           0
#define UI_YSTOP            UI_Y_SIZE
#define UI_MODULO           0 

#define	SAGA_SCROLL			8


int main(void)
{
    // Close Workbench
    // ApolloTakeOver();
    ApolloDebugInit();         

    #ifdef APOLLODEBUG
    ApolloDebugPutStr("\n\n\nWelcome to ApolloDemo Serial Debug Output\n");
    #endif

    // Video Buffers
    UBYTE   *Screen_Video_Buffer, *BackGround_Video_Buffer, *UserInterface_Video_Buffer;
    ULONG   Screen_Video_Lenght, BackGround_Video_Lenght, UserInterFace_Video_Lenght;
    
    // Load and Show Start Screen
    ApolloLoad("Data/Start.16.dds", &Screen_Video_Buffer, &Screen_Video_Lenght, DDS_OFFSET, true);
    ApolloShow(Screen_Video_Buffer, Screen_Video_Lenght, SAGA_MODE, SAGA_MODULO);
    
    // Audio Buffers
    UBYTE   *BackGround_Audio_Buffer, *SoundEffect_Buffer1, *SoundEffect_Buffer2, *SoundEffect_Buffer3, *SoundEffect_Buffer4;
    ULONG   BackGround_Audio_Lenght, SoundEffect_Length1, SoundEffect_Length2, SoundEffect_Length3, SoundEffect_Length4;

    // Load and Play Background Music in a Loop
    ApolloLoad("Data/Intro.aiff", &BackGround_Audio_Buffer, &BackGround_Audio_Lenght, AIFF_OFFSET, false);
    ApolloPlay(BackGround_Audio_Buffer, BackGround_Audio_Lenght, 0, 0x7f, 0x7f, true);

    // Load Music Clips into Buffers
    ApolloLoad("Data/SoundEffect1.aiff", &SoundEffect_Buffer1, &SoundEffect_Length1, AIFF_OFFSET, false);
   	ApolloLoad("Data/SoundEffect2.aiff", &SoundEffect_Buffer2, &SoundEffect_Length2, AIFF_OFFSET, false);
    ApolloLoad("Data/SoundEffect3.aiff", &SoundEffect_Buffer3, &SoundEffect_Length3, AIFF_OFFSET, false);
   	ApolloLoad("Data/SoundEffect4.aiff", &SoundEffect_Buffer4, &SoundEffect_Length4, AIFF_OFFSET, false);

    // Background Variables
    ULONG   BG_Lenght = 0; 
    ULONG   BG_X_Position = 1, BG_Y_Position = 1; 
    BYTE    BG_X_Delta  = 0, BG_Y_Delta = 0;
 
    // ApolloKeyboard Variables
    ApolloKeyBoardState KeyboardState = {0};
    ApolloMouseState    MouseState = {0}; MouseState.MouseX_Pointer_Max = SAGA_X_SIZE; MouseState.MouseY_Pointer_Max = SAGA_Y_SIZE;
    ApolloJoypadState   JoypadState = {0};

    ApolloCPUDelay(5000);
    ApolloFadeOut(0, 0x7f, 0x00);

    // Load and Show Background Map
    ApolloLoad("Data/Leicester.map.raw", &BackGround_Video_Buffer, &BackGround_Video_Lenght, RAW_OFFSET, false);
    ApolloShow(BackGround_Video_Buffer, BackGround_Video_Lenght, SAGA_MODE, BG_MODULO);

    // Main Demo Loop
    while (MouseState.Button_Left == false)
    {
        ApolloWaitVBL();

        ApolloKeyboard(&KeyboardState);
        ApolloMouse(&MouseState);
        ApolloJoypad(&JoypadState);
        
	    if (JoypadState.Joypad_A) ApolloPlay(SoundEffect_Buffer1, SoundEffect_Length1, 1, 0x7F, 0x7F, false);
	    if (JoypadState.Joypad_B) ApolloPlay(SoundEffect_Buffer2, SoundEffect_Length2, 2, 0x7F, 0x7F, false);
        if (JoypadState.Joypad_X) ApolloPlay(SoundEffect_Buffer3, SoundEffect_Length3, 3, 0x7F, 0x7F, false);
	    if (JoypadState.Joypad_Y) ApolloPlay(SoundEffect_Buffer4, SoundEffect_Length4, 4, 0x7F, 0x7F, false);

        if (JoypadState.Joypad_X_Delta !=0) BG_X_Delta = JoypadState.Joypad_X_Delta; else BG_X_Delta = MouseState.MouseX_Value_Delta>>2;
        if (JoypadState.Joypad_Y_Delta !=0) BG_Y_Delta = JoypadState.Joypad_Y_Delta; else BG_Y_Delta = MouseState.MouseY_Value_Delta>>2;
        
        if (BG_X_Delta != 0 && BG_X_Position + (BG_X_Delta * SAGA_SCROLL) >= BG_X_MIN && BG_X_Position + (BG_X_Delta * SAGA_SCROLL) <= (BG_X_MAX))
        {
            BackGround_Video_Buffer += (BG_X_Delta * 2 * SAGA_SCROLL);
            BG_X_Position += (BG_X_Delta * SAGA_SCROLL);
            *((volatile ULONG*)APOLLO_SAGADISPLAY_REG) = (ULONG)(BackGround_Video_Buffer);
        }
        if (BG_Y_Delta != 0 && BG_Y_Position + (BG_Y_Delta * SAGA_SCROLL) >= BG_Y_MIN && BG_Y_Position + (BG_Y_Delta * SAGA_SCROLL) <= (BG_Y_MAX))
        {
            BackGround_Video_Buffer += (BG_Y_Delta * BG_X_SIZE * 2 * SAGA_SCROLL);
            BG_Y_Position += (BG_Y_Delta * SAGA_SCROLL);
            *((volatile ULONG*)APOLLO_SAGADISPLAY_REG) = (ULONG)(BackGround_Video_Buffer);
        }

        #ifdef APOLLODEBUG
        if (BG_X_Delta > 0 || BG_Y_Delta > 0)
        {
          sprintf(ApolloDebugText,"BG_X_Position: %6d | BG_Y_Position: %6d\n", BG_X_Position, BG_Y_Position);
		  ApolloDebugPutStr(ApolloDebugText);
        }
        #endif
    }

    // Load and Show Stop Screen
    ApolloLoad("Data/Stop.16.dds", &Screen_Video_Buffer, &Screen_Video_Lenght, DDS_OFFSET, true);
    ApolloShow(Screen_Video_Buffer, Screen_Video_Lenght, SAGA_MODE, SAGA_MODULO);
}
