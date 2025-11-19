#ifndef SZ_H
#define SZ_H

#ifndef MEMF_PRIVATE
#define MEMF_PRIVATE MEMF_ANY
#endif

#include "../7z.h"
#include "7-Zip_rev.h"
#include <exec/types.h>

#ifdef __amigaos4__
#define PACKED  __attribute__((__packed__))
#else
#define PACKED  
#endif

struct PPMdProps
{
  UBYTE maxorder;
  UInt32 suballocsize;
};

typedef struct _CFileInStream
{
  ISeekInStream InStream;
  struct xadArchiveInfo *ai;
  long posn;
#ifdef __amigaos4__
  struct xadMasterIFace *IxadMaster;
#else
	struct xadMasterBase *xadMasterBase;
#endif
} CFileXadInStream;

struct xad7zprivate {
	CFileXadInStream archiveStream;
	CLookToRead lookStream;
	CSzArEx db;
	ISzAlloc allocImp;
	ISzAlloc allocTempImp;
	UInt32 blockIndex;
	Byte *outBuffer;
	size_t outBufferSize;
};

#endif
