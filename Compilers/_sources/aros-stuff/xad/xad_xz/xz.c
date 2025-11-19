/* XZ client for xadmaster.library
 * chris@unsatisfactorysoftware.co.uk
 */

#ifndef XADMASTER_XZ_C
#define XADMASTER_XZ_C

#include <proto/exec.h>

#ifdef __amigaos4__
struct ExecIFace *IExec;
struct Library *newlibbase;
struct Interface *INewlib;

#include "sys/types.h"
#elif defined(__AROS__)
struct Library *aroscbase = NULL;
#else
#include <exec/types.h>
#endif

#include <proto/xadmaster.h>
#ifdef __AROS__
#include <lzma.h>
#else
#include <proto/lzma.h>
#endif
#include <dos/dos.h>
#ifdef __AROS__
#include <SDI/SDI_compiler.h>
#undef ASM
#define ASM(x) x
#else
#include "SDI_compiler.h"
#endif
#include "xz.h"
#include "XZ_rev.h"

#ifndef XADMASTERFILE
#define sz_Client		FirstClient
#define NEXTCLIENT		0
#ifdef __amigaos4__
#define XADMASTERVERSION	13
#else
#define XADMASTERVERSION	12
#endif
UBYTE *version = VERSTAG;
#endif
#define SZ_VERSION	VERSION
#define SZ_REVISION	REVISION

#ifdef __AROS__
void close_lzma(void)
{
	CloseLibrary(aroscbase);
	aroscbase = NULL;
}

BOOL open_lzma(void) 
{
	if(!aroscbase && !(aroscbase = OpenLibrary("arosc.library", 41)))
		return FALSE;

	return TRUE;
}
#else
struct Library *lzmaBase;
#ifdef __amigaos4__
struct lzmaIFace *Ilzma;
struct ExecIFace *IExec;
#endif

void close_lzma(void)
{
#ifdef __amigaos4__
	if(Ilzma)
	{
		DropInterface((struct Interface *)Ilzma);
		Ilzma=NULL;
	}
#endif
	if(lzmaBase)
	{
		CloseLibrary(lzmaBase);
		lzmaBase=NULL;
	}

#ifdef __amigaos4__
/*
    DropInterface(INewlib);
    CloseLibrary(newlibbase);
	INewlib = NULL;
	newlibbase = NULL;
*/
#endif
}

BOOL open_lzma(void)
{
#ifdef __amigaos4__
    IExec = (struct ExecIFace *)(*(struct ExecBase **)4)->MainInterface;

    newlibbase = OpenLibrary("newlib.library", 52);
    if(newlibbase)
        INewlib = GetInterface(newlibbase, "main", 1, NULL);
#endif

	if(lzmaBase = OpenLibrary("lzma.library",5))
	{
#ifdef __amigaos4__
		if(Ilzma = (struct lzmaIFace *)GetInterface(lzmaBase,"main",1,NULL))
		{
#endif
			return TRUE;
#ifdef __amigaos4__
		}
#endif
	}
	close_lzma();
	return FALSE;
}
#endif

#ifdef __amigaos4__
BOOL sz_RecogData(ULONG size, STRPTR data,
struct xadMasterIFace *IxadMaster)
#else
ASM(BOOL) sz_RecogData(REG(d0, ULONG size), REG(a0, STRPTR data),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  if(data[0]==0xfd & data[1]=='7' & data[2]=='z' & data[3]=='X' & data[4]=='Z' & data[5]==0x00)
    return 1; /* known file */
  else
    return 0; /* unknown file */
}

