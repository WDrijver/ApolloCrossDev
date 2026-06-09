# Makefile for ApolloCrossDev GCC 6.50 Latest
# Willem Drijver

#PREFIX= ../../GCC-6.50-Stable/bin
#CC      = $(PREFIX)/m68k-amigaos-gcc
#AR      = $(PREFIX)/m68k-amigaos-ar
#RANLIB  = $(PREFIX)/m68k-amigaos-ranlib

RM      = rm -f

CFLAGS   = -O2 -Wall -m68040 -m68881 -noixemul 
CPPFLAGS = -Dfopen64=fopen -Dftello64=ftell -Dfseeko64=fseek
LDFLAGS  =
LIBS     = -lz

OBJS = adler32.o compress.o crc32.o gzclose.o gzlib.o gzread.o gzwrite.o uncompr.o deflate.o trees.o zutil.o inflate.o infback.o inftrees.o inffast.o

libz.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)
	$(RANLIB) $@

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) libz.a