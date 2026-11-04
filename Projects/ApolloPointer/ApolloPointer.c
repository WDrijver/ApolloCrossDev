// Example code for SAGA Hardware Sprite

#include "ApolloCrossDev_Lib.h"

static struct ApolloPointer sprite_pointer;     // over 5 KB: keep it off the stack

int main(int argc, char *argv[])
{
    int rc = 0;
    AD(ApolloDebugInit();)
    AD(ApolloDebugPutStr("ApolloPointer Example Program Start\n");)

    if(argc > 1)
    {
        AD(sprintf(ApolloDebugMessage, "ApolloPointer Example Program: Command Line Argument Detected: %s\n", argv[1]);) 
        AD(ApolloDebugPutStr(ApolloDebugMessage);)
    } else {
        printf("\nUsage: ApolloPointer <Sprite Bitmap Filename>\n\nExample: ApolloPointer sprite.bmp\n\nNotes:\n- Sprite Bitmap must be 32x32 pixels, as an 8-bit (CLUT8), 24-bit or 32-bit (ARGB) BMP.\n- Color #0 is transparent; in a 32-bit BMP, alpha 0 is transparent and any alpha below 255 is 50 %% semi-transparent.\n- All other colors are fully opaque.\n\n");
        rc = 5;                                     // RETURN_WARN
        goto exit;
    }   

    struct ApolloPicture sprite_bitmap = {0};
    strncpy(sprite_bitmap.filename, argv[1], 255);
    sprite_bitmap.format = APOLLO_BMP_FORMAT;
    sprite_bitmap.target = APOLLO_PIC_TARGET_SPRITE;

    uint8_t result = ApolloLoadPointer(&sprite_pointer, &sprite_bitmap);
    if (result != APOLLO_PICTURE_OK)
    {
        printf("ApolloPointer: cannot use %s as pointer (error %d)\n", argv[1], result);
        rc = 10;                                    // RETURN_ERROR
        goto exit;
    }

    ApolloShowPointer(&sprite_pointer);

    exit:
    AD(ApolloDebugPutStr("ApolloPointer Example Program End\n");)
    return rc;
}
