#ifndef _CarrylessRangeCoder_h
#define _CarrylessRangeCoder_h
#include "../7z.h"
#include <stdint.h>
#include <exec/types.h>

//#include "CSInputBuffer.h"

typedef struct CarrylessRangeCoder
{
	ILookInStream *input;
	//CSInputBuffer *input;
	uint32_t low,code,range,bottom;
	BOOL uselow;
} CarrylessRangeCoder;

void InitializeRangeCoder(CarrylessRangeCoder *self,ILookInStream *input,BOOL uselow,int bottom);

uint32_t RangeCoderCurrentCount(CarrylessRangeCoder *self,uint32_t scale);
void RemoveRangeCoderSubRange(CarrylessRangeCoder *self,uint32_t lowcount,uint32_t highcount);

int NextSymbolFromRangeCoder(CarrylessRangeCoder *self,uint32_t *freqtable,int numfreq);
int NextBitFromRangeCoder(CarrylessRangeCoder *self);
int NextWeightedBitFromRangeCoder(CarrylessRangeCoder *self,int weight,int size);

int NextWeightedBitFromRangeCoder2(CarrylessRangeCoder *self,int weight,int shift);

void NormalizeRangeCoder(CarrylessRangeCoder *self);
#endif
