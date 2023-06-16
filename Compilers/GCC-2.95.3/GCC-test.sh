# ApolloCrossDev GCC-2.95.3 Install Script v1.5
# 
# Installation:
# 1. Enter Compilers/GCC-2.95.3 directory
# 2. Type "./GCC-2.95.3.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc2953 into <mysource> 
# 3. Read make-gcc2953 for compile instructions

EDITION=GCC-2.95.3
VERSION=1.5
CPU=-j4
GCCVERSION=2.95.3
CFLAGS_FOR_TARGET="-O2 -fomit-frame-pointer"

WORKSPACE="`pwd`"
ARCHIVES=$WORKSPACE/_archives
SOURCES=$WORKSPACE/_sources
BUILDS=$WORKSPACE/_builds
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

CC="gcc"
CXX="g++"
CC32="gcc -m32 -std=gnu11"
CXX32="g++ -m32 -std=gnu++11"
FLAGS="-g -O2"

NDK32_NAME=NDK3.2
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_NAME=NDK_3.9
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3
NDK39_ARCHIVE=NDK39.lha
BINUTILS_NAME=amigaos-binutils-2.14
BINUTILS_DOWNLOAD=https://github.com/adtools/amigaos-binutils-2.14
GCC_NAME=amigaos-gcc-2.95.3
GCC_DOWNLOAD=https://github.com/adtools/amigaos-gcc-2.95.3
FD2SFD_NAME=fd2sfd
FD2SFD_DOWNLOAD=https://github.com/adtools/fd2sfd
FD2PRAGMA_NAME=fd2pragma
FD2PRAGMA_DOWNLOAD=https://github.com/adtools/fd2pragma.git
SFDC_NAME=sfdc
SFDC_DOWNLOAD=https://github.com/adtools/sfdc
IXEMUL_NAME=ixemul-48.2
IXEMUL_DOWNLOAD=http://downloads.sf.net/project/amiga/ixemul.library/48.2/ixemul-src.lha
IXEMUL_ARCHIVE=ixemul-src.lha
CLIB2_NAME=clib2
CLIB2_DOWNLOAD=https://github.com/adtools/clib2
LIBNIX_NAME=libnix
LIBNIX_DOWNLOAD=https://github.com/adtools/libnix
LIBAMIGA_NAME=libamiga
LIBAMIGA_DOWNLOAD=ftp://ftp.exotica.org.uk/mirrors/geekgadgets/amiga/m68k/snapshots/990529/bin/libamiga-bin.tgz
LIBAMIGA_ARCHIVE=libamiga-bin.tgz
LIBDEBUG_NAME=libdebug
LIBDEBUG_DOWNLOAD=https://github.com/adtools/libdebug
LIBM_NAME=libm-5.4
LIBM_DOWNLOAD=ftp://ftp.exotica.org.uk/mirrors/geekgadgets/amiga/m68k/snapshots/990529/src/libm-5.4-src.tgz
LIBM_ARCHIVE=libm-5.4-src.tgz
LIBOGG_NAME=libogg-1.3.5


rm $LOGFILES/*
echo -e -n "\e[0m\e[36m   * $LIBOGG_NAME:\e[30m configure | "
cd $SOURCES/$LIBOGG_NAME
CC="$PREFIX/bin/$TARGET-gcc -noixemul -static-libgcc" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$LIBOGG_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part10_libogg_configure.log 2>>$LOGFILES/part10_libogg_configure_err.log  
echo -e -n "make | "
make -j1 >>$LOGFILES/part10_libogg_make.log 2>>$LOGFILES/part10_libogg_make_err.log   
echo -e "install\e[0m"
make -j1 install >>$LOGFILES/part10_libogg_make.log 2>>$LOGFILES/part10_libogg_make_err.log   
cd $SOURCES
