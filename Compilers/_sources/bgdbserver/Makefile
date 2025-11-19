bgdbserver: src/main.c src/breakpoint.h src/breakpoint.c src/slave.c src/common.h
	m68k-amigaos-gcc -Os -fomit-frame-pointer -noixemul src/*.c -o bgdbserver -s

clean:
	rm bgdbserver

