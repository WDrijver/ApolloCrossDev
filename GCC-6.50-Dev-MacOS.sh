#!/bin/bash
# ApolloCrossDev Primary Install Script v1.2 - MacOS

#Clear Terminal
printf '\33c\e[3J'

VERSION=1.2
CPU=-j16

WORKSPACE="`pwd`"
COMPILERS=Compilers
PROJECTS=Projects
COMPILER=GCC-6.50-Dev
TARGET=m68k-amigaos
PREFIX=$WORKSPACE/$COMPILERS/$COMPILER

MASTER=https://github.com/WDrijver/amiga-gcc
BRANCH=amiga-gcc-stable

ARCHIVES=$WORKSPACE/$COMPILERS/_archives
LOGFILES=$PREFIX/_logs
BUILDS=$PREFIX/_builds
SOURCES=$PREFIX/_sources

export PATH=$PREFIX/bin:$PATH

sudo xcodebuild -license accept >/dev/null
export PATH=$(brew --prefix bison)/bin:$PATH

# ApolloCrossDev Secondary Install Script v1.2 - MacOS

# INIT Terminal
clear
echo -e "\033[1m\033[37m########## \033[31mApollo\033[1;30mCrossDev \033[36m$EDITION\033[30m v$VERSION \033[37m ###########\033[0m\033[36m"
echo -e "\033[1m\033[37m#"
echo -e "\033[1m\033[37m# \033[0mBuilding with CPU=$CPU | If Build fails set CPU=-j1\033[0m\033[36m"
echo " "
echo -e "\033[1m\033[37m0. Sudo Password\033[0m"

