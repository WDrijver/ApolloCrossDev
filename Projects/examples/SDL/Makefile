INCLUDES = -I../include/SDL -I../include
CFLAGS   = -O2 -fomit-frame-pointer
LFLAGS   = -noixemul
LIBS     = -L../lib/ -lSDLstub

all: testbitmap testsprite testwin testpalette loopwave

clean:
	-rm -f testbitmap testsprite testwin testpalette loopwave

testbitmap: testbitmap.c
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) -o testbitmap testbitmap.c $(LIBS)

loopwave: loopwave.c
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) -o loopwave loopwave.c 

testsprite: testsprite.c
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) -o testsprite testsprite.c $(LIBS)

testpalette: testpalette.c
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) -o testpalette testpalette.c $(LIBS) -lm

testwin: testwin.c
	gcc $(CFLAGS) $(LFLAGS) $(INCLUDES) -o testwin testwin.c $(LIBS)


	