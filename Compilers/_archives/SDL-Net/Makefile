# Makefile for SDL_net 1.2.3 - AmigaOS m68k

#PREFIX= ../../GCC-6.50-Latest/bin
#CC      = $(PREFIX)/m68k-amigaos-gcc
#AR      = $(PREFIX)/m68k-amigaos-ar
#RANLIB  = $(PREFIX)/m68k-amigaos-ranlib

CFLAGS  = -mcpu=68040 -noixemul -O2 -mhard-float
#CFLAGS += -I. -I/opt/amiga/include/SDL -I/opt/amiga/include
CFLAGS += -DHAVE_CONFIG_H

ARFLAGS = rcs

TARGET  = libSDL_net.a

OBJS = \
	SDLnet.o \
	SDLnetTCP.o \
	SDLnetUDP.o \
	SDLnetselect.o \
	chatd.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)
	$(RANLIB) $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

install: $(TARGET)
	mkdir -p $(DESTDIR)/lib
	mkdir -p $(DESTDIR)/include/SDL
	cp $(TARGET) $(DESTDIR)/lib/
	cp SDL_net.h $(DESTDIR)/include/SDL/

.PHONY: all clean install