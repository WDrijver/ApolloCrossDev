--- include/netinet/in.h.old	2010-12-29 15:29:10.000000000 +0000
+++ include/netinet/in.h	2010-12-29 15:30:23.000000000 +0000
@@ -152,6 +152,7 @@
 #define	INADDR_MAX_LOCAL_GROUP	0xe00000ffUL	/* 224.0.0.255 */
 
 #define	IN_LOOPBACKNET			127				/* official! */
+#define INADDR_LOOPBACK			0x7f000000UL	/* 127.0.0.1 */
 
 /*
  * Socket address, internet style.
