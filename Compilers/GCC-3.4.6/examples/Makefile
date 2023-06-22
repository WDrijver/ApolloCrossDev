# Generated automatically from Makefile.in by configure.
#### Start of system configuration section. ####

srcdir =	/home/willem/ApolloCrossDev/Compilers/GCC-3.4.6/_builds/build-libnix3/examples
VPATH =		/home/willem/ApolloCrossDev/Compilers/GCC-3.4.6/_builds/build-libnix3/examples

CC =		/home/willem/ApolloCrossDev/Compilers/GCC-3.4.6/ApolloCrossDev/bin/m68k-unknown-amigaos-gcc -Wall -m68020-60 -O2 -msoft-float -funroll-loops -fomit-frame-pointer -noixemul

#### End system configuration section ####

# Some examples

.PHONY: all

all: lib libr dev helloworld

# normal library
lib: libinit.o simplelib.c
	$(CC) -nostdlib -O3 -fomit-frame-pointer -fbaserel $^ -o $@

# multibase library
libr: libinitr.o simplelib.c
	$(CC) -nostdlib -O3 -fomit-frame-pointer -resident $^ -o $@

# device
dev: devinit.o simpledev.c
	$(CC) -nostdlib -O3 -fomit-frame-pointer -fbaserel $^ -o $@

# helloworld
helloworld: nbcrt0.o helloworld.c libstubs.a
	$(CC) -nostdlib -O3 -fomit-frame-pointer -s -fbaserel $^ -o $@
