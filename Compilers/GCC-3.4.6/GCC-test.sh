# ApolloCrossDev GCC-3.4.6 Install Script v2.0
# 
# Installation:
# 1. Enter Compilers/GCC-3.4.6 directory
# 2. Type "./GCC-3.4.6.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc346 into <mysource> 
# 3. Read make-gcc346 for compile instructions

EDITION=GCC-3.4.6
VERSION=2.0
CPU=-j4
GCCVERSION=3.4.6
CFLAGS_FOR_TARGET="-O2 -fomit-frame-pointer"

WORKSPACE="`pwd`"
ARCHIVES=$WORKSPACE/_archives
SOURCES=$WORKSPACE/_sources
BUILDS=$WORKSPACE/_builds
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-unknown-amigaos
export PATH=$PREFIX/bin:$PATH

GCC_AUTOCONF=autoconf2.64
GCC_AUTOHEADER=autoheader2.64
GCC_AUTORECONF=autoreconf2.64
GCC_AUTOM4TE=autom4te2.64

BINUTILS_NAME=amigaos-binutils-2.14
BINUTILS_DOWNLOAD=https://github.com/adtools/$BINUTILS_NAME
GCC_NAME=gcc-3.4.6
GCC_DOWNLOAD=https://ftp.gnu.org/gnu/gcc/$GCC_NAME/$GCC_NAME.tar.gz
GMP_NAME=gmp-4.3.2
GMP_DOWNLOAD=https://ftp.gnu.org/gnu/gmp/$GMP_NAME.tar.gz
MPFR_NAME=mpfr-2.4.2
MPFR_DOWNLOAD=http://www.mpfr.org/$MPFR_NAME/$MPFR_NAME.tar.gz
MPC_NAME=mpc-0.8.2
MPC_DOWNLOAD=http://www.multiprecision.org/downloads/$MPC_NAME.tar.gz
BISON_NAME=bison-2.7.1
BISON_DOWNLOAD=https://ftp.gnu.org/gnu/bison/$BISON_NAME.tar.gz
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
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_NAME=NDK_3.9
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3
NDK39_ARCHIVE=NDK39.lha
NDK_ARCHIVE=ndk-$NDK_NAME-includes.tar.bz2
NDK_DOWNLOAD=http://kas1e.mikendezign.com/zerohero_crosscompilers_backup/files/m68k-amigaos/$NDK_ARCHIVE
OPENURL_NAME=7.16
OPENURL_ARCHIVE=OpenURL-$OPENURL_NAME.lha
OPENURL_DOWNLOAD=https://github.com/jens-maus/libopenurl/releases/download/$OPENURL_NAME/$OPENURL_ARCHIVE
AMISSL_NAME=4.4
AMISSL_ARCHIVE=AmiSSL-$AMISSL_NAME.lha 
AMISSL_DOWNLOAD=https://github.com/jens-maus/amissl/releases/download/$AMISSL_NAME/$AMISSL_ARCHIVE
GUIGFX_ARCHIVE=guigfxlib.lha
GUIGFX_DOWNLOAD=http://neoscientists.org/~bifat/binarydistillery/$GUIGFX_ARCHIVE
RENDER_ARCHIVE=renderlib.lha
RENDER_DOWNLOAD=http://neoscientists.org/~bifat/binarydistillery/$RENDER_ARCHIVE
CODESETS_NAME=6.20
CODESETS_ARCHIVE=codesets-$CODESETS_NAME.lha
CODESETS_DOWNLOAD=https://github.com/jens-maus/libcodesets/releases/download/$CODESETS_NAME/$CODESETS_ARCHIVE
LIBSDL_NAME=libSDL12
LIBVORBIS_NAME=libvorbis-1.3.7
LIBOGG_NAME=libogg-1.3.5
SDL_TTF_NAME=SDL_ttf-2.0.3
FREETYPE_NAME=freetype-2.13.0


# PART 10: Bonus SDK
echo -e "\e[1m\e[37m10. Bonus Libs/SDK\e[0m\e[36m"
rm $LOGFILES/*
echo -e -n "\e[0m\e[36m   * $FREETYPE_NAME:\e[30m configure | "
mkdir -p $BUILDS/build-$FREETYPE_NAME
cd $BUILDS/build-$FREETYPE_NAME
PATH="$PREFIX/bin:$PATH" \
CFLAGS="-I$PREFIX/$TARGET/include" \
LDFLAGS="-L$PREFIX/$TARGET/lib"  \
LIBPNG="libpng-config --libs" \
LIBPNG_CFLAGS="libpng-config --cflags" \
LIBPNG_LDFLAGS="libpng-config --ldflags" \
CC="$PREFIX/bin/$TARGET-gcc -static-libgcc" \
$SOURCES/$FREETYPE_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    --enable-freetype-config \
    >>$LOGFILES/part10_freetype_configure.log 2>>$LOGFILES/part10_freetype_configure_err.log  
echo -e -n "make | "
make $CPU >>$LOGFILES/part10_freetype_make.log 2>>$LOGFILES/part10_freetype_make_err.log   
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part10_freetype_make.log 2>>$LOGFILES/part10_freetype_make_err.log 
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $SDL_TTF_NAME:\e[30m configure | "
mkdir -p $BUILDS/build-$SDL_TTF_NAME
cd $BUILDS/build-$SDL_TTF_NAME
PKG_CONFIG_PATH="$PREFIX/$TARGET/lib/pkgconfig" \
PATH="$PREFIX/bin:$PATH" \
CFLAGS="-I$PREFIX/$TARGET/include" \
LDFLAGS="-L$PREFIX/$TARGET/lib"  \
CC="$PREFIX/bin/$TARGET-gcc -static-libgcc" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$SDL_TTF_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    --with-freetype-prefix=$PREFIX/$TARGET \
    >>$LOGFILES/part10_sdl_ttf_configure.log 2>>$LOGFILES/part10_sdl_ttf_configure_err.log  
echo -e -n "make | "
make $CPU >>$LOGFILES/part10_sdl_ttf_make.log 2>>$LOGFILES/part10_sdl_ttf_err.log   
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part10_sdl_ttf_make.log 2>>$LOGFILES/part10_sdl_ttf_err.log
cd $SOURCES

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit

########################################################################################

# PART 3: Downloads - Original
wget -nc $NDK_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $OPENURL_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $AMISSL_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $GUIGFX_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $RENDER_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $CODESETS_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 

# PART 9: Amiga NDK's (3.4.6) - Original
echo -e "\e[1m\e[37m5. AmigaOS NDK\e[0m\e[36m"
echo -e -n "\e[0m\e[36m   * amigaos ndk 3.9:\e[30m copy | " 
cp -r $SOURCES/$NDK39_NAME/m68k-amigaos/sys-include $PREFIX/$TARGET
echo -e -n "patch | "
for p in `ls $WORKSPACE/_install/recipes/patches/ndk/*.p`; do patch -d $PREFIX/$TARGET <$p -p0 >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log; done 
echo -e "customise\e[0m"
cp -r $WORKSPACE/_install/recipes/files/sys-include/* $PREFIX/$TARGET/sys-include/ >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cd $SOURCES

#AR="$PREFIX/bin/$TARGET-ar" \
#RANLIB="$PREFIX/bin/$TARGET-ranlib" \

