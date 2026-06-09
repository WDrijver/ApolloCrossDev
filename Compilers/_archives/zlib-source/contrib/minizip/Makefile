# Makefile for classic zlib contrib/minizip
#
# Builds:
#   libminizip.a
#   miniunz   (optional test/extract tool)
#   minizip   (optional test/create tool)
#
# Usage:
#   make
#   make clean
#
# Override tools/flags like this:
#   make CC=ppc-amigaos-gcc AR=ppc-amigaos-ar RANLIB=ppc-amigaos-ranlib
#
# If your platform has no fopen64/fseeko64/ftello64, add:
#   make CPPFLAGS="-Dfopen64=fopen -Dftello64=ftell -Dfseeko64=fseek"

#PREFIX= ../../../ApolloCrossDev/Compilers/GCC-6.50-Stable/bin
#CC      = $(PREFIX)/m68k-amigaos-gcc
#AR      = $(PREFIX)/m68k-amigaos-ar
#RANLIB  = $(PREFIX)/m68k-amigaos-ranlib

RM      = rm -f

CFLAGS   = -O2 -Wall -m68040 -m68881 -noixemul 
CPPFLAGS = -Dfopen64=fopen -Dftello64=ftell -Dfseeko64=fseek
LDFLAGS  = -noixemul
LIBS     = -lz

TARGET_LIB      = libminizip.a
TARGET_MINIUNZ  = miniunz
TARGET_MINIZIP  = minizip

LIB_OBJS = \
	ioapi.o \
	unzip.o \
	zip.o

MINIUNZ_OBJS = \
	miniunz.o

MINIZIP_OBJS = \
	minizip.o

.PHONY: all clean lib tools

all: lib tools

lib: $(TARGET_LIB)

tools: $(TARGET_MINIUNZ) $(TARGET_MINIZIP)

$(TARGET_LIB): $(LIB_OBJS)
	$(AR) rcs $@ $(LIB_OBJS)
	$(RANLIB) $@

$(TARGET_MINIUNZ): $(MINIUNZ_OBJS) $(TARGET_LIB)
	$(CC) $(LDFLAGS) -o $@ $(MINIUNZ_OBJS) $(TARGET_LIB) $(LIBS)

$(TARGET_MINIZIP): $(MINIZIP_OBJS) $(TARGET_LIB)
	$(CC) $(LDFLAGS) -o $@ $(MINIZIP_OBJS) $(TARGET_LIB) $(LIBS)

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) *.o $(TARGET_LIB) $(TARGET_MINIUNZ) $(TARGET_MINIZIP)