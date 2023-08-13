# Makefile for gcc version of SDL
#
#  recent changes:
#  25-Jun-2023:		Makefile updated for use with ApolloCrossDev
#					- enabled dummy cdrom (so SDL_ttf will compile)
#					- separated SDL.a and SDL_Apollo.a (combined gives corrupted lib)
#
#  legacy changes:
#  18-Apr: _ApolloKeyRGB565toRGB565: disabled AMMX version of ColorKeying (for now, storem is not working in Gold2)
#  17-Nov: - fixed Shadow Surfaces (hopefully), they were effectively impossible
#            in the code base I got
#          - fixed ARGB32 (CGX code was assuming RGBA all the time)
#  12-Feb: - deleted redundant includes, now only SDL/ directory remains (as it should)

PREFIX 		:= $(PWD)/../../ApolloCrossDev
TARGET 		:= m68k-unknown-amigaos
CC 			:= $(PREFIX)/bin/$(TARGET)-gcc -w
AS 			:= $(PREFIX)/bin/$(TARGET)-as
AR 			:= $(PREFIX)/bin/$(TARGET)-ar
LD 			:= $(PREFIX)/bin/$(TARGET)-ld
RL 			:= $(PREFIX)/bin/$(TARGET)-ranlib
VASM 		:= ../../../VBCC/ApolloCrossDev/bin/vasmm68k_mot
AMIGADATE	:= $(shell date +"%-d.%-m.%Y")
CPU			:= m68030

LIBNIX_INC	= $(PREFIX)/$(TARGET)/libnix/include
LIBSTD_INC	= $(PREFIX)/$(TARGET)/include/libstdc++
APOLLO_INC	= $(PREFIX)/$(TARGET)/include/apollo
LIBNIX_LIB	= $(PWD)/../../Compilers/GCC-3.4.6/ApolloCrossDev/m68k-unknown-amigaos/libnix/lib

GCCFLAGS 	= -I$(LIBNIX_INC) -I. -Iinclude -I$(LIBSTD_INC) -I$(APOLLO_INC) \
		-Isrc/thread -Isrc/video -Isrc/main/amigaos -Isrc/timer -Isrc/events -Isrc/joystick -Iamiga/makefile-support \
		-O2 -fomit-frame-pointer -$(CPU) -ffast-math -noixemul \
		-DNOIXEMUL -D_HAVE_STDINT_H \
		-D__AMIGADATE__=\"$(AMIGADATE)\"
GLFLAGS 	= -DSHARED_LIB -lamiga
GCCFLAGS 	+= -DNO_AMIGADEBUG 
GLFLAGS  	+= -DNO_AMIGADEBUG

GOBJS 		= src/audio/SDL_audio.go src/audio/SDL_audiocvt.go src/audio/SDL_mixer.go src/audio/SDL_mixer_m68k.go src/audio/SDL_wave.go src/audio/amigaos/SDL_ahiaudio.go \
	src/SDL_error.go src/SDL_fatal.go src/video/SDL_RLEaccel.go src/video/SDL_blit.go src/video/SDL_blit_0.go \
	src/video/SDL_blit_1.go src/video/SDL_blit_A.go src/video/SDL_blit_N.go \
	src/video/SDL_bmp.go src/video/SDL_cursor.go src/video/SDL_pixels.go src/video/SDL_surface.go src/video/SDL_stretch.go \
	src/video/SDL_yuv.go src/video/SDL_yuv_sw.go src/video/SDL_video.go \
	src/timer/amigaos/SDL_systimer.go src/timer/SDL_timer.go src/joystick/SDL_joystick.go \
	src/joystick/amigaos/SDL_sysjoystick.go src/events/SDL_quit.go src/events/SDL_active.go \
	src/cpuinfo/SDL_cpuinfo.go src/events/SDL_keyboard.go src/events/SDL_mouse.go src/events/SDL_resize.go src/file/SDL_rwops.go src/SDL.go \
	src/events/SDL_events.go src/thread/amigaos/SDL_sysmutex.go src/thread/amigaos/SDL_syssem.go src/thread/amigaos/SDL_systhread.go src/thread/amigaos/SDL_thread.go \
	src/thread/amigaos/SDL_syscond.go src/video/cybergfx/SDL_cgxvideo.go src/video/cybergfx/SDL_cgxmodes.go src/video/cybergfx/SDL_cgximage.go src/video/cybergfx/SDL_amigaevents.go \
	src/video/cybergfx/SDL_amigamouse.go src/video/cybergfx/SDL_cgxgl.go src/video/cybergfx/SDL_cgxwm.go \
	src/video/cybergfx/SDL_cgxyuv.go src/video/cybergfx/SDL_cgxaccel.go src/video/cybergfx/SDL_cgxgl_wrapper.go \
	src/video/SDL_gamma.go src/main/amigaos/SDL_lutstub.ll src/stdlib/SDL_stdlib.go src/stdlib/SDL_string.go src/stdlib/SDL_malloc.go src/stdlib/SDL_getenv.go \
	src/cdrom/dummy/SDL_syscdrom.go 
# Disabled modules
# src/cdrom/SDL_cdrom.go src/cdrom/amigaos/SDL_syscdrom.go

#
# BEGIN APOLLO ASM SUPPORT
# ( build vasm: make CPU=m68k SYNTAX=mot )
#
VFLAGS 		= -devpac -I$(PREFIX)/$(TARGET)/include -Fhunk -m68080
GCCFLAGS 	+= -DAPOLLO_BLIT -Isrc/video/apollo
AOBJS 		= src/video/apollo/blitapollo.ao \
			src/video/apollo/apolloammxenable.ao \
			src/video/apollo/colorkeyapollo.ao \

%.ao: %.asm
	$(VASM) $(VFLAGS) -o $@ $*.asm
#
# END APOLLO ASM SUPPORT
#

%.go: %.c
	$(CC) $(GCCFLAGS) $(GCCDEFINES) -o $@ -c $*.c

%.ll: %.s
	$(AS) -$(CPU) -o $@ $*.s

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

# DEFINES= DEFINE=ENABLE_CYBERGRAPHICS DEFINE=inline=__inline  DEFINE=NO_SIGNAL_H DEFINE=HAVE_STDIO_H DEFINE=ENABLE_AHI
# DEFINE=HAVE_OPENGL
#-DAPOLLOARGBto565ALPHABLIT  1.5 fps slower in Sonic ??
#-DAPOLLORLEALPHABLIT 0.4 fps slower in Sonic ??
# GCCDEFINES = -DENABLE_CYBERGRAPHICS -DNO_SIGNAL_H -D__MEM_AMIGA -DENABLE_AHI -DNO_AMIGADEBUG 
#-DNO_INLINE_STDARG	
# AOBJS = video/apollo/apolloammxenable.ao video/apollo/blitapollo.ao video/apollo/colorkeyapollo.ao
#video/apollo/blitapollo.ao video/apollo/apolloammxenable.ao video/apollo/colorkeyapollo.ao \
	video/apollo/RLEAlphaBlit565.ao video/apollo/ApolloARGBto565AlphaBlit.ao
#GCCFLAGS += -DAPOLLO_BLITDBG