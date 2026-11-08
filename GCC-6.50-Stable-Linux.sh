#!/bin/bash
# ApolloCrossDev Primary Install Script v1.2

VERSION=1.2
CPU=-j16

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-6.50-Stable
TARGET=m68k-amigaos
PREFIX=$WORKSPACE/$COMPILERS/$COMPILER

MASTER=https://github.com/WDrijver/amiga-gcc
BRANCH=amiga-gcc-stable

ARCHIVES=$WORKSPACE/$COMPILERS/_archives
LOGFILES=$PREFIX/_logs
BUILDS=$PREFIX/_builds
SOURCES=$PREFIX/_sources

export PATH=$PREFIX/bin:$PATH

# ApolloCrossDev Secondary Install Script v1.2

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$COMPILER\e[30m v$VERSION \e[37m ###########\e[0m\e[36m"
echo -e "\e[1m\e[37m#"
echo -e "\e[1m\e[37m# \e[0mBuilding with CPU=$CPU | If Build fails set CPU=-j1\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Check root status\e[0m"

# PART 1: Clean the House
sudo echo -e "\e[1m\e[37m1. Clean the House\e[0m\e[36m"
rm -f -r $PREFIX
mkdir -p $PREFIX
mkdir -p $LOGFILES
rm -f -r $SOURCES
mkdir -p $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install make wget git gcc g++ lhasa libgmp-dev libmpfr-dev libmpc-dev flex bison gettext texinfo ncurses-dev autoconf rsync libreadline-dev >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential devscripts debhelper >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Clone Amiga-GCC
echo -e "\e[1m\e[37m3. Clone Amiga-GCC (Stefan -Bebbo- Franke)\e[0m\e[36m"
git clone --progress -b $BRANCH $MASTER 2>>$LOGFILES/part3_err.log

# Part 4: Compile Amiga-GCC
echo -e -n "\e[1m\e[37m4. Compile Amiga-GCC: \e[0m\e[36m"
cd $SOURCES/amiga-gcc
echo -e -n "\e[0m\e[36mMake Clean | "
make clean >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e -n "\e[0m\e[36mDrop Prefix | "
make drop-prefix PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

echo -e -n "\e[0m\e[36mClone Repos (>1 min)\e[0m\e[36m\n"
# -k: a repository gone upstream (vbcc's master branch, 2026-10) no longer stops the others
make update -k $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Apply Patches from Ioannis Kouretsidis (@JohnStuggi)
echo -e "\e[0m\e[36mApplying Apollo 68080 Patches from Ioannis Kouretsidis (@JohnStuggi)\e[0m"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/q2g-stable.patch >>$LOGFILES/patch.log 2>>$LOGFILES/patch.log

# Apply Patches from Morten (@Morten)
echo -e "\e[0m\e[36mApplying Apollo 68080 Patches from Morten (@Morten)"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/opt-absolute-volatile-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-pipeline-cc0-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-shift-lsr-signext-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/postinc-size-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

echo -e "\e[0m\e[36mOverview of the Applied Patches: "
git diff --stat 

# Apply Patches for ISL
echo -e "\e[0m\e[36mApplying Patches for ISL"
cd $ARCHIVES/isl
cp -f * $SOURCES/amiga-gcc/projects/gcc/gcc

# Build Compiler
cd $SOURCES/amiga-gcc
echo -e -n "\e[0m\e[36mBuild Compiler (>5 min) | "
make all -k $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36mAdd LibDebug\e[0m"
make libdebug PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Build Assembler (vasm)
cd $ARCHIVES/vasm
tar -xzf vasm.tar
cd vasm
make CPU=m68k SYNTAX=mot >>$LOGFILES/vasm.log 2>>$LOGFILES/vasm_err.log
cp -f vasmm68k_mot $PREFIX/bin/vasmm68k_mot >>$LOGFILES/vasm.log 2>>$LOGFILES/vasm_err.log
cp -f vobjdump $PREFIX/bin/vobjdump >>$LOGFILES/vasm.log 2>>$LOGFILES/vasm_err.log
cd $ARCHIVES/vasm
rm -rf vasm

# Build Linker (vlink)
cd $ARCHIVES/vlink
tar -xzf vlink.tar
cd vlink
make CPU=m68k >>$LOGFILES/vlink.log 2>>$LOGFILES/vlink_err.log
cp -f vlink $PREFIX/bin/vlink >>$LOGFILES/vlink.log 2>>$LOGFILES/vlink_err.log
cd $ARCHIVES/vlink
rm -rf vlink

# Part 5: MUI
echo -e "\e[1m\e[37m5. Adding MUI5\e[0m\e[36m"
cd $SOURCES/amiga-gcc
make sdk=mui PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
# MUI 3.8 is muimaster.library 19: lower MUI5's MUIMASTER_VMIN (20) so programs also open on MUI 3.8
perl -pi -e 's/^(#define MUIMASTER_VMIN\s+)20\b/${1}19/' $PREFIX/$TARGET/include/libraries/mui.h >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

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

# Porting libs built from source (same CPU/FPU/libc as the Makefile template)
LIB_CFLAGS="-O2 -m68080 -m68881 -noixemul -fomit-frame-pointer"
LIB_ENV="PATH=$PREFIX/bin:$PATH CC=$PREFIX/bin/$TARGET-gcc AR=$PREFIX/bin/$TARGET-ar RANLIB=$PREFIX/bin/$TARGET-ranlib STRIP=$PREFIX/bin/$TARGET-strip"
LIB_CONF="--host=$TARGET --prefix=$PREFIX/$TARGET --disable-shared --enable-static"
LIB_DEPS="CPPFLAGS=-I$PREFIX/$TARGET/include LDFLAGS=-L$PREFIX/$TARGET/lib"

# lib_unpack <archive dir> <tarball>: extract into $SOURCES and cd into it
lib_unpack() {
  cd $SOURCES && tar -xzf $ARCHIVES/$1/$2 && cd $SOURCES/$(tar -tzf $ARCHIVES/$1/$2 | head -1 | cut -d/ -f1)
}

echo -e -n "\e[0m\e[36mZLib | "
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

echo -e -n "\e[0m\e[36mMiniZip | "
cd $SOURCES/zlib-source/contrib/minizip
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libminizip.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mOgg | "
lib_unpack vorbis libogg-1.3.6.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" ./configure $LIB_CONF >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C src install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C include install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-pkgconfigDATA >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mVorbis | "
lib_unpack vorbis libvorbis-1.3.7.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS ./configure $LIB_CONF --with-ogg=$PREFIX/$TARGET --disable-oggtest --disable-docs --disable-examples >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C lib install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C include install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-pkgconfigDATA >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mPNG | "
lib_unpack SDL_images libpng-1.6.59.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS ./configure $LIB_CONF --disable-tools --disable-tests >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-pkgincludeHEADERS install-nodist_pkgincludeHEADERS install-pkgconfigDATA install-header-links install-library-links >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mJPEG | "
lib_unpack SDL_images jpegsrc.v9f.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" ./configure $LIB_CONF >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-includeHEADERS >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -f jconfig.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mFreeType2 | "
lib_unpack freetype2 freetype-2.14.3.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS CC_BUILD=cc ./configure $LIB_CONF --enable-freetype-config --with-zlib=yes --with-png=no --with-bzip2=no --with-brotli=no --with-harfbuzz=no >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

# amiga-gcc's SDL 1.2 ships no sdl-config, which SDL_mixer's configure needs: add a minimal one
cat > $PREFIX/bin/$TARGET-sdl-config <<SDLCONFIG
#!/bin/sh
# Minimal sdl-config for the amiga-gcc SDL 1.2 port (ApolloCrossDev)
prefix=$PREFIX/$TARGET
for a in "\$@"; do case "\$a" in
  --version) echo 1.2.14 ;;
  --prefix|--exec-prefix) echo "\$prefix" ;;
  --cflags) echo "-I\$prefix/include/SDL -D_GNU_SOURCE=1" ;;
  --libs|--static-libs) echo "-L\$prefix/lib -lSDL" ;;
