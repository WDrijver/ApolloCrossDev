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
rm -f -r $PREFIX >/dev/null
mkdir $PREFIX
rm -f -r $LOGFILES >/dev/null
mkdir -p $LOGFILES
rm -f -r $SOURCES >/dev/null
mkdir $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo "\033[1m\033[37m2. Update Linux Packages\033[0m\033[36m"
brew install bash wget make lhasa gmp mpfr libmpc flex gettext gnu-sed texinfo gcc@12 make automake autoconf bison qt@5 >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

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
echo "\033[0m\033[36m   * Clone Repos (>1 min)"
make update $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Apply Patches from Ioannis Kouretsidis (@JohnStuggi)
echo "\033[0m\033[36m   * Applying 68080 AMMX Patches from Ioannis Kouretsidis (@JohnStuggi)\033[0m"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/q2g-stable.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git diff --stat 
cd $SOURCES/amiga-gcc

echo "\033[0m\033[36m   * Build Amiga-GCC (be patient)\033[0m"
CC=gcc-12 CXX=g++-12 gmake all $CPU NDK=3.2 SHELL=$(brew --prefix)/bin/bash PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo "\033[0m\033[36m   * Add LibDebug\033[0m"
make libdebug PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Part 5: MUI
echo "\033[1m\033[37m5. Adding MUI5\033[0m\033[36m"

cd $SOURCES/amiga-gcc
make sdk=mui PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Patch MUI5 proto header to be compatible with Amiga-GCC
cd $ARCHIVES/MUI5
cp -r -f muimaster_lib.h $PREFIX/$TARGET/include/proto >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: PortLibs (amiga-gcc takes care of Open-GL, SDL and GDB - we add Freetype, ZLib and BZip2)
echo "\033[1m\033[37m6. Adding Porting Libs: "
cd $PREFIX

echo "\033[0m\033[36m  * GL\033[0m"
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo "\033[0m\033[36m  * GDB\033[0m"
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * SDL\033[0m"
cp -r -f $PREFIX/include/SDL $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f $PREFIX/lib/libSDL* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * SDL_mixer\033[0m"
cd $ARCHIVES/SDL_mixer
mkdir -p $SOURCES/SDL_mixer
cp -r -f $ARCHIVES/SDL_mixer/* $SOURCES/SDL_mixer
cd $SOURCES/SDL_mixer
PATH=$PREFIX/bin:$PATH \
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
./configure --host=m68k-amigaos --prefix=$PREFIX/$TARGET >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make CFLAGS="-noixemul -g -O2" LDFLAGS="-noixemul -lSDL" >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f build/.libs/libSDL_mixer.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f *.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * SDL_net\033[0m"
mkdir -p $SOURCES/SDL_net
cp -r -f $ARCHIVES/SDL_net/* $SOURCES/SDL_net
cd $SOURCES/SDL_net
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libSDL_net.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r *.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * SDL_ttf\033[0m"
cd $ARCHIVES/SDL_ttf
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * SDL_images\033[0m"
cd $ARCHIVES/SDL_images
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * Vorbis\033[0m"
cd $ARCHIVES/vorbis
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * FreeType2\033[0m"
cd $ARCHIVES/freetype2
cp -r -f include/* $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * MPEGA (Dynamic)\033[0m"
cd $ARCHIVES/MPEGA-source/include
cp -r -f clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 38 -t $PREFIX/$TARGET/include/proto >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 40 -t $PREFIX/$TARGET/include/inline >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * ZLib\033[0m"
mkdir -p $SOURCES/zlib-source
cp -r -f $ARCHIVES/zlib-source/* $SOURCES/zlib-source
cd $SOURCES/zlib-source
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libz.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zlib.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r zconf.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * MiniZip\033[0m"
cd $SOURCES/zlib-source/contrib/minizip
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libminizip.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * LUA\033[0m"
mkdir -p $SOURCES/lua
cp -r -f $ARCHIVES/lua/* $SOURCES/lua
cd $SOURCES/lua
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f liblua.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/lua
cp -r *.h $PREFIX/$TARGET/include/lua >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo "\033[0m\033[36m  * Timidity\033[0m"
mkdir $SOURCES/timidity-source
cp -r -f $ARCHIVES/timidity-source/src/* $SOURCES/timidity-source
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp -f *.h $PREFIX/$TARGET/include/timidity

echo "\033[0m\033[36m  * ffmpeg\033[0m"
mkdir -p $PREFIX/$TARGET/include/ffmpeg
cp -r -f $ARCHIVES/ffmpeg/* $PREFIX/$TARGET/include

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
git clone --progress https://github.com/WDrijver/ApolloExplorer >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
cd $WORKSPACE/$PROJECTS/ApolloExplorer
/opt/homebrew/opt/qt@5/bin/qmake -recursive >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make clean >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make -j16 >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log
make clean >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log

# Part 9: BGDBServer
echo "\033[1m\033[37m9. BGDG Server\033[0m\033[36m"
cd $WORKSPACE/$PROJECTS
git clone --progress https://github.com/WDrijver/bgdbserver >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
cd $WORKSPACE/$PROJECTS/bgdbserver
make >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
exit
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