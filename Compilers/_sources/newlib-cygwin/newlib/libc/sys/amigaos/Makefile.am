## Process this file with automake to generate Makefile.in

#AUTOMAKE_OPTIONS = cygnus

INCLUDES = $(NEWLIB_CFLAGS) $(CROSS_CFLAGS) $(TARGET_CFLAGS)

AM_CCASFLAGS = $(INCLUDES)

noinst_LIBRARIES = lib.a

lib_a_SOURCES = dummy.c open.c __divsi3.c  \
__adddf3.c  __divdf3.c  __eqdf2.c        __fixdfsi.c     __fixunssfsi.c  __floatunsidf.c  __mulsf3.c  __negsf2.c  __truncdfsf2.c \
__addsf3.c  __divsf3.c  __eqsf2.c        __fixsfsi.c     __floatsidf.c   __floatunsisf.c  __mulsi3.c  __subdf3.c  __udivsi3.c \
__cmpxf2.c  __divsi3.c  __extendsfdf2.c  __fixunsdfsi.c  __floatsisf.c   __muldf3.c       __negdf2.c  __subsf3.c \
isinf.c isnan.c \
cxxglue.c __dosbase.c __unorddf2.c __mathieeedoubbas.c \
abort.c basename.c dirname.c __nocommandline.c uname.c \
malloc.cpp clock.c

#syscalls.c trap.S creat.c
lib_a_CCASFLAGS = $(AM_CCASFLAGS)
lib_a_CFLAGS = $(AM_CFLAGS)

all-local: crt0.o

ACLOCAL_AMFLAGS = -I ../../.. -I ../../../..
CONFIG_STATUS_DEPENDENCIES = $(newlib_basedir)/configure.host
