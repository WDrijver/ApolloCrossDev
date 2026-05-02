// Example code for SAGA Hardware Sprite

#include "ApolloCrossDev_Lib.h"

void main()
{
    AD(ApolloDebugInit();)
    AD(ApolloDebugPutStr("SAGASprite Example Program Start\n");)

    struct ApolloPicture sprite_bitmap = {"SAGA_Sprite_32x32x8.bmp", APOLLO_BMP_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 3};
    uint8_t result = ApolloLoadPicture(&sprite_bitmap);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "SAGASprite Example Program: ERROR - Cannot load Sprite Bitmap %s (Error Code: %d)\n", sprite_bitmap.filename, result); 
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    // Set SAGA Sprite Bitmap Pointer and Sprite Size
    *(volatile uint32_t*)SAGA_VIDEO_SPRITEDATA = (uint32_t)(sprite_bitmap.buffer + sprite_bitmap.position); // Set Sprite Bitmap Pointer in SAGA Sprite Data Register
  
    


    exit:
    AD(ApolloDebugPutStr("SAGASprite Example Program End\n");)
    return;
}