#ifdef __amigaos4__
LONG sz_GetInfo(struct xadArchiveInfo *ai,
struct xadMasterIFace *IxadMaster)
#else
ASM(LONG) sz_GetInfo(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  struct xadFileInfo *fi;
	long err=XADERR_OK;
	if (!open_lzma()) return XADERR_RESOURCE;

	ai->xai_PrivateClient = xadAllocVec(sizeof(struct xad7zprivate),MEMF_PRIVATE | MEMF_CLEAR);
	struct xad7zprivate *xad7z = ai->xai_PrivateClient;

		    fi = (struct xadFileInfo *) xadAllocObjectA(XADOBJ_FILEINFO, NULL);
    		if (!fi) return(XADERR_NOMEMORY);

		fi->xfi_DataPos = 0;
	//	fi->xfi_Size = ; //TargetBufSaveLen;
		fi->xfi_FileName = xadGetDefaultName(XAD_ARCHIVEINFO, ai,
							XAD_EXTENSION, ".xz", TAG_DONE);

      fi->xfi_CrunchSize  = ai->xai_InSize;

	fi->xfi_Flags = XADFIF_NODATE | XADFIF_NOUNCRUNCHSIZE | XADFIF_NOFILENAME;

		 if ((err = xadAddFileEntryA(fi, ai, NULL))) return(XADERR_NOMEMORY);

return(err);
}

#ifdef __amigaos4__
LONG sz_UnArchive(struct xadArchiveInfo *ai,
struct xadMasterIFace *IxadMaster)
#else
ASM(LONG) sz_UnArchive(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
	struct xad7zprivate *xad7z = ai->xai_PrivateClient;
  struct xadFileInfo *fi = ai->xai_CurFile;
	UBYTE *inbuffer, *outbuffer;
	long err=XADERR_OK;
	lzma_ret ret;
	lzma_stream strm = LZMA_STREAM_INIT;

	if (!open_lzma()) return XADERR_RESOURCE;

	inbuffer = 	xadAllocVec(ai->xai_InSize, MEMF_CLEAR);
	outbuffer = xadAllocVec(1024, MEMF_CLEAR);

	xadHookAccess(XADAC_READ, ai->xai_InSize, inbuffer, ai);
	// need to get actual bytes read

	ret = lzma_stream_decoder(&strm, UINT64_MAX, 0);
	if(ret != LZMA_OK) return XADERR_UNKNOWN;


	strm.next_in = inbuffer;
	strm.avail_in = ai->xai_InSize;

	do
	{
		strm.next_out = outbuffer;
		strm.avail_out = 1024;

		ret = lzma_code(&strm, LZMA_RUN);

		if((ret != LZMA_OK) && (ret != LZMA_STREAM_END))
			err = XADERR_DECRUNCH;

		if((err == XADERR_OK) && (strm.avail_out != 1024))
			err = xadHookAccess(XADAC_WRITE, 1024 - strm.avail_out, outbuffer, ai);

		if(err) strm.avail_out = 1024;

	} while (strm.avail_out == 0);

	lzma_end(&strm);

	xadFreeObjectA(inbuffer, NULL);
	xadFreeObjectA(outbuffer, NULL);

	return err;
}

#ifdef __amigaos4__
void sz_Free(struct xadArchiveInfo *ai,
struct xadMasterIFace *IxadMaster)
#else
ASM(void) sz_Free(REG(a0, struct xadArchiveInfo *ai),
REG(a6, struct xadMasterBase *xadMasterBase))
#endif
{
  /* This function needs to free all the stuff allocated in info or
  unarchive function. It may be called multiple times, so clear freed
  entries!
  */

//	struct xad7zprivate *xad7z = ai->xai_PrivateClient;
	xadFreeObjectA(ai->xai_PrivateClient, NULL);
	ai->xai_PrivateClient = NULL;
	close_lzma();
}

/* You need to complete following structure! */
const struct xadClient sz_Client = {
NEXTCLIENT, XADCLIENT_VERSION, XADMASTERVERSION, SZ_VERSION, SZ_REVISION,
6, XADCF_FILEARCHIVER|XADCF_DATACRUNCHER|XADCF_FREEFILEINFO|XADCF_FREEXADSTRINGS,
0 /* Type identifier. Normally should be zero */, "XZ",
(BOOL (*)()) sz_RecogData, (LONG (*)()) sz_GetInfo,
(LONG (*)()) sz_UnArchive, (void (*)()) sz_Free };

#if !defined(__amigaos4__) && !defined(__AROS__)
void main(void)
{
}
#endif
#endif /* XADMASTER_7Z_C */
