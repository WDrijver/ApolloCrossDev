--- binutils/Makefile.in	2014-12-19 12:37:51.912015855 +0000
+++ binutils/Makefile.in	2014-12-19 12:45:15.116014557 +0000
@@ -481,7 +481,7 @@ LINK = $(LIBTOOL) --mode=link $(CCLD) $(
 	$(AM_LDFLAGS) $(LDFLAGS) -o $@
 LEXCOMPILE = $(LEX) $(LFLAGS) $(AM_LFLAGS)
 LTLEXCOMPILE = $(LIBTOOL) --mode=compile $(LEX) $(LFLAGS) $(AM_LFLAGS)
-YLWRAP = $(top_srcdir)/ylwrap
+YLWRAP = $(top_srcdir)/../ylwrap
 YACCCOMPILE = $(YACC) $(YFLAGS) $(AM_YFLAGS)
 LTYACCCOMPILE = $(LIBTOOL) --mode=compile $(YACC) $(YFLAGS) $(AM_YFLAGS)
 DIST_SOURCES = $(addr2line_SOURCES) $(ar_SOURCES) $(coffdump_SOURCES) \
