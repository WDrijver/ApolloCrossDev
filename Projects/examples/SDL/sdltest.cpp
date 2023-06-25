#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <SDL.h>

#include <clib/icon_protos.h>
#include <workbench/startup.h>

 
static SDL_Surface*    Surf_Canvas = NULL;
static SDL_Surface*    Surf_Picture = NULL;

static SDL_TimerID my_timer_id = 0;

static SDL_Joystick *joystick = NULL;


static int TestThread(void *ptr)
{
    int cnt;

    for (cnt = 0; cnt < 10; cnt++) {
        printf("\nThread counter: %d", cnt);
        SDL_Delay(50);
    }

    // Return the final value to the SDL_WaitThread function above
    return cnt;
}

Uint32 TestTimerCallbackFunc(Uint32 interval, void *param)
{
    
    printf("In my timer callback func()\n");
     

    return(interval);
}  
 


void cleanup(void) {
    
    if (Surf_Picture) {
        SDL_FreeSurface(Surf_Picture);
        Surf_Picture = NULL;
    } 
    
        
    if (Surf_Canvas) {
        SDL_FreeSurface(Surf_Canvas);
        Surf_Canvas = NULL;
    }
    
    if (my_timer_id) {
        SDL_RemoveTimer(my_timer_id);
        my_timer_id = 0;
    }  
    
    if (joystick) {
        SDL_JoystickClose(joystick);
        joystick = NULL;
    } 
              
    printf("Calling SDL_Quit()\n");    
    SDL_Quit();     
}          
              
int OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return -1;
    }
     
    SDL_Rect DestR;
    
    DestR.x = X;
    DestR.y = Y;   
   
    //printf("Calling SDL_BlitSurface()\n");        
    SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);
  
   /*SDL_BlitScaled(SDL_Surface*    src,
                   const SDL_Rect* srcrect,
                   SDL_Surface*    dst,
                   SDL_Rect*       dstrect)*/
  
    return 1;
}  
   
SDL_Surface* OnLoad(char* File) {
    SDL_Surface* Surf_Temp = NULL;
    SDL_Surface* Surf_Return = NULL;
 
    if((Surf_Temp = SDL_LoadBMP(File)) == NULL) {
        return NULL;
    }
 
    Surf_Return = SDL_DisplayFormat(Surf_Temp);
    SDL_FreeSurface(Surf_Temp);
 
    return Surf_Return;
}  
 
 
 
void OpenJoystick(void) {
    SDL_JoystickEventState(SDL_ENABLE);
    
    joystick = SDL_JoystickOpen(1);    
}
     
int main(int argc, char *argv[])
{
    struct WBStartup* wbStartup;
    struct DiskObject *diskObject;
    char *toolType;
    Uint32 flags = 0;
    int quit = 0;
    SDL_Event event;
    int i; 
    SDL_Rect **sizes;
    SDL_PixelFormat format;
    //SDL_Thread *thread;
    //int         threadReturnValue;
    Uint32 startTime;

    //void *my_callback_param = NULL;
    //Uint32 timerCallbackDelay = (33 / 10) * 10;  /* To round it down to the nearest 10 ms */
    
    
    if (argc != 0) {
        // Started from the shell.
        printf("You must start this sucker from the WB\n");
        return EXIT_FAILURE;
    }
    

	// Setup command line.
    wbStartup = (struct WBStartup*)argv;
    
    flags = SDL_SWSURFACE;
        
    // Process Tooltypes.
    diskObject = GetDiskObject((char*)wbStartup->sm_ArgList[0].wa_Name);
    if (diskObject != NULL) {
		toolType = (char*)FindToolType(diskObject->do_ToolTypes, "FULLSCREEN");
        if (toolType != NULL) {
            flags |= SDL_FULLSCREEN;
        }
		
		toolType = (char*)FindToolType(diskObject->do_ToolTypes, "CLOSE_WB");
        if (toolType != NULL) {
            SDL_putenv("SDL_CLOSE_WB=1");
        }
        
        toolType = (char*)FindToolType(diskObject->do_ToolTypes, "DISPLAY_MODE");
        if (toolType != NULL) {
            if (strcmp(toolType, "NTSC") == 0) {
                SDL_putenv("SDL_DISPLAY_MODE=NTSC");
            } else if (strcmp(toolType, "PAL") == 0) {
                SDL_putenv("SDL_DISPLAY_MODE=PAL");
            }
        }
        
        toolType = (char*)FindToolType(diskObject->do_ToolTypes, "RELATIVE_MOUSE");
        if (toolType != NULL) {
            SDL_putenv("SDL_MOUSE_RELATIVE=1");
        }
    }
    
    
    SDL_putenv("SDL_AUDIO_PRIORITY=1");


    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_JOYSTICK) < 0) {
        printf("Could not initialize SDL: %s\n", SDL_GetError());
        cleanup();
        return EXIT_FAILURE;
    }     
    
    startTime = SDL_GetTicks();
    printf("start time = %u\n", startTime);
    
    
    OpenJoystick();
    
    
    format.BitsPerPixel = 8;
    sizes = SDL_ListModes(&format, flags);
    if ( sizes == (SDL_Rect **)0 ) {
		printf("No sizes supported at this bit-depth\n");
	} else {              
        for ( i=0; sizes[i]; ++i ) {
            printf("Supported size w = %d h = %d\n", sizes[i]->w, sizes[i]->h);
        }
    }    
 


    if ((Surf_Canvas = SDL_SetVideoMode(320, 240, 8, flags)) == NULL) {
        printf("Could not set video mode: %s\n", SDL_GetError());
        cleanup();
        return EXIT_FAILURE;
    }
    
    SDL_WM_SetCaption("SDL_Test", "SDL_Test");
    
    

    
    
    // Create a thread
    /*thread = SDL_CreateThread(TestThread, (void *)NULL);
    if (NULL == thread) {
        printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
    } else {
        // Wait for the thread to complete. The thread functions return code will
        //       be placed in the "threadReturnValue" variable when it completes.
        //
        SDL_WaitThread(thread, &threadReturnValue);
        printf("\nThread returned value: %d\n", threadReturnValue);
    }*/
    
    // Create a Timer callback.
    //my_timer_id = SDL_AddTimer(timerCallbackDelay, TestTimerCallbackFunc, my_callback_param);
    
    
    
     
   

    printf("Calling OnLoad()\n");
    if ((Surf_Picture = OnLoad("splash_rtg_240_8.bmp")) == NULL) {
        printf("Could not load bmp: %s\n", SDL_GetError());
        cleanup();
        return 1;
    }                     
          
             
       
    /*
     * Palettized screen modes will have a default palette (a standard
     * 8*8*4 colour cube), but if the image is palettized as well we can
     * use that palette for a nicer colour matching
     */
    if (Surf_Picture->format->palette && Surf_Canvas->format->palette) {
        SDL_SetColors(Surf_Canvas, Surf_Picture->format->palette->colors, 0, Surf_Picture->format->palette->ncolors);
    }
    
         
             
    OnDraw(Surf_Canvas, Surf_Picture, 0, 0);
   

    //SDL_UpdateRect(Surf_Canvas, 0, 0, 320, 200);
    SDL_Flip(Surf_Canvas);

    
    quit = 0;
    
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1;
                    break;
            
            	case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        quit = 1;
                    }
                    break;
                     
            	case SDL_JOYAXISMOTION:
                    if( event.jaxis.axis == 0) 
                    {
                         if (event.jaxis.value == 32767) {
                            printf("Right movement code\n");
                        } else if (event.jaxis.value == -32768) {
                            printf("Left movement code\n");
                        }
                    }
            
                    if( event.jaxis.axis == 1) 
                    {
                        if (event.jaxis.value == 32767) {
                            printf("Down movement code\n");
                            printf("current time = %u\n", SDL_GetTicks());
                        } else if (event.jaxis.value == -32768) {
                            printf("Up movement code\n");
                        }
                    }
                    break;
            }
        }  
    }
    
    printf("end time = %u\n", startTime - SDL_GetTicks());
    
  
    
    // clean-up
    cleanup();
      
   
    
    printf("EXIT\n");
    return EXIT_SUCCESS;
}
