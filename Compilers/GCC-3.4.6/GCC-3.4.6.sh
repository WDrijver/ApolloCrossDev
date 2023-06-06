# ApolloCrossDev GCC-3.4.6 Install Script v1.1
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
VERSION=1.1
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
CLIB2_NAME=clib2
CLIB2_DOWNLOAD=https://github.com/adtools/clib2
NDK32_DOWNLOAD=http://aminet.net/dev/misc/NDK3.2.lha
NDK39_DOWNLOAD=https://os.amigaworld.de/download.php?id=3
NDK39_NAME=amiga
NDK_NAME=3.9
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
IXEMUL_NAME=ixemul-48.2
IXEMUL_DOWNLOAD=http://downloads.sf.net/project/amiga/ixemul.library/48.2/ixemul-src.lha
IXEMUL_ARCHIVE=ixemul-src.lha

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev \e[36m$EDITION\e[30m v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# PART 1: Clean the House
sudo echo -e "\e[1m\e[37m1. Prepare Installation\e[0m\e[36m"
echo "   * Clean the House" 
rm -f -r $SOURCES $BUILDS $LOGFILES $PREFIX
echo "   * Create Directories" 
mkdir -p $SOURCES $BUILDS $LOGFILES $PREFIX $PREFIX/$TARGET
mkdir -p $PREFIX/$TARGET/sys-include

cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2_update_linux.log 2>>$LOGFILES/part2_update_linux_err.log
sudo apt -y install build-essential m4 gawk autoconf automake flex bison expect dejagnu texinfo lhasa git subversion \
     make wget libgmp-dev libmpfr-dev libmpc-dev gettext texinfo ncurses-dev rsync libreadline-dev rename gperf gcc-multilib \
     autoconf2.64 \
     >>$LOGFILES/part2_linux_updates.log 2>>$LOGFILES/part2_linux_updates_err.log

# PART 3: Unpack Archives
cd $ARCHIVES
echo -e "\e[1m\e[37m3. Unpack Source Archives\e[0m\e[36m"
for f in *.tar*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done 
lha -xw=$SOURCES/OpenURL $OPENURL_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/AmiSSL $AMISSL_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/guigfxlib $GUIGFX_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/renderlib $RENDER_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/codesets $CODESETS_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
cd $SOURCES

# PART 4: Tools
echo -e "\e[1m\e[37m4. Install Tools\e[0m\e[36m"

# Part 5: Compile BinUtils
echo -e "\e[1m\e[37m5. Compile $BINUTILS_NAME"
echo -e "\e[0m\e[36m   * Patch Binutils\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/binutils/*.p`; do patch -d $SOURCES/$BINUTILS_NAME <$p -p0 >>$LOGFILES/part5_binutils_patch.log 2>>$LOGFILES/part5_binutils_patch_err.log; done 
echo -e "\e[0m\e[36m   * Configure Binutils\e[0m"
mkdir -p $BUILDS/build-$BINUTILS_NAME
cd $BUILDS/build-$BINUTILS_NAME
CFLAGS="-m32" LDFLAGS="-m32" \
$SOURCES/$BINUTILS_NAME/configure \
    --prefix=$PREFIX \
    --target=$TARGET \
    --disable-nls \
    --disable-werror \
    >>$LOGFILES/part5_binutils_configure.log 2>>$LOGFILES/part5_binutils_configure_err.log
echo -e "\e[0m\e[36m   * Build Binutils\e[0m"
make $CPU >>$LOGFILES/part5_binutils_make.log 2>>$LOGFILES/part5_binutils_make_err.log
echo -e "\e[0m\e[36m   * Install Binutils\e[0m"
make $CPU install >>$LOGFILES/part5_binutils_make.log 2>>$LOGFILES/part5_binutils_make_err.log
cd $SOURCES

# Part 6: Compile Bison
echo -e "\e[1m\e[37m5. Compile $BISON_NAME\e[0m\e[36m"
echo -e "\e[0m\e[36m   * Patch Bison\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/bison/*.p`; do patch -d $WORKSPACE/_sources/$BISON_NAME <$p -p0 >>$LOGFILES/part6_bison_patch.log 2>>$LOGFILES/part6_bison_patch_err.log; done 
echo -e "\e[0m\e[36m   * Configure Bisons\e[0m"
mkdir -p $BUILDS/build-$BISON_NAME
cd $BUILDS/build-$BISON_NAME
$SOURCES/$BISON_NAME/configure \
    --prefix="$PREFIX" >>$LOGFILES/part6_bison_configure.log 2>>$LOGFILES/part6_bison_configure_err.log
echo -e "\e[0m\e[36m   * Build Bisons\e[0m"
make $CPU >>$LOGFILES/part6_bison_make.log 2>>$LOGFILES/part6_bison_make_err.log
echo -e "\e[0m\e[36m   * Install Bison\e[0m"
make $CPU install >>$LOGFILES/part6_bison_make.log 2>>$LOGFILES/part6_bison_make_err.log
cd $SOURCES

# Part 7 Prepare GCC
echo -e "\e[1m\e[37m7. Prepare $GCC_NAME\e[0m\e[36m"
echo "   * Move $GMP_NAME to $GCC_NAME/gmp"
mv $GMP_NAME $GCC_NAME/gmp >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo "   * Move $MPFR_NAME to $GCC_NAME/mpfr"
mv $MPFR_NAME $GCC_NAME/mpfr >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo "   * Move $MPC_NAME to $GCC_NAME/mpc"
mv $MPC_NAME $GCC_NAME/mpc >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
echo -e "\e[0m\e[36m   * Patch GCC\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/gcc/*.p`; do patch -d $WORKSPACE/_sources/$GCC_NAME <$p -p0 >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log; done 
echo -e "\e[0m\e[36m   * Customise GCC\e[0m"
cp -r $WORKSPACE/_install/recipes/files/gcc/* $GCC_NAME >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log
cp -r $WORKSPACE/_install/recipes/files.wd/gcc/* $GCC_NAME >>$LOGFILES/part7_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log

# Part 8: Compile GCC (Phase #1)
echo -e "\e[1m\e[37m8. Compile $GCC_NAME (Phase #1)"
mkdir -p $BUILDS/build-$GCC_NAME
cd $BUILDS/build-$GCC_NAME
echo -e "\e[0m\e[36m   * Configure GCC\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
$SOURCES/$GCC_NAME/configure \
	--prefix=$PREFIX \
	--target=$TARGET \
	--disable-threads \
	--disable-nls --disable-c-mbchar \
	--enable-languages=c --enable-checking=no \
	--enable-c99 --with-cross-host \
    --without-x --enable-multilib \
	--enable-maintainer-mode --disable-shared \
    --without-headers \
    >>$LOGFILES/part8_gcc_configure.log 2>>$LOGFILES/part8_gcc_configure_err.log 
echo -e "\e[0m\e[36m   * Build GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 all-gcc >>$LOGFILES/part8_gcc_make.log 2>>$LOGFILES/part8_gcc_make_err.log
echo -e "\e[0m\e[36m   * Install GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 install-gcc >>$LOGFILES/part8_gcc_make.log 2>>$LOGFILES/part8_gcc_make_err.log
cd $SOURCES

# PART 9: Amiga NDK's
echo -e "\e[1m\e[37m9. Amiga NDK's"

echo -e "\e[0m\e[36m   * NDK 3.9\e[0m"
cp -r $SOURCES/$NDK39_NAME/m68k-amigaos/sys-include $PREFIX/$TARGET
echo -e "\e[0m\e[36m   * Patch NDK 3.9\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/ndk/*.p`; do patch -d $PREFIX/$TARGET <$p -p0 >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log; done 
echo -e "\e[0m\e[36m   * Customise NDK 3.9\e[0m"
cp -r $WORKSPACE/_install/recipes/files/ndk/sys-include/* $PREFIX/$TARGET/sys-include/ >>$LOGFILES/part9_NDK_Amiga.log 2>>$LOGFILES/part9_NDK_Amiga_err.log
cd $SOURCES

echo -e "\e[0m\e[36m   * $OPENURL_ARCHIVE\e[0m"
cp -r OpenURL/OpenURL/Developer/C/include/* $PREFIX/$TARGET/sys-include/

echo -e "\e[0m\e[36m   * $AMISSL_ARCHIVE\e[0m"
cp -r AmiSSL/AmiSSL/Developer/include/* $PREFIX/$TARGET/sys-include/
cp -r AmiSSL/AmiSSL/Developer/lib/AmigaOS3/* $PREFIX/$TARGET/lib/

echo -e "\e[0m\e[36m   * $GUIGFX_ARCHIVE\e[0m"
cp -r guigfxlib/include/* $PREFIX/$TARGET/sys-include/

echo -e "\e[0m\e[36m   * $RENDER_ARCHIVE\e[0m"
cp -r renderlib/renderlib/include/* $PREFIX/$TARGET/sys-include/

echo -e "\e[0m\e[36m   * $CODESETS_ARCHIVE\e[0m"
cp -r codesets/codesets/Developer/include/* $PREFIX/$TARGET/sys-include/
cd $SOURCES

# PART 10: Amiga Libraries: CLib2
echo -e "\e[1m\e[37m10. Amiga Libraries"
echo -e "\e[0m\e[36m   * Configure clib2\e[0m"
mkdir -p $BUILDS/build-$CLIB2_NAME
cd $BUILDS/build-$CLIB2_NAME
cp -r $SOURCES/clib2/library/* $BUILDS/build-$CLIB2_NAME
echo -e "\e[0m\e[36m   * Patch clib2\e[0m"
for p in `ls $WORKSPACE/_install/recipes/patches/clib2/*.p`; do patch -d $BUILDS/build-$CLIB2_NAME <$p -p0 >>$LOGFILES/part10_clib2_patch.log 2>>$LOGFILES/part10_clib2_patch_err.log; done 
echo -e "\e[0m\e[36m   * Customise clib2\e[0m"
cp -r $WORKSPACE/_install/recipes/files/clib2/* $BUILDS/build-$CLIB2_NAME >>$LOGFILES/part10_clib2_patch.log 2>>$LOGFILES/part10_clib2_patch_err.log
echo -e "\e[0m\e[36m   * Build clib2\e[0m"
PATH=$PREFIX/bin:$PATH make -f GNUmakefile.68k >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
cp -r $BUILDS/build-$CLIB2_NAME/include/* $PREFIX/$TARGET/sys-include >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
cp -r $BUILDS/build-$CLIB2_NAME/lib $PREFIX/$TARGET >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
ln -sf $PREFIX/$TARGET/lib/ncrt0.o $PREFIX/$TARGET/lib/crt0.o >>$LOGFILES/part10_clib2_make.log 2>>$LOGFILES/part10_clib2_make_err.log
cd $SOURCES

# Part 11: Compile GCC (Phase #2)
echo -e "\e[1m\e[37m11. Compile $GCC_NAME (Phase #2)"
cd $BUILDS/build-$GCC_NAME
echo -e "\e[0m\e[36m   * Build GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 all-target-libiberty >>$LOGFILES/part11_gcc_make.log 2>>$LOGFILES/part11_gcc_make_err.log
echo -e "\e[0m\e[36m   * Install GCC (1 CPU)\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 install-target-libiberty >>$LOGFILES/part11_gcc_make.log 2>>$LOGFILES/part11_gcc_make_err.log
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

wget -nc $NDK_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $OPENURL_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $AMISSL_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $GUIGFX_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $RENDER_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
wget -nc $CODESETS_DOWNLOAD -a $LOGFILES/part9_NDK_Amiga.log 
