--- ld/Makefile.in	2014-12-19 12:37:47.280015870 +0000
+++ ld/Makefile.in	2014-12-19 12:44:38.192014671 +0000
@@ -635,7 +635,7 @@ LEXCOMPILE = $(LEX) $(LFLAGS) $(AM_LFLAG
 LTLEXCOMPILE = $(LIBTOOL) --mode=compile $(LEX) $(LFLAGS) $(AM_LFLAGS)
 YACCCOMPILE = $(YACC) $(YFLAGS) $(AM_YFLAGS)
 LTYACCCOMPILE = $(LIBTOOL) --mode=compile $(YACC) $(YFLAGS) $(AM_YFLAGS)
-YLWRAP = $(top_srcdir)/ylwrap
+YLWRAP = $(top_srcdir)/../ylwrap
 DIST_SOURCES = $(ld_new_SOURCES) $(EXTRA_ld_new_SOURCES)
 TEXINFO_TEX = $(top_srcdir)/../texinfo/texinfo.tex
 am__TEXINFO_TEX_DIR = $(top_srcdir)/../texinfo
