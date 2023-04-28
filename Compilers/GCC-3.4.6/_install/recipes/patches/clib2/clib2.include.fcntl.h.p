? include/.fcntl.h.swp
Index: include/fcntl.h
===================================================================
RCS file: /cvsroot/clib2/library/include/fcntl.h,v
retrieving revision 1.9
diff -u -r1.9 fcntl.h
--- include/fcntl.h	8 Jan 2006 12:06:14 -0000	1.9
+++ include/fcntl.h	24 Dec 2010 05:32:32 -0000
@@ -87,6 +87,8 @@
 #define F_GETOWN	5
 #define F_SETOWN	6
 
+#define FD_CLOEXEC	1
+
 /****************************************************************************/
 
 /*
