#
# Makefile for AROS - unrar

CXX=g++
CC=gcc
AR=ar
CFLAGS=-O2 -Wall
CXXFLAGS=$(CFLAGS)
STRIP=strip
LDFLAGS=-L. -lunrar

##########################

UNRAR_OBJ=filestr.o recvol.o rs.o scantree.o qopen.o
LIB_OBJ=filestr.o scantree.o dll.o qopen.o

OBJECTS=rar.o strlist.o strfn.o pathfn.o smallfn.o global.o file.o filefn.o filcreat.o \
	archive.o arcread.o unicode.o system.o isnt.o crypt.o crc.o rawread.o encname.o \
	resource.o match.o timefn.o rdwrfn.o consio.o options.o errhnd.o rarvm.o secpassword.o \
	rijndael.o getbits.o sha1.o sha256.o blake2s.o hash.o extinfo.o extract.o volume.o \
  list.o find.o unpack.o headers.o threadpool.o rs16.o cmddata.o

OBJECTS+=aros_wchar.o

all: unrar
# libunrar.a UnRDLL

clean:
	rm -f *.o

unrar: CFLAGS+= -DUNRAR
unrar: CXXFLAGS+= -DUNRAR
unrar: $(OBJECTS) $(UNRAR_OBJ)
	$(CXX) -o $@ $^

UnRDLL: CFLAGS+= -D_UNIX
UnRDLL: $(OBJECTS) UnRDLL.o
	$(CXX) -o $@ $^ $(LDFLAGS)

libunrar.a: CFLAGS+= -DRARDLL -DSILENT
libunrar.a:	CXXFLAGS+= -DRARDLL -DSILENT
libunrar.a: $(OBJECTS) $(LIB_OBJ)
	$(AR) rcs $@ $?

