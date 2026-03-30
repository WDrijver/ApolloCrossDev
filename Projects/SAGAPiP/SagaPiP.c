// SAGAPiP Example Code for Apollo Picture in Picture Feature
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

uint32_t test;

void main()
{
    AD(ApolloDebugInit();)
    AD(ApolloDebugPutStr("SAGAPiP 3.0 Example Program Start\n");)
   
    struct Screen *mainscreen = LockPubScreen(NULL);

    uint8_t result;

    // Step 1: Load and Play Soundtrack

    struct ApolloSound soundtrack;
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
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot load PiP Soundtrack %s (Error Code: %d)\n", soundtrack.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit1; 
    }

    result = ApolloPlaySound(&soundtrack);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot play Soundtrack %s (Error Code: %d)\n", soundtrack.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit2; 
    }

    // Step 2: Load PiP Background

    // Step 2.1 Secure WorkBench Screen Pointer

    struct Library *CyberGfxBase = OpenLibrary("cybergraphics.library", 0);
    if (!CyberGfxBase)
    {
        ApolloDebugPutStr("SAGAPiP Example Program: ERROR - Cannot open cybergraphics.library\n");
        goto exit4;
    }

    APTR    workbench_screen_pointer;
    APTR    handle;

    struct Screen *workbench_screen = LockPubScreen("Workbench");

    handle = LockBitMapTags(workbench_screen->RastPort.BitMap, LBMI_BASEADDRESS, (ULONG)&workbench_screen_pointer, TAG_DONE);
    
    UnLockBitMap(handle);

    struct ApolloPicture pip_background_bitmap = {"RHLOS_1280x720x16.dds", APOLLO_DDS_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    
    result = ApolloLoadPicture(&pip_background_bitmap);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot load PiP Background Bitmap %s (Error Code: %d)\n", pip_background_bitmap.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit2;
    }

    result = ApolloShowPicture(&pip_background_bitmap);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot show PiP Background Bitmap %s (Error Code: %d)\n", pip_background_bitmap.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit3;
    }

    ApolloCPUDelay(2000);

    // Step 2.1 = Restore WorkBench Screen

    *(volatile LONG*)APOLLO_SAGA_POINTER = (uint32_t)(workbench_screen_pointer);
    *(volatile uint16_t*)APOLLO_SAGA_GFXMODE = 0x0A04; // Set SAGA Gfxmode to 1280x720x24-Bit
    *(volatile uint16_t*)APOLLO_SAGA_MODULO = 0;

    // Step 3: Load and Show PiP Window

    // Step 3.1 = Load PiP Window Bitmap
    
    struct ApolloPicture pip_window_picture = {"DOTC-640x480x24.dds", APOLLO_DDS_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    result = ApolloLoadPicture(&pip_window_picture);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot load PiP Window Bitmap %s (Error Code: %d)\n", pip_window_picture.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit3;
    }

    // Step 3.2 = Prepare PiP Window BitMap 
    struct BitMap *pip_window_bitmap = AllocBitMap(640, 480, 24, BMF_DISPLAYABLE | BMB_CLEAR | BMB_HIJACKED | BMB_INTERLEAVED | BMB_RTGCHECK , mainscreen->RastPort.BitMap );
    if (!pip_window_bitmap)
    {
        ApolloDebugPutStr("SAGAPiP Example Program: ERROR - Cannot allocate PiP Window BitMap\n");
        goto exit4;
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
    pip_window_template->Title = "Apollo SAGA PiP Window";
    pip_window_template->Screen = mainscreen;
    pip_window_template->BitMap = pip_window_bitmap;
    pip_window_template->MinWidth = 640;
    pip_window_template->MinHeight = 480;
    pip_window_template->MaxWidth = 640;
    pip_window_template->MaxHeight = 480;
    pip_window_template->Type = WBENCHSCREEN;

    struct Window *pipwindow = OpenWindowTags(pip_window_template, TAG_DONE);
    if (!&pipwindow)
    {
        ApolloDebugPutStr("SAGAPiP Example Program: ERROR - Cannot open Workbench Window\n");
        goto exit1;
    } else {
        sprintf(ApolloDebugMessage,"SAGAPiP Example Program: Workbench Window opened at (X=%d,Y=%d) with size (W=%d,H=%d)\n",
        pipwindow->LeftEdge, pipwindow->TopEdge, pipwindow->Width, pipwindow->Height);
        ApolloDebugPutStr(ApolloDebugMessage);
    }
    FreeVec(pip_window_template);

    ApolloCPUDelay(3000);

    // Step 4 = Load and Show PiP *Overlay* on Top of PiP Window Bitmap

    // Step 4.1 = Load PiP-1 Overlay Bitmap

    struct ApolloPicture pip1_overlay_picture = {"DOTC-640x480x8.bmp", APOLLO_BMP_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 1};
    result = ApolloLoadPicture(&pip1_overlay_picture);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot load PiP Overlay Bitmap %s (Error Code: %d)\n", pip1_overlay_picture.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit5;
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
    *(volatile int16_t*)APOLLO_SAGA_PIP1_DMAROWS = pip1_overlay_picture.width*(pip1_overlay_picture.depth/8);                 // DMA fetch = number of pixels per row = width*bytes per pixel
    
    // Step 4.3 = Load PiP-2 Overlay Bitmap

    struct ApolloPicture pip2_overlay_picture = {"DOTC2-320x240x8.bmp", APOLLO_BMP_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 2};
    result = ApolloLoadPicture(&pip2_overlay_picture);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGAPiP Example Program: ERROR - Cannot load PiP Overlay Bitmap %s (Error Code: %d)\n", pip2_overlay_picture.filename, result);
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit5;
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
    *(volatile int16_t*)APOLLO_SAGA_PIP2_DMAROWS = pip2_overlay_picture.width*(pip2_overlay_picture.depth/8);                 // DMA fetch = number of pixels per row = width*bytes per pixel
    //*(volatile uint32_t*)APOLLO_SAGA_PIP2CHK_COL = 0x00FF00FF;                                                              // Set PiP Overlay Colorkey to R=0xFF G=0x00 B=0xFF
     
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
                            ApolloDebugPutStr("SAGAPiP Example Program: Mouse Button Event - SELECT DOWN\n");
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
    *(volatile int16_t*)APOLLO_SAGA_PIP1_DMAROWS = 0;       // DMA fetch = number of pixels per row (0 = disable)
    *(volatile int16_t*)APOLLO_SAGA_PIP2_DMAROWS = 0;       // DMA fetch = number of pixels per row (0 = disable)

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

    ApolloFillColor((uint8_t*)(pip_window_bitmap_pointer + xpos + ypos), width, height, depth, dstmod, 0xFF00FF); // Fill PiP Background with 16-Bit R5B6G5 Transparent 0xF81F Color

    UnLockBitMap(handle);

    // Step 5.2 Set the correct SAGA PiP register values
    *(volatile LONG*)APOLLO_SAGA_PIP1_POINTER = (uint32_t)(pip_background_bitmap.buffer+ pip_background_bitmap.position);                    // Set PiP Bitmap Pointer
    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_START = 16;                                                                                       // Set PiP X Start Position (+16 pixel correction needed)   
    *(volatile int16_t*)APOLLO_SAGA_PIP1_X_STOP =  1280+16;                                                                                  // Set PiP X Stop Position (+16 pixel correction needed)
    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_START = 0;                                                                                        // Set PiP Y Start Position                                                
    *(volatile int16_t*)APOLLO_SAGA_PIP1_Y_STOP = 720;                                                                                       // Set PiP Y Stop Position                                      
    *(volatile int16_t*)APOLLO_SAGA_PIP1_GFXMODE = APOLLO_SAGA_16_R5G6B5;                                                                    // Match Apollo SAGA with PiP Background Bitmap format
    *(volatile int16_t*)APOLLO_SAGA_PIP1_MODULO = 0;                                                                                         // No Modulo (PiP Bitmap width matches PiP Window width)                 
    *(volatile int16_t*)APOLLO_SAGA_PIP1_CLRKEY = 0x8F0F;                                                                                    // Enable Chromakey (0x8) + R (0x0) + G (0x0) + B (0x0) = BLACK
    *(volatile int16_t*)APOLLO_SAGA_PIP1_DMAROWS = 1280*2;                                                                                   // DMA fetch = number of pixels per row
      
    while (close == false)
    {                                                   
        WaitPort(pipwindow->UserPort);

        while(message=GT_GetIMsg(pipwindow->UserPort))
        {
            switch(message->Class)
            {
                case IDCMP_CLOSEWINDOW:                     // Close window event
                    ApolloDebugPutStr("SAGAPiP Example Program: Close Window Event\n");
                    close = true; 
                    break;
            }
            GT_ReplyIMsg(message);                          // Reply message
        }
    }
        
    *(volatile int16_t*)APOLLO_SAGA_PIP1_DMAROWS = 0;       // DMA fetch = number of pixels per row (0 = disable)

 
exit5:    
    CloseWindow(pipwindow); 
    FreeBitMap(pip_window_bitmap);

exit4:
    FreeVec(pip1_overlay_picture.buffer);   
    FreeVec(pip2_overlay_picture.buffer);                 
    FreeVec(pip_window_picture.buffer);

exit3:    
    FreeVec(pip_background_bitmap.buffer);

exit2:
    if (soundtrack.fadeout) ApolloFadeOutSound(&soundtrack);
    ApolloStopSound(&soundtrack);
    FreeVec(soundtrack.buffer);

exit1: 
    UnlockPubScreen(NULL, mainscreen);
    AD(ApolloDebugPutStr("SAGAPiP Example Program End\n");)
    return;   
}