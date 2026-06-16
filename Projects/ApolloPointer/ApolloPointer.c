// Example code for SAGA Hardware Sprite

#include "ApolloCrossDev_Lib.h"

void main(int argc, char *argv[])
{
    AD(ApolloDebugInit();)
    AD(ApolloDebugPutStr("ApolloPointer Example Program Start\n");)

    if(argc > 1)
    {
        AD(sprintf(ApolloDebugMessage, "ApolloPointer Example Program: Command Line Argument Detected: %s\n", argv[1]);) 
        AD(ApolloDebugPutStr(ApolloDebugMessage);)
    } else {
        printf("\nUsage: ApolloPointer <Sprite Bitmap Filename>\n\nExample: ApolloPointer sprite.bmp\n\nNotes:\n- Sprite Bitmap must be 32x32 pixels in size and in CLUT8 BMP format.\n- Color #0 is transparant and Color #1 is semi-transparant.\n- All other colors are fully opaque.\n\n");
        goto exit;
    }   

    struct ApolloPicture sprite_bitmap = {"", APOLLO_BMP_FORMAT, true, NULL, 0, 0, 0, 0, 0, 0, 0, 0, APOLLO_PIC_TARGET_SPRITE};
    strncpy(sprite_bitmap.filename, argv[1], 255);

    struct ApolloPointer sprite_pointer;
    ApolloLoadPointer(&sprite_pointer, &sprite_bitmap);

    ApolloShowPointer(&sprite_pointer);

    exit:
    AD(ApolloDebugPutStr("ApolloPointer Example Program End\n");)
    return;
}