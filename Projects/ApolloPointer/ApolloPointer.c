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
    uint8_t result = ApolloLoadPicture(&sprite_bitmap);
    if(result != 0x0)
    {
        printf("\nApolloPointer Example Program: ERROR - Cannot load Sprite Bitmap %s (Error Code: %d)\n", sprite_bitmap.filename, result); 
        goto exit;
    }

    if (sprite_bitmap.width != 32 || sprite_bitmap.height != 32)
    {
        printf("\nApolloPointer Example Program: ERROR - Sprite Bitmap %s has invalid dimensions (Width: %d, Height: %d). Expected dimensions are 32x32 pixels.\n", sprite_bitmap.filename, sprite_bitmap.width, sprite_bitmap.height); 
        goto exit;
    }

    if (sprite_bitmap.depth != 8 && sprite_bitmap.depth != 24 && sprite_bitmap.depth != 32)
    {
        printf("\nApolloPointer Example Program: ERROR - Sprite Bitmap %s has invalid color depth (Depth: %d).\nExpected color depth is either 8 bits per pixel (CLUT8), 24 bits per pixel (RGB888), or 32 bits per pixel (RGBA8888).\n", sprite_bitmap.filename, sprite_bitmap.depth); 
        goto exit;
    }

    // Set SAGA Sprite Bitmap Pointer and Sprite Size
    uint8_t *SpritePointer_Source = (sprite_bitmap.buffer + sprite_bitmap.position);
    ULONG SpritePointer_Target;
    
    SpritePointer_Target = SAGA_VIDEO_SPRITEDATA;

    if (sprite_bitmap.depth == 8)       // Source BMP = CLUT8
    {
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
    }

    if (sprite_bitmap.depth == 32)      // Source BMP = RGBA8888
    {
        printf("\nR8G8B8A8 Detected\n");
        
        ULONG pixel, color, alpha;
        UWORD currentindex, nextfreeindex;

        struct ApolloPointer sprite_pointer;

        sprite_pointer.colors[0] = 0x000000; // Color index 0 is reserved for transparency (fully transparent)

        currentindex = 1;
        nextfreeindex = 1;
                
        for (int rows=0; rows<32; rows++)
        {
            for (int pixels=0; pixels<32; pixels++)
            {
                pixel = *(ULONG*)((sprite_bitmap.buffer + sprite_bitmap.position) + (rows * 32 * 4) + (pixels * 4));
                
                color = pixel & 0x00FFFFFF;    // Get R8G8B8 part of the pixel
                alpha = (pixel>>24) & 0xFF;    // Get A8 part of the pixel
                    
                if(alpha == 0x00)
                {
                    currentindex = 0; // Fully transparent  
                } else {
                    if(alpha != 0xFF) alpha = 0x00; // Semi-transparent

                    for (currentindex=1; currentindex<nextfreeindex; currentindex++)
                    {
                        if (sprite_pointer.colors[currentindex] == color) break; // Color already exists in sprite_colors array, reuse index
                    }
                    if (currentindex == nextfreeindex) // New color, add to sprite_colors array
                    {
                        sprite_pointer.colors[nextfreeindex] = color;
                        if (nextfreeindex < 255) nextfreeindex++;
                    }
                }

                sprite_pointer.data[(rows * 32 * 2) + (pixels * 2)]     = currentindex;         // Store color index in sprite data
                sprite_pointer.data[(rows * 32 * 2) + (pixels * 2) + 1] = (UBYTE)alpha;   // Store alpha value in sprite data

                ADX(sprintf(ApolloDebugMessage, "A8R8G8B8 Pixel[%2d:%2d] = 0x%08X | Sprite-Pixel-1 (Index) = 0x%02X | Sprite-Pixel-2 (Alpha) = 0x%02X | NextFreeIndex=%03d\n", rows, pixels, pixel, currentindex, (UBYTE)(alpha), nextfreeindex);)
                ADX(ApolloDebugPutStr(ApolloDebugMessage);)
            }
        }

        ADX(sprintf(ApolloDebugMessage, "Total Unique Colors Detected (including transparency): %d\n", nextfreeindex);)
        ADX(ApolloDebugPutStr(ApolloDebugMessage);)

        for (int i=0; i<256; i++)
        {    
            *(volatile UWORD*)SAGA_VIDEO_SPRITECLUT_IDX = i;
            *(volatile ULONG*)SAGA_VIDEO_SPRITECLUT_RGB = sprite_pointer.colors[i];
        }
        ApolloCopyLongs((UBYTE*) &sprite_pointer.data, (UBYTE*)SpritePointer_Target, 64*32);
    }

    exit:
    AD(ApolloDebugPutStr("ApolloPointer Example Program End\n");)
    return;
}