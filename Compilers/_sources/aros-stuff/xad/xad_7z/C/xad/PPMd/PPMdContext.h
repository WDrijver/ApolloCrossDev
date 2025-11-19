#ifndef _PPMdContext_h
#define _PPMdContext_h

#include "CarrylessRangeCoder.h"
#include "PPMdSubAllocator.h"
#include "../7z.h"
#include <string.h>

#define MAX_O 255
#define INT_BITS 7
#define PERIOD_BITS 7
#define TOT_BITS (INT_BITS+PERIOD_BITS)
#define MAX_FREQ 124
#define INTERVAL (1<<INT_BITS)
#define BIN_SCALE (1<<TOT_BITS)

#define SWAP(t1,t2) { PPMdState tmp=(t1); (t1)=(t2); (t2)=tmp; }

#ifndef __amigaos4__
#pragma pack(1)
#endif
typedef struct SEE2Context
{ // SEE-contexts for PPM-contexts with masked symbols
	uint16_t Summ;
	uint8_t Shift,Count;
} PACKED SEE2Context;
#ifndef __amigaos4__
#pragma pack()
#endif

typedef struct PPMdContext PPMdContext;

#ifndef __amigaos4__
#pragma pack(1)
#endif
typedef struct PPMdState { uint8_t Symbol,Freq; uint32_t Successor; } PACKED PPMdState;
#ifndef __amigaos4__
#pragma pack()
#endif

#ifndef __amigaos4__
#pragma pack(1)
#endif
struct PPMdContext
{
	uint8_t LastStateIndex,Flags;
	uint16_t SummFreq;
	uint32_t States;
	uint32_t Suffix;
} PACKED;
#ifndef __amigaos4__
#pragma pack()
#endif

typedef struct PPMdCoreModel PPMdCoreModel;

struct PPMdCoreModel
{
	PPMdSubAllocator *alloc;

	CarrylessRangeCoder coder;
	uint32_t scale;

	PPMdState *FoundState; // found next state transition
	int OrderFall,InitEsc,RunLength,InitRL;
	uint8_t CharMask[256];
	uint8_t LastMaskIndex,EscCount,PrevSuccess;

	void (*RescalePPMdContext)(PPMdContext *self,PPMdCoreModel *model);
};

SEE2Context MakeSEE2(int initval,int count);
unsigned int GetSEE2MeanMasked(SEE2Context *self);
unsigned int GetSEE2Mean(SEE2Context *self);
void UpdateSEE2(SEE2Context *self);

PPMdContext *PPMdStateSuccessor(PPMdState *self,PPMdCoreModel *model);
void SetPPMdStateSuccessorPointer(PPMdState *self,PPMdContext *newsuccessor,PPMdCoreModel *model);
PPMdState *PPMdContextStates(PPMdContext *self,PPMdCoreModel *model);
void SetPPMdContextStatesPointer(PPMdContext *self, PPMdState *newstates,PPMdCoreModel *model);
PPMdContext *PPMdContextSuffix(PPMdContext *self,PPMdCoreModel *model);
void SetPPMdContextSuffixPointer(PPMdContext *self,PPMdContext *newsuffix,PPMdCoreModel *model);
PPMdState *PPMdContextOneState(PPMdContext *self);

PPMdContext *NewPPMdContext(PPMdCoreModel *model);
PPMdContext *NewPPMdContextAsChildOf(PPMdCoreModel *model,PPMdContext *suffixcontext,PPMdState *suffixstate,PPMdState *firststate);

void PPMdDecodeBinSymbol(PPMdContext *self,PPMdCoreModel *model,uint16_t *bs,int freqlimit,BOOL altnextbit);
int PPMdDecodeSymbol1(PPMdContext *self,PPMdCoreModel *model,BOOL greaterorequal);
void UpdatePPMdContext1(PPMdContext *self,PPMdCoreModel *model,PPMdState *state);
void PPMdDecodeSymbol2(PPMdContext *self,PPMdCoreModel *model,SEE2Context *see);
void UpdatePPMdContext2(PPMdContext *self,PPMdCoreModel *model,PPMdState *state);
void RescalePPMdContext(PPMdContext *self,PPMdCoreModel *model);

void ClearPPMdModelMask(PPMdCoreModel *self);
#endif