esac; done
SDLCONFIG
chmod +x $PREFIX/bin/$TARGET-sdl-config

echo -e -n "\e[0m\e[36mSDL_mixer | "
mkdir -p $SOURCES/SDL_mixer
cp -r -f $ARCHIVES/SDL_mixer/* $SOURCES/SDL_mixer
cd $SOURCES/SDL_mixer
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS SDL_CONFIG=$PREFIX/bin/$TARGET-sdl-config ./configure $LIB_CONF --disable-sdltest --disable-music-cmd --disable-music-ogg-shared --disable-music-mod --disable-music-midi-fluidsynth --disable-music-flac --disable-music-mp3 >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
# library only: the playwave/playmus demos are not needed
make build/libSDL_mixer.la >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f build/.libs/libSDL_mixer.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f SDL_mixer.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_net | "
mkdir -p $SOURCES/SDL_net
cp -r -f $ARCHIVES/SDL_net/* $SOURCES/SDL_net
cd $SOURCES/SDL_net
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libSDL_net.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r *.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_ttf | "
lib_unpack SDL_ttf SDL_ttf-2.0.11.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS SDL_CONFIG=$PREFIX/bin/$TARGET-sdl-config ./configure $LIB_CONF --disable-sdltest --without-x --with-freetype-prefix=$PREFIX/$TARGET >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-libSDL_ttfincludeHEADERS >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mSDL_image | "
lib_unpack SDL_images SDL_image-1.2.12.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS SDL_CONFIG=$PREFIX/bin/$TARGET-sdl-config LIBPNG_CFLAGS=-I$PREFIX/$TARGET/include/libpng16 LIBPNG_LIBS="-lpng16 -lz" LIBS="-lz -lm" ./configure $LIB_CONF --disable-sdltest --without-x --disable-png-shared --disable-jpg-shared --disable-tif --disable-webp >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-libSDL_imageincludeHEADERS >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mMPEGA (Dynamic) | "
cd $ARCHIVES/MPEGA-source/include
cp -r -f clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 38 -t $PREFIX/$TARGET/include/proto >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 40 -t $PREFIX/$TARGET/include/inline >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e -n "\e[0m\e[36mLua | "
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

echo -e "\e[0m\e[36mTimidity\e[0m"
mkdir $SOURCES/timidity-source
cp -r -f $ARCHIVES/timidity-source/src/* $SOURCES/timidity-source
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp -f *.h $PREFIX/$TARGET/include/timidity

# PART 7: NDK
echo -e "\e[1m\e[37m7. Development Kits\e[0m\e[36m"
cd $PREFIX/$TARGET
git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $ARCHIVES
cp -r -f P96/* $PREFIX/$TARGET/include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f cmake/* $PREFIX/lib >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

# PART 8: Cleanup
echo -e "\e[1m\e[37m8. Cleanup\e[0m\e[36m"
rm -rf $SOURCES
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit


