#include "ApolloCrossDev_Defines.h"
#include "ApolloCrossDev_Library.h"

void main()
{
    uint32_t *saga_pip_unaligned = (uint32_t*)AllocVec((640*480*2)+31, MEMF_ANY);
    uint32_t *saga_pip_aligned32 = (uint32_t*)( ((uint32_t)saga_pip_unaligned+31) & ~31); 

    *(volatile LONG*)APOLLO_SAGA_PIP_POINTER = (uint32_t)saga_pip_aligned32;

    *(volatile WORD*)APOLLO_SAGA_PIP_X_START = 320;
    *(volatile WORD*)APOLLO_SAGA_PIP_X_STOP = 960;
    *(volatile WORD*)APOLLO_SAGA_PIP_Y_START = 120;
    *(volatile WORD*)APOLLO_SAGA_PIP_Y_STOP = 600;
    *(volatile WORD*)APOLLO_SAGA_PIP_GFXMODE = APOLLO_SAGA_PIP_TRANSON + APOLLO_SAGA_16_R5G6B5;
    *(volatile WORD*)APOLLO_SAGA_PIP_MODULO = 0;
    *(volatile WORD*)APOLLO_SAGA_PIP_CLRKEY = 0;

    *(volatile WORD*)APOLLO_SAGA_PIP_DMAROWS = 640*2;

    ApolloFill(saga_pip_aligned32, 640*2, 480, 0, (APOLLO_SAGA_PIP_TRANS16<<16)+APOLLO_SAGA_PIP_TRANS16);

    for(int row=0;row<240;row++)
    {
        for(int pixel=0;pixel<(640*2);pixel+=2)
        {
           *(uint64_t*)((uint32_t)saga_pip_aligned32+pixel+(row*640*2)) = 0x0000000000000000;
        }
    }

    //*(volatile WORD*)APOLLO_SAGA_PIP_DMAROWS = 0;
    
} 