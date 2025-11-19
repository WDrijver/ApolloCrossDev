CC = gcc
AR = ar
DEBUG = 0
OSTYPE = $(shell uname -s)

CFLAGS = -Wall -Werror-implicit-function-declaration -Wno-strict-aliasing -Wno-pointer-sign

ifeq ($(OSTYPE), MorphOS)
CFLAGS += -noixemul
endif

ifeq ($(OSTYPE), AmigaOS)
CFLAGS += -D__USE_INLINE__ -Wno-deprecated-declarations
endif

ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG
else
CFLAGS += -O2
endif

OBJS = dllstartup.o dll.o dllimport.o
ARCHIVE = libdll.a

$(ARCHIVE) : $(OBJS)
	$(AR) rcs $@ $?

clean :
	rm -f $(ARCHIVE) $(OBJS)

#DEPENDENCIES

dllstartup.o: dllstartup.c dll.h dll_intern.h

dll.o: dll.c dll.h dll_intern.h

dllimport.o: dllimport.c dll.h dll_intern.h
