CC = gcc
OSTYPE = $(shell uname -s)
DEBUG ?= 0

CFLAGS = -Wall -D_7ZIP_PPMD_SUPPPORT 

ifeq ($(DEBUG),1)
    CFLAGS += -g -ggdb -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

LDFLAGS = -nostartfiles -lbz2

ifeq ($(OSTYPE),Morphos)
	LDFLAGS += -noixemul
	CFLAGS += -noixemul
endif
	
OBJ = \
	extheader.o \
	7z.o \
	7zAlloc.o \
	../7zBuf.o \
	../7zCrc.o \
	../7zCrcOpt.o \
	../7zDec.o \
	../7zIn.o \
	../7zStream.o \
	../LzmaDec.o \
	../Lzma2Dec.o \
	../Bra86.o \
	../Bcj2.o \
	../CpuArch.o \
	../Ppmd7.o \
	../Ppmd7Dec.o

7z: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(OBJ)