# PART 1: Clean the House
echo -e "\033[1m\033[37m1. Clean the House\033[0m\033[36m"
rm -f -r $PREFIX >/dev/null
mkdir $PREFIX
rm -f -r $LOGFILES >/dev/null
mkdir -p $LOGFILES
rm -f -r $SOURCES >/dev/null
mkdir $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\033[1m\033[37m2. Update Linux Packages\033[0m\033[36m"
brew install gcc@12 bash wget make lhasa gmp mpfr libmpc flex gettext gnu-sed texinfo make automake autoconf bison >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Clone Amiga-GCC
echo -e "\033[1m\033[37m3. Clone Amiga-GCC (Stefan -Bebbo- Franke)\033[0m\033[36m"
git clone --progress -b $BRANCH $MASTER 2>>$LOGFILES/part3_err.log

# Part 4: Compile Amiga-GCC
echo -e "\033[1m\033[37m4. Compile Amiga-GCC\033[0m\033[36m"
cd $SOURCES/amiga-gcc
echo -e "\033[0m\033[36m   * Clean Amiga-GCC\033[0m"
CC=gcc-12 CXX=g++-12 gmake clean $CPU >>$LOGFILES/part4_clean.log 2>>$LOGFILES/part4_clean_err.log
echo -e "\033[0m\033[36m   * Clean ApolloCrossDev\033[0m"
CC=gcc-12 CXX=g++-12 gmake drop-prefix $CPU PREFIX=$PREFIX >>$LOGFILES/part4_dropprefix.log 2>>$LOGFILES/part4_dropprefix_err.log
echo -e "\033[0m\033[36m   * Clone Repos (>1 min)"
# -k: a repository gone upstream (vbcc's master branch, 2026-10) no longer stops the others
CC=gcc-12 CXX=g++-12 gmake update -k $CPU NDK=3.2 PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Apply Patches from Ioannis Kouretsidis (@JohnStuggi)
echo -e "\033[0m\033[36m   * Applying Apollo 68080 Patches from Ioannis Kouretsidis (@JohnStuggi)\033[0m"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/q4d-candidate.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/q7k-post-q4d-candidate.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Apply Patches from Morten (@Morten)
echo -e "\033[0m\033[36m   * Applying Apollo 68080 Patches from Morten (@Morten)\033[0m"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/opt-absolute-volatile-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-pipeline-cc0-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-shift-lsr-signext-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-shift-jump-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/opt-elim-shift-count-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/postinc-size-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
git apply $ARCHIVES/patches/bbb-autoinc-direct-add-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# m68k backend fix: dbne_hi/dbge_hi with the loop counter in an address register (cmp.w there is
# 32-bit: endless loops at -O2). See the patch's header.
echo -e "\033[0m\033[36m   * Applying the m68k dbra HI address-register fix\033[0m"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/m68k-dbra-hi-addrreg-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# m68k backend fix: -mrtd crashed on every call (no call_pop patterns; bbb track_sp and
# notice_update_cc did not know them either). See the patch's header.
echo -e "\033[0m\033[36m   * Applying the m68k -mrtd call_pop fix"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/m68k-mrtd-call-pop.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# m68k backend fix: sincos<mode>2_68881 had no constraint on its input operand (ICE in
# print_operand_address2 with -m68881 -ffast-math). See the patch's header.
echo -e "\033[0m\033[36m   * Applying the m68k sincos constraint fix"
cd $SOURCES/amiga-gcc/projects/gcc
git apply $ARCHIVES/patches/m68k-sincos-constraint-fix.patch >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

echo -e "\033[0m\033[36m   * Overview of the Applied Patches:\033[0m"
git diff --stat 

# Apply Patches for ISL
echo -e "\033[0m\033[36m   * Applying Patches for ISL\033[0m"
cd $ARCHIVES/isl
cp -f * $SOURCES/amiga-gcc/projects/gcc/gcc

# Build Compiler
cd $SOURCES/amiga-gcc
echo -e "\033[0m\033[36m   * Build Amiga-GCC (be patient)\033[0m"
CC=gcc-12 CXX=g++-12 gmake all -k $CPU NDK=3.2 SHELL=$(brew --prefix)/bin/bash PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\033[0m\033[36m   * Add LibDebug\033[0m"
CC=gcc-12 CXX=g++-12 gmake libdebug $CPU PREFIX=$PREFIX >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log

# Build Assembler (vasm)
cd $ARCHIVES/vasm
# start clean: stale objects would be newer than the extracted sources, so make would keep them
rm -rf vasm
# vasm.tar = unmodified vasm 2.0g daily 2026-09-03 (licence: redistribution without modifications)
tar -xzf vasm.tar
cd vasm
# Apollo patches by Renaud Schweingruber (TuKo1982), see README-vasm-2.0g-apollo.md
# -m68080: label destinations only become (d16,PC) with OPT OA+
patch -p1 < $ARCHIVES/vasm/vasm2_0g-apollo-pcdest.patch >>$LOGFILES/vasm.log 2>>$LOGFILES/vasm_err.log
# -m68080: sub.l #w stays sub (addiw.l #-w inverts C and X); move.l #-128..127,Dn becomes moveq
patch -p1 < $ARCHIVES/vasm/vasm2_0g-apollo-subflags-moveq.patch >>$LOGFILES/vasm.log 2>>$LOGFILES/vasm_err.log
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
echo -e "\033[1m\033[37m5. Adding MUI5\033[0m\033[36m"
cd $SOURCES/amiga-gcc
CC=gcc-12 CXX=g++-12 gmake sdk=mui $CPU PREFIX=$PREFIX >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r -f $SOURCES/amiga-gcc/build/mui/SDK/MUI/C/include/mui/* $PREFIX/$TARGET/include/mui >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
# MUI 3.8 is muimaster.library 19: lower MUI5's MUIMASTER_VMIN (20) so programs also open on MUI 3.8
perl -pi -e 's/^(#define MUIMASTER_VMIN\s+)20\b/${1}19/' $PREFIX/$TARGET/include/libraries/mui.h >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Copy proto headers to be compatible with Amiga-GCC (MacOS only)
cd $ARCHIVES/proto
cp -r -f *.h $PREFIX/$TARGET/include/proto >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log

# Part 6: PortLibs (amiga-gcc takes care of Open-GL, SDL and GDB - we add Freetype, ZLib and BZip2)
echo -e "\033[1m\033[37m6. Adding Porting Libs: "
cd $PREFIX

echo -e "\033[0m\033[36m  * GL\033[0m"
cp -r -f $PREFIX/include/GL $PREFIX/$TARGET/include/GL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -e "\033[0m\033[36m  * GDB\033[0m"
cp -r -f $PREFIX/include/gdb $PREFIX/$TARGET/include/GDB >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * SDL\033[0m"
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

echo -e "\033[0m\033[36m  * ZLib\033[0m"
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

echo -e "\033[0m\033[36m  * MiniZip\033[0m"
cd $SOURCES/zlib-source/contrib/minizip
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libminizip.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * Ogg\033[0m"
lib_unpack vorbis libogg-1.3.6.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" ./configure $LIB_CONF >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C src install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C include install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-pkgconfigDATA >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * Vorbis\033[0m"
lib_unpack vorbis libvorbis-1.3.7.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS ./configure $LIB_CONF --with-ogg=$PREFIX/$TARGET --disable-oggtest --disable-docs --disable-examples >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C lib install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make -C include install >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-pkgconfigDATA >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * PNG\033[0m"
lib_unpack SDL_images libpng-1.6.59.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS ./configure $LIB_CONF --disable-tools --disable-tests >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-pkgincludeHEADERS install-nodist_pkgincludeHEADERS install-pkgconfigDATA install-header-links install-library-links >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * JPEG\033[0m"
lib_unpack SDL_images jpegsrc.v9f.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" ./configure $LIB_CONF >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-includeHEADERS >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -f jconfig.h $PREFIX/$TARGET/include >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * FreeType2\033[0m"
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

echo -e "\033[0m\033[36m  * SDL_mixer\033[0m"
mkdir -p $SOURCES/SDL_mixer
cp -r -f $ARCHIVES/SDL_mixer/* $SOURCES/SDL_mixer
cd $SOURCES/SDL_mixer
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS SDL_CONFIG=$PREFIX/bin/$TARGET-sdl-config ./configure $LIB_CONF --disable-sdltest --disable-music-cmd --disable-music-ogg-shared --disable-music-mod --disable-music-midi-fluidsynth --disable-music-flac --disable-music-mp3 >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
# library only: the playwave/playmus demos are not needed
make build/libSDL_mixer.la >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f build/.libs/libSDL_mixer.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f SDL_mixer.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * SDL_net\033[0m"
mkdir -p $SOURCES/SDL_net
cp -r -f $ARCHIVES/SDL_net/* $SOURCES/SDL_net
cd $SOURCES/SDL_net
CC=$PREFIX/bin/m68k-amigaos-gcc \
AR=$PREFIX/bin/m68k-amigaos-ar \
RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
make >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libSDL_net.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r *.h $PREFIX/$TARGET/include/SDL >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * SDL_ttf\033[0m"
lib_unpack SDL_ttf SDL_ttf-2.0.11.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS SDL_CONFIG=$PREFIX/bin/$TARGET-sdl-config ./configure $LIB_CONF --disable-sdltest --without-x --with-freetype-prefix=$PREFIX/$TARGET >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-libSDL_ttfincludeHEADERS >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * SDL_image\033[0m"
lib_unpack SDL_images SDL_image-1.2.12.tar.gz
env $LIB_ENV CFLAGS="$LIB_CFLAGS" $LIB_DEPS SDL_CONFIG=$PREFIX/bin/$TARGET-sdl-config LIBPNG_CFLAGS=-I$PREFIX/$TARGET/include/libpng16 LIBPNG_LIBS="-lpng16 -lz" LIBS="-lz -lm" ./configure $LIB_CONF --disable-sdltest --without-x --disable-png-shared --disable-jpg-shared --disable-tif --disable-webp >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make install-libLTLIBRARIES install-libSDL_imageincludeHEADERS >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * MPEGA (Dynamic)\033[0m"
cd $ARCHIVES/MPEGA-source/include
cp -r -f clib/* $PREFIX/$TARGET/include/clib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp -r -f libraries/* $PREFIX/$TARGET/include/libraries >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 38 -t $PREFIX/$TARGET/include/proto >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
$PREFIX/bin/fd2pragma -i fd/mpega.fd -c clib/mpega_protos.h -s 40 -t $PREFIX/$TARGET/include/inline >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * LUA\033[0m"
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

echo -e "\033[0m\033[36m  * Timidity\033[0m"
mkdir $SOURCES/timidity-source
cp -r -f $ARCHIVES/timidity-source/src/* $SOURCES/timidity-source
cd $SOURCES/timidity-source
make -f Makefile.apollocrossdev >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cp libtimidity.a $PREFIX/$TARGET/lib >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
mkdir -p $PREFIX/$TARGET/include/timidity
cp -f *.h $PREFIX/$TARGET/include/timidity >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

echo -e "\033[0m\033[36m  * ffmpeg\033[0m"
mkdir -p $PREFIX/$TARGET/include/ffmpeg
cp -r -f $ARCHIVES/ffmpeg/* $PREFIX/$TARGET/include

# PART 7: NDK
echo -e "\033[1m\033[37m7. Development Kits\033[0m\033[36m"
cd $PREFIX/$TARGET
git clone --progress https://github.com/WDrijver/DevPac >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $ARCHIVES
cp -r -f P96/* $PREFIX/$TARGET/include >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r -f cmake/* $PREFIX/lib >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

# PART 8: Cleanup
echo -e "\033[1m\033[37m8. Cleanup\033[0m\033[36m"
rm -rf $SOURCES
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\033[1m\033[32mFINISHED\033[0m"
echo " "
exit