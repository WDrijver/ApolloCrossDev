Index: signal_headers.h
===================================================================
RCS file: /cvsroot/clib2/library/signal_headers.h,v
retrieving revision 1.8
diff -u -r1.8 signal_headers.h
--- signal_headers.h	8 Jan 2006 12:04:24 -0000	1.8
+++ signal_headers.h	29 Dec 2010 20:06:22 -0000
@@ -74,7 +74,7 @@
 
 /****************************************************************************/
 
-#define NUM_SIGNALS (SIGTERM - SIGABRT + 1)
+#define NUM_SIGNALS (SIGPIPE - SIGABRT + 1)
 
 /****************************************************************************/
 
Index: signal_raise.c
===================================================================
RCS file: /cvsroot/clib2/library/signal_raise.c,v
retrieving revision 1.10
diff -u -r1.10 signal_raise.c
--- signal_raise.c	8 Jan 2006 12:04:24 -0000	1.10
+++ signal_raise.c	29 Dec 2010 20:06:22 -0000
@@ -45,7 +45,8 @@
 	SIG_DFL,	/* SIGILL */
 	SIG_DFL,	/* SIGINT */
 	SIG_DFL,	/* SIGSEGV */
-	SIG_DFL		/* SIGTERM */
+	SIG_DFL,	/* SIGTERM */
+	SIG_DFL		/* SIGPIPE */
 };
 
 /****************************************************************************/
@@ -68,10 +69,10 @@
 
 	SHOWVALUE(sig);
 
-	assert( SIGABRT <= sig && sig <= SIGTERM );
+	assert( SIGABRT <= sig && sig <= SIGPIPE );
 
 	/* This has to be a well-known and supported signal. */
-	if(sig < SIGABRT || sig > SIGTERM)
+	if(sig < SIGABRT || sig > SIGPIPE)
 	{
 		SHOWMSG("unknown signal number");
 
Index: signal_sigmask.c
===================================================================
RCS file: /cvsroot/clib2/library/signal_sigmask.c,v
retrieving revision 1.5
diff -u -r1.5 signal_sigmask.c
--- signal_sigmask.c	8 Jan 2006 12:04:24 -0000	1.5
+++ signal_sigmask.c	29 Dec 2010 20:06:22 -0000
@@ -52,7 +52,7 @@
 
 	assert( 0 <= sig && sig <= 31 );
 
-	if(SIGABRT <= sig && sig <= SIGTERM)
+	if(SIGABRT <= sig && sig <= SIGPIPE)
 		result = (1 << sig);
 	else
 		result = 0;
Index: signal_signal.c
===================================================================
RCS file: /cvsroot/clib2/library/signal_signal.c,v
retrieving revision 1.4
diff -u -r1.4 signal_signal.c
--- signal_signal.c	8 Jan 2006 12:04:24 -0000	1.4
+++ signal_signal.c	29 Dec 2010 20:06:22 -0000
@@ -47,7 +47,7 @@
 	SHOWVALUE(sig);
 	SHOWPOINTER(handler);
 
-	if(sig < SIGABRT || sig > SIGTERM || handler == SIG_ERR)
+	if(sig < SIGABRT || sig > SIGPIPE || handler == SIG_ERR)
 	{
 		SHOWMSG("unsupported signal");
 
Index: include/signal.h
===================================================================
RCS file: /cvsroot/clib2/library/include/signal.h,v
retrieving revision 1.8
diff -u -r1.8 signal.h
--- include/signal.h	8 Jan 2006 12:06:14 -0000	1.8
+++ include/signal.h	29 Dec 2010 20:06:22 -0000
@@ -61,6 +61,7 @@
 #define SIGINT	4
 #define SIGSEGV	5
 #define SIGTERM	6
+#define SIGPIPE 7
 
 /****************************************************************************/
 
