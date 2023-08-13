--- include/sys/resource.h.old	2010-12-29 03:18:01.000000000 +0000
+++ include/sys/resource.h	2010-12-29 03:20:10.000000000 +0000
@@ -47,6 +47,10 @@
 #include <sys/types.h>	/* For the definition of rlim_t */
 #endif /* _SYS_TYPES_H */
 
+#ifndef _SYS_TIME_H
+#include <sys/time.h>
+#endif
+
 /****************************************************************************/
 
 #ifdef __cplusplus
@@ -90,6 +94,23 @@
 
 /****************************************************************************/
 
+#define RUSAGE_SELF	0
+#define RUSAGE_CHILDREN	1
+
+/****************************************************************************/
+
+struct rusage
+{
+	struct timeval ru_utime;
+	struct timeval ru_stime;	
+};
+
+/****************************************************************************/
+
+extern int getrusage(int who, struct rusage *usage);
+
+/****************************************************************************/
+
 #ifdef __cplusplus
 }
 #endif /* __cplusplus */
