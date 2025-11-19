CC = gcc
CXX = g++
OSTYPE = $(shell uname -s)
DEBUG ?= 0

CFLAGS = -Wall

ifeq ($(DEBUG),1)
    CFLAGS += -g -ggdb -DDEBUG
else
    CFLAGS += -O2 -DNDEBUG
endif

LDFLAGS = -nostartfiles -lstdc++

ifeq ($(OSTYPE),Morphos)
	LDFLAGS += -noixemul
	CFLAGS += -noixemul
endif

OBJ = \
	extheader.o \
	xad_rar.o \
	Rar_Extractor-0.2.1/unrarlib/unrarlib.o \
	Rar_Extractor-0.2.1/unrar/librarextract.a

all: RAR

RAR: $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)

Rar_Extractor-0.2.1/unrar/librarextract.a:
	make -C Rar_Extractor-0.2.1/unrar

clean:
	rm -f $(OBJ)
