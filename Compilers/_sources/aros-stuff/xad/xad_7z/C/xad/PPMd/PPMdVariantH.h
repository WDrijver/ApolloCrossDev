#ifndef _PPMdVariantH_h
#define _PPMdVariantH_h

#include "PPMdContext.h"
#include "PPMdSubAllocatorVariantH.h"

#define YES TRUE
#define NO FALSE

// PPMd Variant H. Used by RAR and 7-Zip.

typedef struct PPMdModelVariantH
{
	PPMdCoreModel core;

	PPMdSubAllocatorVariantH *alloc;

	PPMdContext *MinContext,*MaxContext;
	int MaxOrder,HiBitsFlag;
	BOOL SevenZip;
	SEE2Context SEE2Cont[25][16],DummySEE2Cont;
	uint8_t NS2BSIndx[256],HB2Flag[256],NS2Indx[256];
	uint16_t BinSumm[128][64]; // binary SEE-contexts
} PPMdModelVariantH;

void StartPPMdModelVariantH(PPMdModelVariantH *self,ILookInStream *input,
PPMdSubAllocatorVariantH *alloc,int maxorder,BOOL sevenzip);
int NextPPMdVariantHByte(PPMdModelVariantH *self);

#endif
