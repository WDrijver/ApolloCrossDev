# ApolloCrossDev Secondary Install Script v1.0 - MacOS

# INIT Terminal
clear
echo "\033[1m\033[37m########## \033[31mApollo\033[1;30mCrossDev \033[36m$EDITION\033[30m v$VERSION \033[37m ###########\033[0m\033[36m"
echo "\033[1m\033[37m#"
echo "\033[1m\033[37m# \033[0mBuilding with CPU=$CPU | If Build fails set CPU=-j1\033[0m\033[36m"
echo " "
echo "\033[1m\033[37m0. Sudo Password\033[0m"

# PART 1: Clean the House
echo "\033[1m\033[37m1. Clean the House\033[0m\033[36m"
rm -f -r $PREFIX
mkdir $PREFIX
rm -f -r $LOGFILES
mkdir -p $LOGFILES
rm -f -r $SOURCES
mkdir $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo "\033[1m\033[37m2. Update Linux Packages\033[0m\033[36m"
brew install bash wget make lhasa gmp mpfr libmpc flex gettext gnu-sed texinfo gcc@12 make autoconf bison >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Clone Amiga-GCC
echo "\033[1m\033[37m3. Clone Amiga-GCC (Stefan -Bebbo- Franke)\033[0m\033[36m"
git clone --progress -b $BRANCH $MASTER 2>>$LOGFILES/part3_err.log

# Part 4: Compile Amiga-GCC
echo "\033[1m\033[37m4. Compile Amiga-GCC\033[0m\033[36m"
cd $SOURCES/amiga-gcc
echo "\033[0m\033[36m   * Clean Amiga-GCC\033[0m"
make clean >>$LOGFILES/part4_clean.log 2>>$LOGFILES/part4_clean_err.log
echo "\033[0m\033[36m   * Clean ApolloCrossDev\033[0m"
make drop-prefix PREFIX=$PREFIX >>$LOGFILES/part4_dropprefix.log 2>>$LOGFILES/part4_dropprefix_err.log
echo "\033[0m\033[36m   * Build Amiga-GCC (be patient)\033[0m"
CC=gcc-12 CXX=g++-12 make all $CPU SHELL=$(brew --prefix)/bin/bash PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 5: MUI
echo "\033[1m\033[37m5. Adding MUI5\033[0m\033[36m"
cd $SOURCES/amiga-gcc
make sdk=mui PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: PortLibs (amiga-gcc takes care of Open-GL, SDL and GDB - we add Freetype, ZLib and BZip2)
echo -n "\033[1m\033[37m6. Adding Porting Libs: "
cd $PREFIX
echo -n "\033[0m\033[36mGL | "
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -n "\033[0m\033[36mGDB | "
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -n "\033[0m\033[36mSDL | "
cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
rm -r -f $PREFIX/lib/libSDL* >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
rm -r -f $PREFIX/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mSDL-TTF | "
cd $ARCHIVES/SDL-TTF
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mSDL-Mixer | "
cd $ARCHIVES/SDL-Mixer
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mSDL-Images | "
cd $ARCHIVES/SDL-Images
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mFreeType2 | "
cd $ARCHIVES/freetype2
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mVorbis | "
cd $ARCHIVES/vorbis
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mMPEGA (Dynamic) | "
cd $ARCHIVES/MPEGA-source/include
cp -r -f clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 38 -t $PREFIX/$TARGET/include/proto >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 40 -t $PREFIX/$TARGET/include/inline >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -n "\033[0m\033[36mZLib | "
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

echo "\033[0m\033[36mTimidity\033[0m"
mkdir $SOURCES/timidity-source
cp -r -f $ARCHIVES/timidity-source/src/* $SOURCES/timidity-source
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp -f *.h $PREFIX/$TARGET/include/timidity

# PART 7: NDK
echo "\033[1m\033[37m7. Development Kits\033[0m\033[36m"
cd $PREFIX/$TARGET
git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $ARCHIVES
cp -r -f P96/* $PREFIX/$TARGET/include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f cmake/* $PREFIX/lib >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

# Part 8: ApolloExplorer
echo "\033[1m\033[37m8. ApolloExplorer\033[0m\033[36m"
cd $WORKSPACE/$PROJECTS
git clone --progress https://github.com/ronybeck/ApolloExplorer >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
cd $WORKSPACE/$PROJECTS/ApolloExplorer
qmake >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log

# Part 9: BGDBServer
echo "\033[1m\033[37m9. BGDG Server\033[0m\033[36m"
cd $WORKSPACE/$PROJECTS
git clone --progress https://github.com/WDrijver/bgdbserver >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
cd $WORKSPACE/$PROJECTS/bgdbserver
make >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log

# PART 10: Cleanup
echo "\033[1m\033[37m10. Cleanup\033[0m\033[36m"
rm -rf $SOURCES
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo "\033[1m\033[32mFINISHED\033[0m"
echo " "
exit