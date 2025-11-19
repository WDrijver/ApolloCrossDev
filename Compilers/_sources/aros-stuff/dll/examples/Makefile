CC = gcc
CXX = g++
AR = ar
DEBUG = 0
OSTYPE = $(shell uname -s)

EXEOBJS = test1.o test2.o
DLLOBJS = dll1.o dll2.o server.o

OBJS = $(EXEOBJS) $(DLLOBJS)

EXE = $(EXEOBJS:.o=.exe)

DLL = $(DLLOBJS:.o=.dll)


CFLAGS = -Wall -Wno-strict-aliasing -I..

ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

LDFLAGS = -L.. -ldll -lstdc++

ifeq ($(OSTYPE), MorphOS)
CFLAGS += -noixemul
LDFLAGS += -noixemul
ifeq ($(DEBUG),1)
LDFLAGS += -ldebug
endif
endif

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

%.o : %.cpp
	$(CXX) $(CFLAGS) -o $@ -c $<

all : $(EXE) $(DLL)

$(EXE) : %.exe : %.o
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

$(DLL) : %.dll : %.o
	$(CC) $(CFLAGS) $< $(LDFLAGS) -o $@

depend : 
	makedepend -f Makefile -i / $(OBJS)

clean :
	rm -f $(EXE) $(DLL) $(OBJS)
