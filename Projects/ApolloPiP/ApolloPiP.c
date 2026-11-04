// ApolloPiP Example Code for Apollo Picture in Picture Feature
// 30-3-2026 Willem Drijver
//
// This example code demonstrates how to use the Apollo SAGA Picture in Picture (PiP) feature
//
// There are two modes:
// 1. PiP Overlay Mode   : PiP bitmap is in Front of the Main Screen and uses a fixed 0xF81F colorkey for transparency
// 2. PiP ChromaKey Mode : PiP bitmap is Behind the Main Screen and only shows through areas with a user defined chromakey 
//
// Starting ApolloCore 11900 there a TWO PiP windows available (see ApolloCrossDev_Defines.h)

#include "ApolloCrossDev_Lib.h"

extern char ApolloDebugMessage[256];

int main(void)
{
    AD(ApolloDebugInit();)
    AD(ApolloDebugPutStr("ApolloPiP 3.0 Example Program Start\n");)
   
    struct Screen *mainscreen = LockPubScreen(NULL);

    uint8_t result;

    // All resources start cleared, so the single exit path frees only what was acquired
    struct ApolloSound soundtrack = {0};
    struct ApolloPicture pip_background_bitmap = {0}, pip_window_picture = {0};
    struct ApolloPicture pip1_overlay_picture = {0}, pip2_overlay_picture = {0};
    struct Library *CyberGfxBase = NULL;
    struct BitMap *pip_window_bitmap = NULL;
    struct Window *pipwindow = NULL;
    bool sound_playing = false;
    APTR handle;

    // Save the current SAGA mode, chunky pointer and modulo from their read-back shadows at $DFExxx
    uint16_t saved_gfxmode = *(volatile uint16_t*)(APOLLO_SAGA_GFXMODE - 0x1000);
    uint32_t saved_pointer = *(volatile uint32_t*)(APOLLO_SAGA_POINTER - 0x1000);
    uint16_t saved_modulo  = *(volatile uint16_t*)(APOLLO_SAGA_MODULO - 0x1000);

    // Step 1: Load and Play Soundtrack

    strcpy(soundtrack.filename, "DOTC-Peter_Clarke_Intro.aiff");
    soundtrack.format = APOLLO_AIFF_FORMAT;
    soundtrack.loop = true;
    soundtrack.fadein = true;
    soundtrack.volume_left = 0x88;
    soundtrack.volume_right = 0x88;
    soundtrack.staticchannel = false;

    result = ApolloLoadSound(&soundtrack);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot load PiP Soundtrack %s (Error Code: %d)\n", soundtrack.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit; 
    }

    result = ApolloPlaySound(&soundtrack);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot play Soundtrack %s (Error Code: %d)\n", soundtrack.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit; 
    }
    sound_playing = true;

    // Step 2: Load PiP Background

    // Step 2.1 Secure WorkBench Screen Pointer

    CyberGfxBase = OpenLibrary((CONST_STRPTR)"cybergraphics.library", 0);
    if (!CyberGfxBase)
    {
        ApolloDebugPutStr("ApolloPiP Example Program: ERROR - Cannot open cybergraphics.library\n");
        goto exit;
    }

    strcpy(pip_background_bitmap.filename, "RHLOS_1280x720x16.dds");
    pip_background_bitmap.format = APOLLO_DDS_FORMAT;
    pip_background_bitmap.endian = true;
    pip_background_bitmap.target = APOLLO_PIC_TARGET_SAGA;
    
    result = ApolloLoadPicture(&pip_background_bitmap);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot load PiP Background Bitmap %s (Error Code: %d)\n", pip_background_bitmap.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    result = ApolloShowPicture(&pip_background_bitmap);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot show PiP Background Bitmap %s (Error Code: %d)\n", pip_background_bitmap.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    Delay(100);                                                 // 2 seconds (dos.library, 50 ticks per second)

    // Step 2.1 = Restore WorkBench Screen

    *(volatile LONG*)APOLLO_SAGA_POINTER = saved_pointer;
    *(volatile uint16_t*)APOLLO_SAGA_GFXMODE = saved_gfxmode;   // the mode saved at the start, not a fixed $0A04
    *(volatile uint16_t*)APOLLO_SAGA_MODULO = saved_modulo;

    // Step 3: Load and Show PiP Window

    // Step 3.1 = Load PiP Window Bitmap
    
    strcpy(pip_window_picture.filename, "DOTC-640x480x24.dds");
    pip_window_picture.format = APOLLO_DDS_FORMAT;
    pip_window_picture.endian = true;
    pip_window_picture.target = APOLLO_PIC_TARGET_SAGA;
    result = ApolloLoadPicture(&pip_window_picture);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot load PiP Window Bitmap %s (Error Code: %d)\n", pip_window_picture.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    // Step 3.2 = Prepare PiP Window BitMap 
    pip_window_bitmap = AllocBitMap(640, 480, 24, BMF_DISPLAYABLE | BMF_CLEAR | BMF_INTERLEAVED, mainscreen->RastPort.BitMap );
    if (!pip_window_bitmap)
    {
        ApolloDebugPutStr("ApolloPiP Example Program: ERROR - Cannot allocate PiP Window BitMap\n");
        goto exit;
    }    

    sprintf(ApolloDebugMessage,"AllocBitMap PiP Window BitMap: BPR = %d | Rows = %d | Flags = %x | Depth = %d | Pad = %x\n",
         pip_window_bitmap->BytesPerRow, pip_window_bitmap->Rows, pip_window_bitmap->Flags, pip_window_bitmap->Depth, pip_window_bitmap->pad );
    ApolloDebugPutStr(ApolloDebugMessage);

    // NOTE: AllocBitMap returns different results on P96 (AmigaOS) versys CyberGraphx (ApolloOS) graphics drivers.
    // So we cannot assume any BitMap layout and need to use CyberGraphx functions which are compatible with both drivers.
    
    // Step 3.3 = Acquire a pointer to PiP Window Bitmap

    APTR    pip_window_bitmap_pointer;
    
    handle = LockBitMapTags(pip_window_bitmap, LBMI_BASEADDRESS, (ULONG)&pip_window_bitmap_pointer, TAG_DONE);
        
    // Step 3.4 = Copy PiP WIndow BitMap into PiP Window BitMap

    ApolloCopyPicture32((uint8_t*)(pip_window_picture.buffer + pip_window_picture.position),
     (uint8_t*)pip_window_bitmap_pointer, pip_window_picture.width*(pip_window_picture.depth/8), pip_window_picture.height, 0, 0);

    UnLockBitMap(handle);

    // Step 3.4 = Prepare PiP Window Structure and Open Window in WorkBench
    
    struct NewWindow *pip_window_template = (struct NewWindow*)AllocVec(sizeof(struct NewWindow), MEMF_ANY);
    pip_window_template->LeftEdge = 320;  
    pip_window_template->TopEdge = 120;
    pip_window_template->Width = 640;
    pip_window_template->Height = 480;
    pip_window_template->DetailPen = 0;
    pip_window_template->BlockPen = 1;
    pip_window_template->IDCMPFlags = IDCMP_CLOSEWINDOW | IDCMP_CHANGEWINDOW | IDCMP_MOUSEBUTTONS;
    pip_window_template->Flags =  WFLG_SUPER_BITMAP | WFLG_DRAGBAR | WFLG_CLOSEGADGET | WFLG_RMBTRAP | WFLG_ACTIVATE | WFLG_NOCAREREFRESH ; // 
    pip_window_template->FirstGadget = NULL;
    pip_window_template->CheckMark = NULL;
    pip_window_template->Title = (UBYTE *)"Apollo SAGA PiP Window";
    pip_window_template->Screen = mainscreen;
    pip_window_template->BitMap = pip_window_bitmap;
    pip_window_template->MinWidth = 640;
    pip_window_template->MinHeight = 480;
    pip_window_template->MaxWidth = 640;
    pip_window_template->MaxHeight = 480;
    pip_window_template->Type = WBENCHSCREEN;

    pipwindow = OpenWindowTags(pip_window_template, TAG_DONE);
    FreeVec(pip_window_template);
    if (!pipwindow)
    {
        ApolloDebugPutStr("ApolloPiP Example Program: ERROR - Cannot open Workbench Window\n");
        goto exit;
    } else {
        sprintf(ApolloDebugMessage,"ApolloPiP Example Program: Workbench Window opened at (X=%d,Y=%d) with size (W=%d,H=%d)\n",
        pipwindow->LeftEdge, pipwindow->TopEdge, pipwindow->Width, pipwindow->Height);
        ApolloDebugPutStr(ApolloDebugMessage);
    }

    Delay(150);                                                 // 3 seconds

    // Step 4 = Load and Show PiP *Overlay* on Top of PiP Window Bitmap

    // Step 4.1 = Load PiP-1 Overlay Bitmap

    strcpy(pip1_overlay_picture.filename, "DOTC-640x480x8.bmp");
    pip1_overlay_picture.format = APOLLO_BMP_FORMAT;
    pip1_overlay_picture.endian = true;
    pip1_overlay_picture.target = APOLLO_PIC_TARGET_PIP1;
    result = ApolloLoadPicture(&pip1_overlay_picture);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot load PiP Overlay Bitmap %s (Error Code: %d)\n", pip1_overlay_picture.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    // Step 4.2 = Set Apollo SAGA PiP-1 Registers for PiP Overlay Mode
    
    *(volatile LONG*)APOLLO_SAGA_PIP1_POINTER = (uint32_t)(pip1_overlay_picture.buffer + pip1_overlay_picture.position);      // Set PiP Bitmap Pointer
    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_START = pipwindow->LeftEdge + 16;                                                // Set PiP X Start Position (+16 pixel correction needed) 
    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_STOP = pipwindow->LeftEdge + pip1_overlay_picture.width + 16 ;                    // Set PiP X Stop Position (+16 pixel correction needed) 
    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_START = pipwindow->TopEdge - 1;                                                  // Set PiP Y Start Position (-1 pixel correction needed)     
    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_STOP = pipwindow->TopEdge + pip1_overlay_picture.height - 1;                      // Set PiP Y Stop Position (-1 pixel correction needed)
    *(volatile int16_t*)APOLLO_SAGA_PIP1_GFXMODE = APOLLO_SAGA_PIP_TRANSON + APOLLO_SAGA_8_INDEX;                           // Match Apollo SAGA with PiP Overlay Bitmap format                         
    *(volatile int16_t*)APOLLO_SAGA_PIP1_MODULO = 0;                                                                        // No Modulo (PiP Bitmap width matches PiP Window width)                                   
    *(volatile int16_t*)APOLLO_SAGA_PIP1_CLRKEY = 0x0000;                                                                   // Colorkey = 0 -> ChromKey mode disable -> Overlay Mode Enabled
    ApolloShowPiP1(&pip1_overlay_picture);                 // DMA fetch = number of pixels per row = width*bytes per pixel
    
    // Step 4.3 = Load PiP-2 Overlay Bitmap

    strcpy(pip2_overlay_picture.filename, "DOTC2-320x240x8.bmp");
    pip2_overlay_picture.format = APOLLO_BMP_FORMAT;
    pip2_overlay_picture.endian = true;
    pip2_overlay_picture.target = APOLLO_PIC_TARGET_PIP2;
    result = ApolloLoadPicture(&pip2_overlay_picture);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPiP Example Program: ERROR - Cannot load PiP Overlay Bitmap %s (Error Code: %d)\n", pip2_overlay_picture.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    // Step 4.4 = Set Apollo SAGA PiP-2 Registers for PiP Overlay Mode
    
    *(volatile LONG*)APOLLO_SAGA_PIP2_POINTER = (uint32_t)(pip2_overlay_picture.buffer + pip2_overlay_picture.position);      // Set PiP Bitmap Pointer
    *(volatile int16_t*)APOLLO_SAGA_PIP2_X_START = pipwindow->LeftEdge + 16 + 640;                                                // Set PiP X Start Position (+16 pixel correction needed) 
    *(volatile int16_t*)APOLLO_SAGA_PIP2_X_STOP = pipwindow->LeftEdge + pip2_overlay_picture.width + 16 + 640;                    // Set PiP X Stop Position (+16 pixel correction needed) 
    *(volatile int16_t*)APOLLO_SAGA_PIP2_Y_START = pipwindow->TopEdge - 1;                                                  // Set PiP Y Start Position (-1 pixel correction needed)     
    *(volatile int16_t*)APOLLO_SAGA_PIP2_Y_STOP = pipwindow->TopEdge + pip2_overlay_picture.height - 1;                      // Set PiP Y Stop Position (-1 pixel correction needed)
    *(volatile int16_t*)APOLLO_SAGA_PIP2_GFXMODE = APOLLO_SAGA_PIP_TRANSON + APOLLO_SAGA_8_INDEX;                           // Match Apollo SAGA with PiP Overlay Bitmap format                         
    *(volatile int16_t*)APOLLO_SAGA_PIP2_MODULO = 0;                                                                        // No Modulo (PiP Bitmap width matches PiP Window width)                                   
    *(volatile int16_t*)APOLLO_SAGA_PIP2_CLRKEY = 0x0000;                                                                   // Colorkey = 0 -> ChromKey mode disable -> Overlay Mode Enabled
    ApolloShowPiP2(&pip2_overlay_picture);                 // DMA fetch = number of pixels per row = width*bytes per pixel
     
    // Step 4.5 = Process Window Changes (Update PiP position, Enable Transparency on Mouseclick, Close Window)
    
    struct IntuiMessage *message;    
    bool close = false;
    
    while (close == false)
    {                                                   
        WaitPort(pipwindow->UserPort);
        
        while(message = GT_GetIMsg(pipwindow->UserPort))
        {      
            switch(message->Class)
            {
                case IDCMP_CHANGEWINDOW:                    // Window has been moved or resized
                    ApolloDebugPutStr("Window Change Event\n");
                    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_START = pipwindow->LeftEdge + 16;
                    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_STOP = pipwindow->LeftEdge + pip1_overlay_picture.width + 16;
                    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_START = pipwindow->TopEdge -1;
                    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_STOP = pipwindow->TopEdge + pip1_overlay_picture.height -1;
                    *(volatile int16_t*)APOLLO_SAGA_PIP2_X_START = pipwindow->LeftEdge + 16 + 640;
                    *(volatile int16_t*)APOLLO_SAGA_PIP2_X_STOP = pipwindow->LeftEdge + pip2_overlay_picture.width + 16 + 640;
                    *(volatile int16_t*)APOLLO_SAGA_PIP2_Y_START = pipwindow->TopEdge -1;
                    *(volatile int16_t*)APOLLO_SAGA_PIP2_Y_STOP = pipwindow->TopEdge + pip2_overlay_picture.height -1;
                    break;

                case IDCMP_MOUSEBUTTONS:
                    ApolloDebugPutHex("Mousebutton Event", message->Code);
                    switch(message->Code)                   // Mouse button event    
                    {
                        case SELECTDOWN:                    // Fill a Square with Transparent Pixels
                            ApolloDebugPutStr("ApolloPiP Example Program: Mouse Button Event - SELECT DOWN\n");
                            *(volatile uint32_t*)APOLLO_SAGA_PIP1CHK_COL = 0xFFFF00FF;  // Set PiP Overlay Color [FF] to R=0xFF G=0x00 B=0xFF
                            ApolloFillBitMap(pip1_overlay_picture.buffer + 160*(pip1_overlay_picture.depth/8) + (120*640*(pip1_overlay_picture.depth/8)), 320, 240, pip1_overlay_picture.depth, 320, 0xFFFFFFFF); 
                            
                            break;
                        case MENUDOWN:                         
                            ApolloDebugPutStr("Close Window Event\n");
                            close = true; 
                            break;
                    }
                    break;  
            }
            GT_ReplyIMsg(message);
        }
    }

    close = false;
    ApolloHidePiP1();
    ApolloHidePiP2();

    // Step 5 - PiP ChromaKey Mode - Use WorkBench Window as Chromakey to "peep though" and see the Background Bitmap
    
    // Step 5.1 Fill WorkBench Window with Chromakey Color (BLACK = 0x0000)

    uint32_t ypos = ((pipwindow->TopEdge + pipwindow->BorderTop) * 1280 * (pip_window_picture.depth/8));
    uint32_t xpos = ((pipwindow->LeftEdge + pipwindow->BorderLeft) * (pip_window_picture.depth/8));
    uint32_t width = pip_window_picture.width - pipwindow->BorderLeft - pipwindow->BorderRight;
    uint32_t depth = pip_window_picture.depth;
    uint32_t height = pip_window_picture.height - pipwindow->BorderTop - pipwindow->BorderBottom;
    uint32_t dstmod = GetBitMapAttr(pipwindow->RPort->BitMap, BMA_WIDTH) - width;
    
    handle = LockBitMapTags(pipwindow->RPort->BitMap, LBMI_BASEADDRESS, (ULONG)&pip_window_bitmap_pointer, TAG_DONE);
    
    //ApolloFillBitMap((uint8_t*)(pip_window_bitmap_pointer + xpos + ypos), width, height, depth, dstmod, 0x00000000);

    ApolloFillColor((uint8_t*)(pip_window_bitmap_pointer + xpos + ypos), width, height, depth, dstmod, 0xFF00FF);                           // Fill PiP Background with 16-Bit R5B6G5 Transparent 0xFF00FF PURPLE Color

    UnLockBitMap(handle);

    // Step 5.2 Set the correct SAGA PiP register values
    *(volatile LONG*)APOLLO_SAGA_PIP1_POINTER = (uint32_t)(pip_background_bitmap.buffer+ pip_background_bitmap.position);                    // Set PiP Bitmap Pointer
    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_START = 16;                                                                                       // Set PiP X Start Position (+16 pixel correction needed)   
    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_STOP =  1280+16;                                                                                  // Set PiP X Stop Position (+16 pixel correction needed)
    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_START = 0;                                                                                        // Set PiP Y Start Position                                                
    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_STOP = 720;                                                                                       // Set PiP Y Stop Position                                      
    *(volatile int16_t*)APOLLO_SAGA_PIP1_GFXMODE = APOLLO_SAGA_16_R5G6B5;                                                                    // Match Apollo SAGA with PiP Background Bitmap format
    *(volatile int16_t*)APOLLO_SAGA_PIP1_MODULO = 0;                                                                                         // No Modulo (PiP Bitmap width matches PiP Window width)                 
    *(volatile int16_t*)APOLLO_SAGA_PIP1_CLRKEY = 0x8F0F;                                                                                    // Enable Chromakey (0x8) + R (0xF) + G (0x0) + B (0xF) = PURPLE Color
    ApolloShowPiP1(&pip_background_bitmap);                    // DMA row length = 1280 x 2 bytes: PIP on
      
    while (close == false)
    {                                                   
        WaitPort(pipwindow->UserPort);

        while(message=GT_GetIMsg(pipwindow->UserPort))
        {
            switch(message->Class)
            {
                case IDCMP_CLOSEWINDOW:                     // Close window event
                    ApolloDebugPutStr("ApolloPiP Example Program: Close Window Event\n");
                    close = true; 
                    break;
            }
            GT_ReplyIMsg(message);                          // Reply message
        }
    }
        
    ApolloHidePiP1();

 
exit:
    ApolloHidePiP1();
    ApolloHidePiP2();
    *(volatile LONG*)APOLLO_SAGA_POINTER = saved_pointer;        // back to the saved display
    *(volatile uint16_t*)APOLLO_SAGA_GFXMODE = saved_gfxmode;
    *(volatile uint16_t*)APOLLO_SAGA_MODULO = saved_modulo;

    if (pipwindow) CloseWindow(pipwindow);
    if (pip_window_bitmap) FreeBitMap(pip_window_bitmap);

    ApolloFreePicture(&pip1_overlay_picture);                    // ApolloFreePicture skips a picture that was never loaded
    ApolloFreePicture(&pip2_overlay_picture);
    ApolloFreePicture(&pip_window_picture);
    ApolloFreePicture(&pip_background_bitmap);

    if (sound_playing)
    {
        if (soundtrack.fadeout) ApolloFadeOutSound(&soundtrack);
        ApolloStopSound(&soundtrack);
    }
    ApolloFreeSound(&soundtrack);

    if (CyberGfxBase) CloseLibrary(CyberGfxBase);
    UnlockPubScreen(NULL, mainscreen);
    AD(ApolloDebugPutStr("ApolloPiP Example Program End\n");)
    return 0;   
}