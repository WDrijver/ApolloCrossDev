/* 7zAlloc.h -- Allocation functions
2008-03-28
Igor Pavlov
Public domain */

#ifndef __7Z_ALLOC_H
#define __7Z_ALLOC_H

#include "7z.h"
#include <stddef.h>

void *SzAlloc(void *p, size_t size);
void SzFree(void *p, void *address);

void *SzAllocTemp(void *p, size_t size);
void SzFreeTemp(void *p, void *address);

void *BzAlloc(void *opaque,size_t items,size_t size);
void BzFree(void *opaque,void *addr);
#endif
