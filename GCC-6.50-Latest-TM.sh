# ApolloCrossDev Install Script v1.0

VERSION=1.0
CPU=-j16

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-6.50-Latest
TARGET=m68k-amigaos
PREFIX=$WORKSPACE/$COMPILERS/$COMPILER

ARCHIVES=$WORKSPACE/$COMPILERS/_archives
LOGFILES=$PREFIX/_logs
BUILDS=$PREFIX/_builds
SOURCES=$PREFIX/_sources

export PATH=$PREFIX/bin:$PATH

# Part 6: PortLibs (amiga-gcc takes care of Open-GL, SDL and GDB - we add Freetype, ZLib and BZip2)
echo -e -n "\e[1m\e[37m6. Adding Porting Libs: "
cd $PREFIX
echo -e -n "\e[0m\e[36mGL | "
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -e -n "\e[0m\e[36mGDB | "
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -e -n "\e[0m\e[36mSDL | "
cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
rm -r -f $PREFIX/lib/libSDL* >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
rm -r -f $PREFIX/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL-TTF | "
cd $ARCHIVES/SDL-TTF
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL-Mixer | "
cd $ARCHIVES/SDL-Mixer
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL-Images | "
cd $ARCHIVES/SDL-Images
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mFreeType2 | "
cd $ARCHIVES/freetype2
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mVorbis | "
cd $ARCHIVES/vorbis
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mZLib | "
cp -r -f $ARCHIVES/zlib-source $SOURCES/zlib-source
cd $SOURCES/zlib-source
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=CC=$PREFIX/bin/m68k-amigaos-ranlib \
CFLAGS="-noixemul -m68040 -O2 -ffast-math -fomit-frame-pointer" \
./configure >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make all $CPU >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libz.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/zlib
cp -r zlib.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zconf.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=CC=$PREFIX/bin/m68k-amigaos-ranlib \
CFLAGS="-noixemul -m68040 -O2 -ffast-math -fomit-frame-pointer" \
./configure >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make all $CPU >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libz.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/zlib
cp -r zlib.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zconf.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\e[0m\e[36mTimidity\e[0m"
cp -r -f $ARCHIVES/timidity-source/src $SOURCES/timidity-source
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp *.h $PREFIX/$TARGET/include/timidity
