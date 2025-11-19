/* xadClientVer (c) 2011 Chris Young <chris@unsatisfactorysoftware.co.uk> */

#include <proto/xadmaster.h>
#include <proto/exec.h>
#include <proto/dos.h>
#ifdef __AROS__
#include <string.h>
#include <stdio.h>
struct xadMasterBase *xadMasterBase;
#endif

int main(int argc, char **argv)
{
	LONG rarray[] = {0,0};
	struct RDArgs *args;
	int ret=0;
	char *client = NULL;
	BOOL rev = FALSE;

	enum
	{
		A_CLIENT,
		A_REV
	};

	STRPTR template = "CLIENT/A,REV=REVISION/S";

	args = ReadArgs(template,rarray,NULL);

	if(args)
	{

		if(rarray[A_CLIENT])
			client = strdup(rarray[A_CLIENT]);

		if(rarray[A_REV])
			rev = TRUE;

		FreeArgs(args);
	}
	else
	{
		printf("xadClientVer\nhttp://www.unsatisfactorysoftware.co.uk\n\n");
		return 0;
	}


    if((xadMasterBase = (struct xadMasterBase *) OpenLibrary("xadmaster.library", 13)))
    {
#ifdef __amigaos4__
		IxadMaster = (struct xadMasterIFace *)GetInterface(xadMasterBase,"main",1,NULL);
#endif

		struct xadClient *xc;

		if((xc = xadGetClientInfo()))
		{
      		while(xc && (ret==0))
      		{printf("'%s'\t%d\t%d\n",xc->xc_ArchiverName,xc->xc_ClientVersion,xc->xc_ClientRevision);
#ifdef __amigaos4__
				if(!strncmp(client,xc->xc_ArchiverName))
#else
				if(!strcmp(client,xc->xc_ArchiverName))
#endif
				{
					printf("%ld.%ld\n",xc->xc_ClientVersion,xc->xc_ClientRevision);

					if(rev)	ret = xc->xc_ClientRevision;
						else ret = xc->xc_ClientVersion;
				}
				xc = xc->xc_Next;
			}
		}

#ifdef __amigaos4__
		if(IxadMaster) DropInterface((struct Interface *)IxadMaster);
#endif
		CloseLibrary((struct Library *) xadMasterBase);
    }
    else
      printf("Could not open xadmaster.library\n");

  return ret;
}
