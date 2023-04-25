? include/.unistd.h.swp
Index: include/unistd.h
===================================================================
RCS file: /cvsroot/clib2/library/include/unistd.h,v
retrieving revision 1.23
diff -u -r1.23 unistd.h
--- include/unistd.h	6 Jan 2007 10:09:49 -0000	1.23
+++ include/unistd.h	24 Dec 2010 05:27:43 -0000
@@ -133,6 +133,8 @@
 extern int execve(const char *path,char *const argv[],char *const envp[]);
 extern int execvp(const char *command,char * const argv[]);
 extern int profil(unsigned short *buffer, size_t bufSize, size_t offset, unsigned int scale);
+extern pid_t vfork(void);
+extern int pipe(int pipefd[2]);
 
 /****************************************************************************/
 
