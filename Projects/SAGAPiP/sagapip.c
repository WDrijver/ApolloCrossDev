#include "../_apollo/Apollo.h"

void main()
{
    ULONG *saga_pip = (ULONG)AllocVec(80*80*2, MEMF_ANY);
    *(volatile LONG*)APOLLO_SAGA_PIP_POINTER = (ULONG)saga_pip;

    *(volatile WORD*)APOLLO_SAGA_PIP_X_START = 320;
    *(volatile WORD*)APOLLO_SAGA_PIP_X_STOP = 400;
    *(volatile WORD*)APOLLO_SAGA_PIP_Y_START = 120;
    *(volatile WORD*)APOLLO_SAGA_PIP_Y_STOP = 200;
    *(volatile WORD*)APOLLO_SAGA_PIP_GFXMODE = 0x0102; //APOLLO_SAGA_640_480<<8 + APOLLO_SAGA_16_R5G6B5;
    *(volatile WORD*)APOLLO_SAGA_PIP_MODULO = 0;

    *(volatile WORD*)APOLLO_SAGA_PIP_DMAROWS = 80*2;

    for(int row=0;row<80;row++)
    {
        for(int pixel=0;pixel<80;pixel++)
        {
            *((UWORD*)saga_pip+(pixel*2)+(row*80*2)) = 0xaaaa;
        }
    }


    
} 