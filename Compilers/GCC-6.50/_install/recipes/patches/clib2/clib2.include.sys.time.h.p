Index: include/sys/time.h
===================================================================
RCS file: /cvsroot/clib2/library/include/sys/time.h,v
retrieving revision 1.11
diff -u -r1.11 time.h
--- include/sys/time.h	8 Jan 2006 12:06:14 -0000	1.11
+++ include/sys/time.h	29 Dec 2010 21:08:36 -0000
@@ -142,6 +142,11 @@
 
 /****************************************************************************/
 
+#define timerisset(tvp) ((tvp)->tv_sec != 0 || (tvp)->tv_usec != 0)
+#define timerclear(tvp) ((tvp)->tv_sec = (tvp)->tv_usec = 0)
+
+/****************************************************************************/
+
 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
