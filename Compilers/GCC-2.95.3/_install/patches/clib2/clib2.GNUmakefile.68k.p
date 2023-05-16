--- GNUmakefile.68k	2016-11-18 16:22:21.000000000 +0000
+++ GNUmakefile.68k	2016-11-18 23:25:54.801998101 +0000
@@ -24,9 +24,9 @@ MAKE = $(MAKE_COMMAND) -f GNUmakefile.68
 
 # override certain things for non-native builds
 ifneq ($(HOST), AmigaOS)
-   CC = m68k-amigaos-gcc
-   AR = m68k-amigaos-ar -q
-   RANLIB = m68k-amigaos-ranlib
+   CC = m68k-amigaos-gcc
+   AR = m68k-amigaos-ar -q
+   RANLIB = m68k-amigaos-ranlib
    COPY = cp
    DELETE = rm -rf
    MAKEDIR = mkdir -p
@@ -533,6 +532,7 @@ UNIX_LIB = \
 	stdlib_realloc.o \
 	stdlib_resetmemstats.o \
 	stdlib_system.o \
+	stubs.o \
 	systeminfo_sysinfo.o \
 	termios_cfgetispeed.o \
 	termios_cfgetospeed.o \
@@ -982,10 +982,10 @@ all: \
 	lib/n32bcrt0.o \
 	lib/n32rcrt0.o \
 	lib/libm020/libm.a \
-	lib/libm.a \
-	lib/libb/libm.a \
-	lib/libb/libm020/libm.a \
-	lib/libb32/libm020/libm.a
+	lib/libm.a
+#	lib/libb/libm.a \
+#	lib/libb/libm020/libm.a \
+#	lib/libb32/libm020/libm.a
 
 ##############################################################################
 
