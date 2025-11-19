/* 7zAlloc.c -- Allocation functions
2008-03-28
Igor Pavlov
Public domain */

#include <stdlib.h>
#include "7zAlloc.h"

/* #define _SZ_ALLOC_DEBUG */
/* use _SZ_ALLOC_DEBUG to debug alloc/free operations */

#ifdef _SZ_ALLOC_DEBUG

#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
int g_allocCount = 0;
int g_allocCountTemp = 0;

#endif

#include <proto/exec.h>

#ifndef SHOWHEADERINFO
#ifdef __amigaos4__
struct ExecIFace *IExec;
#else
//struct Library *SysBase;
#endif
#endif


void *SzAlloc(void *p, size_t size)
{
  p = p;
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "\nAlloc %10d bytes; count = %10d", size, g_allocCount);
  g_allocCount++;
  #endif

#ifdef __amigaos4__
IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
#elif defined(__AROS__)
// handled in link time
#else
 SysBase = *(struct ExecBase **)4;
#endif
  return AllocVec(size,MEMF_PRIVATE | MEMF_CLEAR);
}

void SzFree(void *p, void *address)
{
  p = p;
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
  {
    g_allocCount--;
    fprintf(stderr, "\nFree; count = %10d", g_allocCount);
  }
  #endif
#ifdef __amigaos4__
IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
#elif defined(__AROS__)
// handled in link time
#else
 SysBase = *(struct ExecBase **)4;
#endif
FreeVec(address);
}

void *SzAllocTemp(void *p, size_t size)
{
  p = p;
  if (size == 0)
    return 0;
  #ifdef _SZ_ALLOC_DEBUG
  fprintf(stderr, "\nAlloc_temp %10d bytes;  count = %10d", size, g_allocCountTemp);
  g_allocCountTemp++;
  #ifdef _WIN32
  return HeapAlloc(GetProcessHeap(), 0, size);
  #endif
  #endif
#ifdef __amigaos4__
IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
#elif defined(__AROS__)
// handled in link time
#else
 SysBase = *(struct ExecBase **)4;
#endif
  return AllocVec(size,MEMF_PRIVATE | MEMF_CLEAR);
}

void SzFreeTemp(void *p, void *address)
{
  p = p;
  #ifdef _SZ_ALLOC_DEBUG
  if (address != 0)
  {
    g_allocCountTemp--;
    fprintf(stderr, "\nFree_temp; count = %10d", g_allocCountTemp);
  }
  #ifdef _WIN32
  HeapFree(GetProcessHeap(), 0, address);
  return;
  #endif
  #endif
#ifdef __amigaos4__
IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;
#elif defined(__AROS__)
// handled in link time
#else
 SysBase = *(struct ExecBase **)4;
#endif
FreeVec(address);
}

void *BzAlloc(void *opaque,size_t items,size_t size)
{
	void *v = SzAlloc(0,items*size);
	return v;
}

void BzFree(void *opaque,void *addr)
{
	if (addr != NULL) SzFree(0,addr);
}
