#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <clib/exec_protos.h>
#include <exec/types.h>

int main()
{
    UWORD* const ApolloCoreRelease = (UWORD*)0xDFF3EA;

    printf("%d", *ApolloCoreRelease);

    return;
}   

