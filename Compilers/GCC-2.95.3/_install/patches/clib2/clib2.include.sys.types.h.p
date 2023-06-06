--- include/sys/types.h	2010-12-29 00:58:25.000000000 +0000
+++ include/sys/types.h	2010-12-29 01:01:33.000000000 +0000
@@ -59,6 +59,12 @@
 
 /****************************************************************************/
 
+#ifndef _SYS_SELECT_H
+#include <sys/select.h>
+#endif /* _SYS_SELECT_H */
+
+/****************************************************************************/
+
 #ifdef __cplusplus
 extern "C" {
 #endif /* __cplusplus */
@@ -65,6 +71,15 @@
 
 /****************************************************************************/
 
+/* BSD types */
+#ifndef __u_char_defined
+typedef unsigned char u_char;
+typedef unsigned short int u_short;
+typedef unsigned int u_int;
+typedef unsigned long u_long;
+#define __u_char_defined
+#endif
+
 typedef char * caddr_t;
 typedef unsigned int comp_t;
 typedef unsigned long dev_t;
