// Example code for SAGA Hardware Sprite

#include "ApolloCrossDev_Lib.h"

void main(int argc, char *argv[])
{
    AD(ApolloDebugInit();)
    AD(ApolloDebugPutStr("ApolloPointer Example Program Start\n");)

    if(argc > 1)
    {
        sprintf(ApolloDebugMessage, "ApolloPointer Example Program: Command Line Argument Detected: %s\n", argv[1]); 
        ApolloDebugPutStr(ApolloDebugMessage);
    } else {
        ApolloDebugPutStr("ApolloPointer Example Program: No Command Line Argument Detected\n");
        printf("\nUsage: ApolloPointer <Sprite Bitmap Filename>\n\nExample: ApolloPointer sprite.bmp\n\nNotes:\n- Sprite Bitmap must be 32x32 pixels in size and in 24-bit BMP format.\n- Color #0 is transparant and Color #1 is semi-transparant.\n- All other colors are fully opaque.\n\n");
        goto exit;
    }   

    struct ApolloPicture sprite_bitmap = {"", APOLLO_BMP_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, 3};
    strncpy(sprite_bitmap.filename, argv[1], 255);
    uint8_t result = ApolloLoadPicture(&sprite_bitmap);
    if(result != 0x0)
    {
        sprintf(ApolloDebugMessage, "ApolloPointer Example Program: ERROR - Cannot load Sprite Bitmap %s (Error Code: %d)\n", sprite_bitmap.filename, result); 
        ApolloDebugPutStr(ApolloDebugMessage);
        goto exit;
    }

    // Set SAGA Sprite Bitmap Pointer and Sprite Size

    uint8_t *SpritePointer_Source = (sprite_bitmap.buffer + sprite_bitmap.position);
    ULONG SpritePointer_Target;
    
    SpritePointer_Target = SAGA_VIDEO_SPRITEDATA;

    UWORD pixel1, pixel2;
    ULONG pixel;
    
    for (int rows=0; rows<32; rows++)
    {
      for (int pixels=0; pixels<16; pixels++)
        {
            pixel1 = *(UBYTE*)(SpritePointer_Source + (rows * 32) + (pixels * 2) + 0);
            pixel1<<=8;
            if((pixel1 != 0xFF00) && (pixel1 != 0x0000))
            {
                pixel1|=0xFF;
            } else {
                pixel1|=0x80;
            }
           
            pixel2 = *(UBYTE*)(SpritePointer_Source + (rows * 32) + (pixels * 2) + 1);
            pixel2<<=8;
            if((pixel2 != 0xFF00) && (pixel2 != 0x0000))
            {
                pixel2|=0xFF;
            } else {
                pixel2|=0x80;
            }

            pixel = (pixel1 << 16) + pixel2;

            *(LONG *)(SpritePointer_Target) = (LONG)pixel;
            
            SpritePointer_Target=SpritePointer_Target+4;
        }
    }
    
    exit:
    AD(ApolloDebugPutStr("ApolloPointer Example Program End\n");)
    return;
}