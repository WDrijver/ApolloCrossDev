# ApolloCrossDev GCC-6.50 - Install Script v0.6
# 
# Installation:
# 1. Enter Compilers/GCC-6.5.0 directory
# 2. Type "./GCC-6.5.0.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc650 into <mysource> 
# 3. Read make-gcc650 for compile instructions

EDITION=GCC-6.50
VERSION=0.6
CPU=-j16

WORKSPACE="`pwd`"
ARCHIVES=$WORKSPACE/_archives
SOURCES=$WORKSPACE/_sources
BUILDS=$WORKSPACE/_builds
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

#SDL-Framework for ApolloCrossDev
LIBDSL_AMIGA68K_NAME=libSDL12_Amiga68k-master
LIBOGG_NAME=libogg-1.3.5
LIBVORBIS_NAME=libvorbis-1.3.7
LIBTHEORA_NAME=libtheora-1.1.1
LIBFREETYPE_NAME=freetype-2.13.0
LIBSDL_TTF_NAME=SDL_ttf-2.0.11

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$EDITION\e[30m v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
sudo echo -e "\e[1m\e[37m1. Sudo Password\e[0m\e[36m"
rm -rf $LOGFILES/* $BUILDS/* $SOURCES/*

# PART 3: Unpack Archives
cd $ARCHIVES
echo -e "\e[1m\e[37m3. Unpack Source Archives\e[0m\e[36m"
echo -e -n "\e[0m\e[36m   * tar | " 
for f in *.tar*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done
echo -e -n "tgz | " 
for f in *.tgz*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done 
cd $SOURCES

# PART 11: SDL Framework
echo -e "\e[1m\e[37m10. SDL Development Library\e[0m\e[36m"

echo -e -n "\e[0m\e[36m   * $LIBDSL_AMIGA68K_NAME:\e[30m configure | "
mkdir -p $BUILDS/build-$LIBDSL_AMIGA68K_NAME
cd $BUILDS/build-$LIBDSL_AMIGA68K_NAME
cp -rf $SOURCES/$LIBDSL_AMIGA68K_NAME/* $BUILDS/build-$LIBDSL_AMIGA68K_NAME >>$LOGFILES/part10_sdl_prepare.log 2>>$LOGFILES/part10_sdl_prepare_err.log
echo -e -n "make | "
make $CPU clean >>$LOGFILES/part10_sdl_make.log 2>>$LOGFILES/part10_sdl_make_err.log
make $CPU >>$LOGFILES/part10_sdl_make.log 2>>$LOGFILES/part10_sdl_make_err.log
echo -e "install\e[0m"
cp libSDL*.a $PREFIX/$TARGET/lib >>$LOGFILES/part10_sdl_install.log 2>>$LOGFILES/part10_sdl_install_err.log
mkdir -p $PREFIX/$TARGET/include/SDL
cp include/SDL/* $PREFIX/$TARGET/include/SDL >>$LOGFILES/part10_sdl_install.log 2>>$LOGFILES/part10_sdl_install_err.log
cp -rf $WORKSPACE/_install/recipes/files.wd/SDL/bin/sdl-config $PREFIX/bin >>$LOGFILES/part10_sdl_install.log 2>>$LOGFILES/part10_sdl_install_err.log
chmod 744 $PREFIX/bin/sdl-config >>$LOGFILES/part10_sdl_install.log 2>>$LOGFILES/part10_sdl_install_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $LIBFREETYPE_NAME:\e[30m patch | "
cp -rf $WORKSPACE/_install/recipes/files.wd/freetype/builds/unix/* $SOURCES/$LIBFREETYPE_NAME/builds/unix >>$LOGFILES/part10_freetype_patch.log 2>>$LOGFILES/part10_freetype_patch_err.log
echo -e -n "configure | "
mkdir -p $BUILDS/build-$LIBFREETYPE_NAME
cd $BUILDS/build-$LIBFREETYPE_NAME
PATH="$PREFIX/bin:$PATH" \
CFLAGS="-I$PREFIX/$TARGET/include -O2 -fomit-frame-pointer -m68040 -m68881 -ffast-math -noixemul" \
LDFLAGS="-L$PREFIX/$TARGET/lib"  \
LIBPNG="libpng-config --libs" \
LIBPNG_CFLAGS="libpng-config --cflags" \
LIBPNG_LDFLAGS="libpng-config --ldflags" \
CC="$PREFIX/bin/$TARGET-gcc -static-libgcc" \
$SOURCES/$LIBFREETYPE_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    --enable-freetype-config \
    >>$LOGFILES/part10_freetype_configure.log 2>>$LOGFILES/part10_freetype_configure_err.log  
echo -e -n "make | "
make $CPU >>$LOGFILES/part10_freetype_make.log 2>>$LOGFILES/part10_freetype_make_err.log   
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part10_freetype_make.log 2>>$LOGFILES/part10_freetype_install_err.log 
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $LIBSDL_TTF_NAME:\e[30m configure | "
mkdir -p $BUILDS/build-$LIBSDL_TTF_NAME
cd $BUILDS/build-$LIBSDL_TTF_NAME
export LIBS="-L$PREFIX/$TARGET/lib -lm -lSDL -lSDL_Apollo -ldebug"
PATH="$PREFIX/bin:$PATH" \
PKG_CONFIG_PATH="$PREFIX/$TARGET/lib/pkgconfig" \
SDL_CONFIG="$PREFIX/bin/sdl-config" \
FREETYPE_CONFIG="$PREFIX/$TARGET/bin/freetype-config" \
CFLAGS="-O2 -fomit-frame-pointer -m68040 -m68881 -ffast-math -noixemul -I$PREFIX/$TARGET/include/SDL" \
DEFS="-D_HAVE_STDINT_H" \
LDFLAGS="-L$PREFIX/$TARGET/lib" \
CC="$PREFIX/bin/$TARGET-gcc -static-libgcc" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$LIBSDL_TTF_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part10_sdl_ttf_configure.log 2>>$LOGFILES/part10_sdl_ttf_configure_err.log  
echo -e -n "make | "
make $CPU >>$LOGFILES/part10_sdl_ttf_make.log 2>>$LOGFILES/part10_sdl_ttf_make_err.log   
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part10_sdl_ttf_make.log 2>>$LOGFILES/part10_sdl_ttf_make_err.log
cp -rf $SOURCES/$LIBSDL_TTF_NAME/SDL_ttf.h $PREFIX/$TARGET/include/SDL
cd $SOURCES

# PART 5: Cleanup
echo -e "\e[1m\e[37m5. Cleanup\e[0m\e[36m"
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit
