# ApolloCrossDev GCC-3.4.6 Install Script v0.9
# 
# Installation:
# 1. Enter Compilers/GCC-3.4.6 directory
# 2. Type "./GCC-3.4.6.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc346 into <mysource> 
# 3. Read make-gcc346 for compile instructions

EDITION=GNU-3.4.6
VERSION=0.9
CPU=-j1
GCCVERSION=3.4.6
CFLAGS_FOR_TARGET="-O2 -fomit-frame-pointer"

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-unknown-amigaos
export PATH=$PREFIX/bin:$PATH

GCC_AUTOCONF=autoconf2.64
GCC_AUTOHEADER=autoheader2.64
GCC_AUTORECONF=autoreconf2.64
GCC_AUTOM4TE=autom4te2.64

BINUTILS_VERSION=amigaos-binutils-2.14
BINUTILS_DOWNLOAD=https://github.com/adtools/$BINUTILS_VERSION
GCC_VERSION=gcc-3.4.6
GCC_DOWNLOAD=https://ftp.gnu.org/gnu/gcc/$GCC_VERSION/$GCC_VERSION.tar.gz
GMP_VERSION=gmp-4.3.2
GMP_DOWNLOAD=https://ftp.gnu.org/gnu/gmp/$GMP_VERSION.tar.gz
MPFR_VERSION=mpfr-2.4.2
MPFR_DOWNLOAD=http://www.mpfr.org/$MPFR_VERSION/$MPFR_VERSION.tar.gz
MPC_VERSION=mpc-0.8.2
MPC_DOWNLOAD=http://www.multiprecision.org/downloads/$MPC_VERSION.tar.gz
BISON_VERSION=bison-2.7.1
BISON_DOWNLOAD=https://ftp.gnu.org/gnu/bison/$BISON_VERSION.tar.gz
CLIB2_VERSION=clib2
CLIB2_DOWNLOAD=https://github.com/adtools/clib2
#LIBNIX_NAME=libnix
#LIBNIX_DOWNLOAD=https://github.com/adtools/libnix
#LIBNIX3_NAME=libnix
#LIBNIX3_DOWNLOAD=https://github.com/diegocr/libnix
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3
NDK_VERSION=3.9
NDK_ARCHIVE=ndk-$NDK_VERSION-includes.tar.bz2
NDK_DOWNLOAD=http://kas1e.mikendezign.com/zerohero_crosscompilers_backup/files/m68k-amigaos/$NDK_ARCHIVE
OPENURL_VERSION=7.16
OPENURL_ARCHIVE=OpenURL-$OPENURL_VERSION.lha
OPENURL_DOWNLOAD=https://github.com/jens-maus/libopenurl/releases/download/$OPENURL_VERSION/$OPENURL_ARCHIVE
AMISSL_VERSION=4.4
AMISSL_ARCHIVE=AmiSSL-$AMISSL_VERSION.lha 
AMISSL_DOWNLOAD=https://github.com/jens-maus/amissl/releases/download/$AMISSL_VERSION/$AMISSL_ARCHIVE
GUIGFX_ARCHIVE=guigfxlib.lha
GUIGFX_DOWNLOAD=http://neoscientists.org/~bifat/binarydistillery/$GUIGFX_ARCHIVE
RENDER_ARCHIVE=renderlib.lha
RENDER_DOWNLOAD=http://neoscientists.org/~bifat/binarydistillery/$RENDER_ARCHIVE
CODESETS_VERSION=6.20
CODESETS_ARCHIVE=codesets-$CODESETS_VERSION.lha
CODESETS_DOWNLOAD=https://github.com/jens-maus/libcodesets/releases/download/$CODESETS_VERSION/$CODESETS_ARCHIVE

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$EDITION\e[30m v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# PART 1: Clean the House
sudo echo -e "\e[1m\e[37m1. Prepare Installation\e[0m\e[36m"
echo "   * Clean the House" 
rm -f -r $PREFIX $LOGFILES $SOURCES
echo "   * Create Directories" 
mkdir -p $PREFIX $LOGFILES $SOURCES
#mkdir -p $PREFIX/$TARGET/libnix $PREFIX/$TARGET/libnix/lib

cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2_update_linux.log 2>>$LOGFILES/part2_update_linux_err.log
sudo apt -y install build-essential m4 gawk autoconf automake flex bison expect dejagnu texinfo lhasa git subversion \
     make wget libgmp-dev libmpfr-dev libmpc-dev gettext texinfo ncurses-dev rsync libreadline-dev rename \
    >>$LOGFILES/part2_update_linux.log 2>>$LOGFILES/part2_update_linux_err.log

# PART 3: Download Sources
echo -e "\e[1m\e[37m3. Download Sources\e[0m\e[36m"
echo "   * $BINUTILS_VERSION" 
git clone --progress $BINUTILS_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo "   * $GCC_VERSION" 
wget -nc $GCC_DOWNLOAD -a $LOGFILES/part3_sources.log
echo "   * $GMP_VERSION" 
wget -nc $GMP_DOWNLOAD -a $LOGFILES/part3_sources.log
echo "   * $MPFR_VERSION" 
wget -nc $MPFR_DOWNLOAD -a $LOGFILES/part3_sources.log
echo "   * $MPC_VERSION" 
wget -nc $MPC_DOWNLOAD -a $LOGFILES/part3_sources.log
echo "   * $BISON_VERSION" 
wget -nc $BISON_DOWNLOAD -a $LOGFILES/part3_sources.log
echo "   * clib2-$CLIB2_VERSION"
git clone --progress $CLIB2_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo "   * $LIBNIX_NAME"
git clone --progress $LIBNIX_DOWNLOAD 2>>$LOGFILES/part3_sources.log

# PART 4: Unpack Sources
echo -e "\e[1m\e[37m4. Unpack Sources\e[0m\e[36m"
for f in *.tar*; do tar xfk $f >>$LOGFILES/part4_unpack.log 2>>$LOGFILES/part4_unpack_err.log; done 

# Part 5: Compile BinUtils
echo -e "\e[1m\e[37m5. Compile $BINUTILS_VERSION"
echo -e "\e[0m\e[36m   * Patch Binutils\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/binutils/*.p`; do patch -d $WORKSPACE/_sources/$BINUTILS_VERSION <$p -p0 >>$LOGFILES/part5_binutils_patch.log 2>>$LOGFILES/part5_binutils_patch_err.log; done 
echo -e "\e[0m\e[36m   * Configure Binutils\e[0m"
mkdir -p build-binutils
cd build-binutils
CFLAGS="-m32" \
LDFLAGS="-m32" \
../$BINUTILS_VERSION/configure \
    --prefix=$PREFIX \
    --target=$TARGET \
    --disable-nls \
    --disable-werror \
    >>$LOGFILES/part5_binutils_configure.log 2>>$LOGFILES/part5_binutils_configure_err.log
echo -e "\e[0m\e[36m   * Build Binutils ($CPU)\e[0m"
make $CPU >>$LOGFILES/part5_binutils_make.log 2>>$LOGFILES/part5_binutils_make_err.log
echo -e "\e[0m\e[36m   * Install Binutils ($CPU)\e[0m"
make $CPU install >>$LOGFILES/part5_binutils_make.log 2>>$LOGFILES/part5_binutils_make_err.log
cd $SOURCES

# Part 6: Compile Bison
echo -e "\e[1m\e[37m5. Compile $BISON_VERSION\e[0m\e[36m"
echo -e "\e[0m\e[36m   * Patch Bison\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/bison/*.p`; do patch -d $WORKSPACE/_sources/$BISON_VERSION <$p -p0 >>$LOGFILES/part6_bison_patch.log 2>>$LOGFILES/part6_bison_patch_err.log; done 
echo -e "\e[0m\e[36m   * Configure Bison ($CPU)\e[0m"
mkdir -p build-bison
cd build-bison
../$BISON_VERSION/configure \
    --prefix="$PREFIX" >>$LOGFILES/part6_bison_configure.log 2>>$LOGFILES/part6_bison_configure_err.log
echo -e "\e[0m\e[36m   * Build Bison ($CPU)\e[0m"
make $CPU >>$LOGFILES/part6_bison_make.log 2>>$LOGFILES/part6_bison_make_err.log
echo -e "\e[0m\e[36m   * Install Bison ($CPU)\e[0m"
make $CPU install >>$LOGFILES/part6_bison_make.log 2>>$LOGFILES/part6_bison_make_err.log
cd $SOURCES

# Part 7 Prepare GCC
echo -e "\e[1m\e[37m7. Prepare $GCC_VERSION\e[0m\e[36m"
echo "   * Move $GMP_VERSION to $GCC_VERSION/gmp"
mv $GMP_VERSION $GCC_VERSION/gmp >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo "   * Move $MPFR_VERSION to $GCC_VERSION/mpfr"
mv $MPFR_VERSION $GCC_VERSION/mpfr >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo "   * Move $MPC_VERSION to $GCC_VERSION/mpc"
mv $MPC_VERSION $GCC_VERSION/mpc >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo -e "\e[0m\e[36m   * Patch GCC\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/gcc/*.p`; do patch -d $WORKSPACE/_sources/$GCC_VERSION <$p -p0 >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log; done 
patch $WORKSPACE/_sources/$GCC_VERSION/gcc/collect2.c $WORKSPACE/_install/recipes/patches.wd/collect2.c.p >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo -e "\e[0m\e[36m   * Customise GCC\e[0m"
cp -r $WORKSPACE/_install/recipes/files/gcc/* $GCC_VERSION >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log

# Part 8: Compile GCC (Phase #1)
echo -e "\e[1m\e[37m8. Compile $GCC_VERSION (Phase #1)"
mkdir -p build-gcc
cd build-gcc
echo -e "\e[0m\e[36m   * Configure GCC\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" ../$GCC_VERSION/configure \
	--prefix=$PREFIX \
	--target=$TARGET \
	--disable-threads \
	--disable-nls --disable-c-mbchar \
	--enable-languages=c --enable-checking=no \
	--enable-c99 --with-cross-host \
	--disable-multilib --without-x \
	--enable-maintainer-mode --disable-shared \
	--without-headers \
    >>$LOGFILES/part8_gcc_configure.log 2>>$LOGFILES/part8_gcc_configure_err.log 
echo -e "\e[0m\e[36m   * Build GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 all-gcc >>$LOGFILES/part8_gcc_make.log 2>>$LOGFILES/part8_gcc_make_err.log
echo -e "\e[0m\e[36m   * Install GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 install-gcc >>$LOGFILES/part8_gcc_make.log 2>>$LOGFILES/part8_gcc_make_err.log
cd $SOURCES

# PART 9: Amiga NDK's
echo -e "\e[1m\e[37m9. Amiga NDK's"
mkdir -p $PREFIX/$TARGET
echo -e "\e[0m\e[36m   * NDK 3.9\e[0m"
wget -nc $NDK_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
tar -C $PREFIX/$TARGET --strip-components=2 -xjf $NDK_ARCHIVE
echo -e "\e[0m\e[36m   * Patch NDK 3.9\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/ndk/*.p`; do patch -d $PREFIX/$TARGET <$p -p0 >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log; done 
echo -e "\e[0m\e[36m   * Customise NDK 3.9\e[0m"
cp -r $WORKSPACE/_install/recipes/files/ndk/sys-include/* $PREFIX/$TARGET/sys-include/ >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cd $SOURCES
echo -e "\e[0m\e[36m   * $OPENURL_ARCHIVE\e[0m"
wget -nc $OPENURL_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
lha -xw=OpenURL $OPENURL_ARCHIVE >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cp -r OpenURL/OpenURL/Developer/C/include/* $PREFIX/$TARGET/sys-include/
echo -e "\e[0m\e[36m   * $AMISSL_ARCHIVE\e[0m"
wget -nc $AMISSL_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
lha xw=AmiSSL $AMISSL_ARCHIVE >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cp -r AmiSSL/AmiSSL/Developer/include/* $PREFIX/$TARGET/sys-include/
cp -r AmiSSL/AmiSSL/Developer/lib/AmigaOS3/* $PREFIX/$TARGET/lib/
echo -e "\e[0m\e[36m   * $GUIGFX_ARCHIVE\e[0m"
wget -nc $GUIGFX_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
lha xw=guigfxlib $GUIGFX_ARCHIVE >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cp -r guigfxlib/include/* $PREFIX/$TARGET/sys-include/
echo -e "\e[0m\e[36m   * $RENDER_ARCHIVE\e[0m"
wget -nc $RENDER_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
lha xw=renderlib $RENDER_ARCHIVE >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cp -r renderlib/renderlib/include/* $PREFIX/$TARGET/sys-include/
echo -e "\e[0m\e[36m   * $CODESETS_ARCHIVE\e[0m"
wget -nc $CODESETS_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
lha xw=codesets $CODESETS_ARCHIVE >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cp -r codesets/codesets/Developer/include/* $PREFIX/$TARGET/sys-include/
cd $SOURCES

# PART 10: Amiga Libs/Includes
echo -e "\e[1m\e[37m10. Amiga Libraries"
echo -e "\e[0m\e[36m   * Configure clib2\e[0m"
mkdir -p build-clib2
cd build-clib2
cp -r $WORKSPACE/_sources/clib2/library/* $SOURCES/build-clib2
echo -e "\e[0m\e[36m   * Patch clib2\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/clib2/*.p`; do patch -d $SOURCES/build-clib2 <$p -p0 >>$LOGFILES/part10_clib2_patch.log 2>>$LOGFILES/part10_clib2_patch_err.log; done 
echo -e "\e[0m\e[36m   * Customise clib2\e[0m"
cp -r $WORKSPACE/_install/recipes/files/clib2/* $SOURCES/build-clib2 >>$LOGFILES/part10_clib2_patch.log 2>>$LOGFILES/part10_clib2_patch_err.log
echo -e "\e[0m\e[36m   * Build clib2 ($CPU)\e[0m"
PATH=$PREFIX/bin:$PATH make -f GNUmakefile.68k >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
cp -r $SOURCES/build-clib2/include $PREFIX/$TARGET >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
cp -r $SOURCES/build-clib2/lib $PREFIX/$TARGET >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
ln -sf $PREFIX/$TARGET/lib/ncrt0.o $PREFIX/$TARGET/lib/crt0.o >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
cd $SOURCES

# Part 11: Compile GCC (Phase #2)
echo -e "\e[1m\e[37m11. Compile $GCC_VERSION (Phase #2)"
cd build-gcc
echo -e "\e[0m\e[36m   * Build GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 all-gcc >>$LOGFILES/part11_gcc_make.log 2>>$LOGFILES/part11_gcc_make_err.log
echo -e "\e[0m\e[36m   * Install GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" make -j1 install-gcc >>$LOGFILES/part11_gcc_make.log 2>>$LOGFILES/part11_gcc_make_err.log
cd $SOURCES

# PART 12: Cleanup
echo -e "\e[1m\e[37m12. Cleanup\e[0m\e[36m"
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit



########################################################################################


#Scrapbook


#LIBNIX
mkdir -p $PREFIX/$TARGET/libnix $PREFIX/$TARGET/libnix/lib

echo -e "\e[1m\e[37m12. Amiga Libraries"
echo -e -n "\e[0m\e[36m   * libnix:\e[30m configure | "
mkdir -p build-libnix
cd build-libnix
CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
$SOURCES/$LIBNIX_NAME/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/libnix_configure.log 2>>$LOGFILES/libnix_configure_err.log   
echo -e -n "make | "
CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
make -j1 >>$LOGFILES/libnix_make.log 2>>$LOGFILES/libnix_make_err.log
echo -e "install\e[0m"
make -j1 install >>$LOGFILES/libnix_install.log 2>>$LOGFILES/libnix_install_err.log
cp -r $SOURCES/$LIBNIX_NAME/sources/headers/stabs.h $PREFIX/$TARGET/include
cd $SOURCES



# PART 9: Amiga NDK's
echo -e "\e[1m\e[37m9. Amiga NDK's"
mkdir NDK3.2
cd NDK3.2
echo -e "\e[0m\e[36m   * NDK 3.2\e[0m"
wget -nc $NDK32_DOWNLOAD -a $LOGFILES/part9.log
lha -xw=$PREFIX/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log
cd $SOURCES

mkdir -p $PREFIX/$TARGET
echo -e "\e[0m\e[36m   * NDK 3.9\e[0m"
wget -nc $NDK_DOWNLOAD -a $LOGFILES/part9.log 
tar -C $PREFIX/$TARGET --strip-components=2 -xjf $NDK_ARCHIVE
echo -e "\e[0m\e[36m   * Patch NDK 3.9\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/ndk/*.p`; do patch -d $PREFIX/$TARGET <$p -p0 >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log; done 
echo -e "\e[0m\e[36m   * Customise NDK 3.9\e[0m"
cp -r $WORKSPACE/_install/recipes/files/ndk/* $PREFIX/$TARGET >>$LOGFILES/part9.log 2>>$LOGFILES/part9_err.log




#	--disable-threads \
#	--disable-nls --disable-c-mbchar \
#	--enable-languages=c --enable-checking=no \
#	--enable-c99 --with-cross-host \
#   --without-x \
#	--enable-maintainer-mode --disable-shared \
#	--without-headers \
#	--disable-multilib \ 

echo -e -n "\e[0m\e[36m   * libnix:\e[30m configure | "
mkdir -p build-libnix
cd build-libnix
CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
$SOURCES/$LIBNIX_NAME/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/libnix_configure.log 2>>$LOGFILES/libnix_configure_err.log   
echo -e -n "make | "
CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
make -j1 >>$LOGFILES/libnix_make.log 2>>$LOGFILES/libnix_make_err.log
echo -e "install\e[0m"
make -j1 install >>$LOGFILES/libnix_install.log 2>>$LOGFILES/libnix_install_err.log
cp -r $SOURCES/$LIBNIX_NAME/sources/headers/stabs.h $PREFIX/$TARGET/libnix/include
cd $SOURCES



CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
CFLAGS="-Wall -m68020-60 -O2 -msoft-float -funroll-loops -fomit-frame-pointer -noixemul -I../headers -g"
$SOURCES/$LIBNIX_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/libnix_configure.log 2>>$LOGFILES/libnix_configure_err.log  

# Clib2

