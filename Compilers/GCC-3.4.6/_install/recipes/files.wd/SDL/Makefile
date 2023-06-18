# Makefile for gcc version of SDL
#
#  recent changes:
#  12-Jun-2023:		Makefile updated for use with ApolloCrossDev
#
#
#  legacy changes:
#  18-Apr: _ApolloKeyRGB565toRGB565: disabled AMMX version of ColorKeying (for now, storem is not working in Gold2)
#  17-Nov: - fixed Shadow Surfaces (hopefully), they were effectively impossible
#            in the code base I got
#          - fixed ARGB32 (CGX code was assuming RGBA all the time)
#  12-Feb: - deleted redundant includes, now only SDL/ directory remains (as it should)

PREFIX := $(PWD)/../../ApolloCrossDev
TARGET := m68k-unknown-amigaos

CC := $(PREFIX)/bin/$(TARGET)-gcc -w
AS := $(PREFIX)/bin/$(TARGET)-as
AR := $(PREFIX)/bin/$(TARGET)-ar
LD := $(PREFIX)/bin/$(TARGET)-ld
RL := $(PREFIX)/bin/$(TARGET)-ranlib
VASM := ../../../VBCC/ApolloCrossDev/bin/vasmm68k_mot

LIBNIX_INC=$(PREFIX)/$(TARGET)/libnix/include
LIBSTD_INC=$(PREFIX)/$(TARGET)/include/libstdc++
APOLLO_INC=$(PREFIX)/$(TARGET)/include/apollo
LIBSDL_INC=./include/SDL

LIBNIX_LIB=$(PWD)/../../Compilers/GCC-3.4.6/ApolloCrossDev/m68k-unknown-amigaos/libnix/lib

DEFINES= DEFINE=ENABLE_CYBERGRAPHICS DEFINE=inline=__inline  DEFINE=NO_SIGNAL_H DEFINE=HAVE_STDIO_H DEFINE=ENABLE_AHI
# DEFINE=HAVE_OPENGL

GCCFLAGS = -I./include -I$(LIBSDL_INC) -I$(LIBNIX_INC) -I$(LIBSTD_INC) -I$(APOLLO_INC) \
		-O2 -noixemul -fomit-frame-pointer -m68020-60 -ffast-math \
		-DNOIXEMUL -D_HAVE_STDINT_H
GLFLAGS = -DSHARED_LIB -lamiga
GCCFLAGS += -DNO_AMIGADEBUG 
#-DAPOLLOARGBto565ALPHABLIT  1.5 fps slower in Sonic ??
#-DAPOLLORLEALPHABLIT 0.4 fps slower in Sonic ??
GLFLAGS  += -DNO_AMIGADEBUG

GCCDEFINES = -DENABLE_CYBERGRAPHICS -DNO_SIGNAL_H -D__MEM_AMIGA -DENABLE_AHI -DNO_AMIGADEBUG 
#-DNO_INLINE_STDARG

GOBJS = audio/SDL_audio.go audio/SDL_audiocvt.go audio/SDL_mixer.go audio/SDL_mixer_m68k.go audio/SDL_wave.go audio/amigaos/SDL_ahiaudio.go \
	SDL_error.go SDL_fatal.go video/SDL_RLEaccel.go video/SDL_blit.go video/SDL_blit_0.go \
	video/SDL_blit_1.go video/SDL_blit_A.go video/SDL_blit_N.go \
	video/SDL_bmp.go video/SDL_cursor.go video/SDL_pixels.go video/SDL_surface.go video/SDL_stretch.go \
	video/SDL_yuv.go video/SDL_yuv_sw.go video/SDL_video.go \
	timer/amigaos/SDL_systimer.go timer/SDL_timer.go joystick/SDL_joystick.go \
	joystick/SDL_sysjoystick.go SDL_cdrom.go SDL_syscdrom.go events/SDL_quit.go events/SDL_active.go \
	events/SDL_keyboard.go events/SDL_mouse.go events/SDL_resize.go file/SDL_rwops.go SDL.go \
	events/SDL_events.go thread/amigaos/SDL_sysmutex.go thread/amigaos/SDL_syssem.go thread/amigaos/SDL_systhread.go thread/amigaos/SDL_thread.go \
	thread/amigaos/SDL_syscond.go video/amigaos/SDL_cgxvideo.go video/amigaos/SDL_cgxmodes.go video/amigaos/SDL_cgximage.go video/amigaos/SDL_amigaevents.go \
	video/amigaos/SDL_amigamouse.go video/amigaos/SDL_cgxgl.go video/amigaos/SDL_cgxwm.go \
	video/amigaos/SDL_cgxyuv.go video/amigaos/SDL_cgxaccel.go video/amigaos/SDL_cgxgl_wrapper.go \
	video/SDL_gamma.go SDL_lutstub.ll stdlib/SDL_stdlib.go stdlib/SDL_string.go stdlib/SDL_malloc.go stdlib/SDL_getenv.go \
	video/apollo/triplebuffer.go 
	
AOBJS = video/apollo/apolloammxenable.ao video/apollo/blitapollo.ao video/apollo/colorkeyapollo.ao

#video/apollo/blitapollo.ao video/apollo/apolloammxenable.ao video/apollo/colorkeyapollo.ao \
	video/apollo/RLEAlphaBlit565.ao video/apollo/ApolloARGBto565AlphaBlit.ao

#
# BEGIN APOLLO ASM SUPPORT
# ( build vasm: make CPU=m68k SYNTAX=mot )
#
VFLAGS = -devpac -I$(PREFIX)/$(TARGET)/include -Fhunk -m68080
GCCFLAGS += -DAPOLLO_BLIT -I./video/apollo
#GCCFLAGS += -DAPOLLO_BLITDBG
#GOBJS +=	video/apollo/blitapollo.ao \
#		video/apollo/apolloammxenable.ao \
#		video/apollo/colorkeyapollo.ao #\
#		video/apollo/RLEAlphaBlit565.ao \
#		video/apollo/ApolloARGBto565AlphaBlit.ao

%.ao: %.asm
	$(VASM) $(VFLAGS) -o $@ $*.asm
#
# END APOLLO ASM SUPPORT
#

%.go: %.c
	$(CC) $(GCCFLAGS) $(GCCDEFINES) -o $@ -c $*.c

%.ll: %.s
	$(AS) -m68040 -o $@ $*.s

all: libSDL.a libSDL_Apollo.a

libSDL.a: $(GOBJS)
	-rm -f libSDL.a
	$(AR) cru libSDL.a $(GOBJS)
	$(RL) libSDL.a

libSDL_Apollo.a: $(AOBJS)
	-rm -f libSDL_Apollo.a
	$(AR) cru libSDL_Apollo.a $(AOBJS)
	$(RL) libSDL_Apollo.a

clean:
	-rm -f $(GOBJS) $(AOBJS) libSDL.a libSDL_Apollo.a
