#include <stdlib.h>
#include <exec/memory.h>
#include <workbench/startup.h>
#include <proto/exec.h>
#include <dos/dos.h>
#include <proto/dos.h>
#include "stabs.h"

extern int __argc; /* Defined in startup */
extern char **__argv;
extern char *__commandline;
extern unsigned long __commandlen;
extern struct WBStartup *_WBenchMsg;

static char *cline = NULL; /* Copy of commandline */
static BPTR cd = 0l; /* Lock for Current Directory */

/* This guarantees that this module gets linked in.
 If you replace this by an own reference called
 __nocommandline you get no commandline arguments */
// ALIAS(__nocommandline, __initcommandline);

void __nocommandline(void) {
	struct WBStartup *wbs = _WBenchMsg;

	if (wbs != NULL) {
		if (wbs->sm_ArgList != NULL) /* cd to icon */
			cd = CurrentDir(DupLock(wbs->sm_ArgList->wa_Lock));
	} else {
		char **av, *a, *cl = __commandline;
		size_t i = __commandlen;
		int ac;

		if (!(cline = (char *) malloc(i + 1))) /* get buffer */
			exit(RETURN_FAIL);

		for (a = cline, ac = 1;;) /* and parse commandline */
		{
			while (i && (*cl == ' ' || *cl == '\t' || *cl == '\n')) {
				cl++;
				i--;
			}
			if (!i)
				break;
			if (*cl == '\"') {
				cl++;
				i--;
				while (i) {
					if (*cl == '\"') {
						cl++;
						i--;
						break;
					}
					if (*cl == '*') {
						cl++;
						i--;
						if (!i)
							break;
					}
					*a++ = *cl++;
					i--;
				}
			} else
				while (i && (*cl != ' ' && *cl != '\t' && *cl != '\n')) {
					*a++ = *cl++;
					i--;
				}
			*a++ = '\0';
			ac++;
		}
		/* NULL Terminated */
		if (!(__argv = av = (char **) calloc(((__argc = ac) + 1), sizeof(char *))))
			exit(RETURN_FAIL);

		for (a = cline, i = 1; i < ac; i++) {
			av[i] = a;
			while (*a++)
				;
		}

		for (i = 256;; i += 256) /* try in steps of 256 bytes */
		{
			if (!(*av = (char *) malloc(i)))
				break;
			GetProgramName(*av, i); /* Who am I ? */
			if (IoErr() != ERROR_LINE_TOO_LONG)
				break;
			free(*av);
		}

		if (*av == NULL)
			exit(RETURN_FAIL);
	}
}

void __exitcommandline(void) {
	struct WBStartup *wbs = _WBenchMsg;

	if (wbs != NULL) {
		if (wbs->sm_ArgList != NULL) /* set original lock */
			UnLock(CurrentDir(cd));
	}
}

/* Add these two functions to the lists */
ADD2INIT(__nocommandline, -40);
ADD2EXIT(__exitcommandline, -40);
