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
SDL_TTF_NAME=SDL2_ttf-2.20.2
FREETYPE_NAME=freetype-master

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
mkdir -p $PREFIX/bin $PREFIX/etc $PREFIX/$TARGET/bin $PREFIX/$TARGET/sys-include
mkdir -p $PREFIX/$TARGET/include $PREFIX/$TARGET/lib $PREFIX/$TARGET/include/inline
mkdir -p $PREFIX/$TARGET/include/lvo  $PREFIX/$TARGET/lib/fd $PREFIX/$TARGET/lib/sfd
mkdir -p $PREFIX/$TARGET/clib2 $PREFIX/$TARGET/clib2/include $PREFIX/$TARGET/clib2/lib
mkdir -p $PREFIX/$TARGET/libnix $PREFIX/$TARGET/libnix/include $PREFIX/$TARGET/libnix/lib

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
echo -e "\e[36m   * On first run:\e[30m please be patient"
sudo apt -y update >>$LOGFILES/part2_update_linux.log 2>>$LOGFILES/part2_update_linux_err.log
sudo apt -y install build-essential m4 gawk autoconf automake flex bison expect dejagnu texinfo lhasa git subversion \
     make wget libgmp-dev libmpfr-dev libmpc-dev gettext texinfo ncurses-dev rsync libreadline-dev rename gperf gcc-multilib \
     autoconf2.64 \
     >>$LOGFILES/part2_linux_updates.log 2>>$LOGFILES/part2_linux_updates_err.log

# PART 3: Unpack Archives
cd $ARCHIVES
echo -e "\e[1m\e[37m3. Unpack Source Archives\e[0m\e[36m"
echo -e -n "\e[0m\e[36m   * tar | " 
for f in *.tar*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done 

echo -e -n "tgz | " 
for f in *.tgz*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done 
echo -e "lha\e[0m" 
lha -xw=$SOURCES $IXEMUL_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES $NDK39_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/OpenURL $OPENURL_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/AmiSSL $AMISSL_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/guigfxlib $GUIGFX_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/renderlib $RENDER_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES/codesets $CODESETS_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
cd $SOURCES

# PART 4: Tools
echo -e "\e[1m\e[37m4. Install Tools\e[0m\e[36m"
echo -e -n "\e[0m\e[36m   * bison:\e[30m patch | " 
for p in `ls $WORKSPACE/_install/recipes/patches/bison/*.p`; do patch -d $WORKSPACE/_sources/$BISON_NAME <$p -p0 >>$LOGFILES/part6_bison_patch.log 2>>$LOGFILES/part6_bison_patch_err.log; done 
echo -e -n "configure | "
mkdir -p $BUILDS/build-$BISON_NAME
cd $BUILDS/build-$BISON_NAME
$SOURCES/$BISON_NAME/configure \
    --prefix="$PREFIX" >>$LOGFILES/part6_bison_configure.log 2>>$LOGFILES/part6_bison_configure_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part6_bison_make.log 2>>$LOGFILES/part6_bison_make_err.log
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part6_bison_make.log 2>>$LOGFILES/part6_bison_make_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $FD2SFD_NAME:\e[30m configure | " 
mkdir -p $BUILDS/build-$FD2SFD_NAME
cd $BUILDS/build-$FD2SFD_NAME
$SOURCES/$FD2SFD_NAME/configure \
    --prefix=$PREFIX \
    >>$LOGFILES/part4_tools_fd2sfd.log 2>>$LOGFILES/part4_tools_fd2sfd_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part4_tools_fd2sfd.log 2>>$LOGFILES/part4_tools_fd2sfd_err.log
echo -e "install\e[0m"
cp fd2sfd $PREFIX/bin >>$LOGFILES/part4_tools_fd2sfd.log 2>>$LOGFILES/part4_tools_fd2sfd_err.log
cp $SOURCES/$FD2SFD_NAME/cross/share/m68k-amigaos/alib.h $PREFIX/$TARGET/include/inline >>$LOGFILES/part4_tools_fd2sfd.log 2>>$LOGFILES/part4_tools_fd2sfd_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $FD2PRAGMA_NAME:\e[30m configure | " 
cp -r $SOURCES/$FD2PRAGMA_NAME $BUILDS/build-$FD2PRAGMA_NAME
cd $BUILDS/build-$FD2PRAGMA_NAME
    >>$LOGFILES/part4_tools_fd2pragma.log 2>>$LOGFILES/part4_tools_fd2pragma_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part4_tools_fd2pragma.log 2>>$LOGFILES/part4_tools_fd2pragma_err.log
echo -e "install\e[0m"
cp fd2pragma $PREFIX/bin 
cp Include/inline/* $PREFIX/$TARGET/include/inline 
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $SFDC_NAME:\e[30m configure | " 
cp -r $SOURCES/$SFDC_NAME $BUILDS/build-$SFDC_NAME
cd $BUILDS/build-$SFDC_NAME
./configure \
    --prefix=$PREFIX \
    >>$LOGFILES/part4_tools_sfdc.log 2>>$LOGFILES/part4_tools_sfdc_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part4_tools_sfdc.log 2>>$LOGFILES/part4_tools_sfdc_err.log
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part4_tools_sfdc.log 2>>$LOGFILES/part4_tools_sfdc_err.log
cd $SOURCES

# Part 5: Compile BinUtils
echo -e "\e[1m\e[37m5. Compile Binutils"
echo -e -n "\e[0m\e[36m   * binutils:\e[30m patch | " 
for p in `ls $WORKSPACE/_install/recipes/patches/binutils/*.p`; do patch -d $SOURCES/$BINUTILS_NAME <$p -p0 >>$LOGFILES/part5_binutils_patch.log 2>>$LOGFILES/part5_binutils_patch_err.log; done 
echo -e -n "configure | "
mkdir -p $BUILDS/build-$BINUTILS_NAME
cd $BUILDS/build-$BINUTILS_NAME
CFLAGS="-m32" LDFLAGS="-m32" \
$SOURCES/$BINUTILS_NAME/configure \
    --prefix=$PREFIX \
    --target=$TARGET \
    --disable-nls \
    --disable-werror \
    >>$LOGFILES/part5_binutils_configure.log 2>>$LOGFILES/part5_binutils_configure_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part5_binutils_make.log 2>>$LOGFILES/part5_binutils_make_err.log
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part5_binutils_make.log 2>>$LOGFILES/part5_binutils_make_err.log
cd $SOURCES

# Part 6 Compile GCC Compiler
echo -e "\e[1m\e[37m6. Compile GCC (Compiler)\e[0m"

mv ixemul $IXEMUL_NAME
echo -e -n "\e[0m\e[36m   * gcc:\e[30m patch | " 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done  
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/general/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/general <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/glue/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/glue <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/ixnet/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/ixnet <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/library/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/library <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/stdio/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdio <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/stdlib/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdlib <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/string/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/string <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$IXEMUL_NAME/utils/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/utils <$p >>$LOGFILES/part6_ixemul_patch.log 2>>$LOGFILES/part6_ixemul_patch_err.log; done
cp -f $WORKSPACE/_install/recipes/files.wd/ixemul/* $SOURCES/$IXEMUL_NAME/include >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part6_prepare_gcc_err.log
for p in `ls $WORKSPACE/_install/recipes/patches/gcc/*.p`; do patch -d $WORKSPACE/_sources/$GCC_NAME <$p -p0 >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part7_prepare_gcc_err.log; done 
echo -e -n "customise | "
cp -r $WORKSPACE/_install/recipes/files/gcc/* $GCC_NAME >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part6_prepare_gcc_err.log
cp -r $WORKSPACE/_install/recipes/files.wd/gcc/* $GCC_NAME >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part6_prepare_gcc_err.log

echo -e -n "gmp | "
mv $GMP_NAME $GCC_NAME/gmp >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part6_prepare_gcc_err.log
echo -e -n "mpfr | "
mv $MPFR_NAME $GCC_NAME/mpfr >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part6_prepare_gcc_err.log
echo -e -n "mpc | "
mv $MPC_NAME $GCC_NAME/mpc >>$LOGFILES/part6_prepare_gcc.log 2>>$LOGFILES/part6_prepare_gcc_err.log
mkdir -p $BUILDS/build-$GCC_NAME
cd $BUILDS/build-$GCC_NAME
echo -e -n "configure | "
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
$SOURCES/$GCC_NAME/configure \
	--prefix=$PREFIX \
	--target=$TARGET \
	--disable-threads \
	--disable-nls --disable-c-mbchar \
	--enable-languages=c,c++ --enable-checking=no \
	--enable-c99 --with-cross-host \
    --without-x --enable-multilib \
	--enable-maintainer-mode \
    --with-headers=$SOURCES/$IXEMUL_NAME/include \
    >>$LOGFILES/part6_gcc_configure.log 2>>$LOGFILES/part6_gcc_configure_err.log 
echo -e -n "make | "
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 all-gcc >>$LOGFILES/part6_gcc_make.log 2>>$LOGFILES/part6_gcc_make_err.log
echo -e "install\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 install-gcc >>$LOGFILES/part6_gcc_make.log 2>>$LOGFILES/part6_gcc_make_err.log
cd $SOURCES

# PART 7: AmigaOS NDK
echo -e "\e[1m\e[37m7. AmigaOS NDK\e[0m\e[36m"
echo -e -n "\e[0m\e[36m   * amigaos ndk 3.9:\e[30m patch | " 
for p in `ls $WORKSPACE/_install/recipes/patches/$NDK39_NAME/Include/include_h/devices/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/include_h/devices <$p >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$NDK39_NAME/Include/include_h/graphics/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/include_h/graphics <$p >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log; done 
for p in `ls $WORKSPACE/_install/recipes/patches/$NDK39_NAME/Include/sfd/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/sfd <$p >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log; done 
cp $WORKSPACE/_install/recipes/patches/$NDK39_NAME/Include/include_h/proto/* $SOURCES/$NDK39_NAME/Include/include_h/proto >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
echo -e -n "headers | "
cp -r  $SOURCES/NDK_3.9/Include/include_h/* $PREFIX/$TARGET/include >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/include_i/* $PREFIX/$TARGET/include >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/fd/* $PREFIX/$TARGET/lib/fd >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/sfd/* $PREFIX/$TARGET/lib/sfd >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/linker_libs/* $PREFIX/$TARGET/lib >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Documentation/Autodocs $PREFIX/$TARGET/doc >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log
echo -e -n "protos | "
cd $PREFIX/$TARGET/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=proto --output=$PREFIX/$TARGET/include/proto/$name >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log; done
cd $PREFIX/$TARGET/include/proto
rename -f 's/_lib.sfd/.h/' ./*.sfd
echo -e -n "inlines | "
cd $PREFIX/$TARGET/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=macros --output=$PREFIX/$TARGET/include/inline/$name >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log; done
cd $PREFIX/$TARGET/include/inline
rename -f 's/_lib.sfd/.h/' ./*.sfd
echo -e "lvo\e[0m"
cd $PREFIX/$TARGET/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=lvo --output=$PREFIX/$TARGET/include/lvo/$name >>$LOGFILES/part7_amigaos_ndk.log 2>>$LOGFILES/part7_amigaos_ndk_err.log; done
cd $PREFIX/$TARGET/include/lvo
rename -f 's/.sfd/.i/' ./*.sfd
cd $SOURCES

echo -e -n "\e[0m\e[36m   * additional ndk:\e[30m "
echo -e -n "openurl | "
cp -r OpenURL/OpenURL/Developer/C/include/* $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
echo -e -n "amissl | "
cp -r AmiSSL/AmiSSL/Developer/include/* $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
cp -r AmiSSL/AmiSSL/Developer/lib/AmigaOS3/* $PREFIX/$TARGET/lib/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
echo -e -n "guigfxlib | "
cp -r guigfxlib/include/* $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
echo -e -n "renderlib | "
cp -r renderlib/renderlib/include/* $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
echo -e -n "codesets | "
cp -r codesets/codesets/Developer/include/* $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
echo -e -n "apollo | "
cp -r apollo $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
echo -e "ahi\e[0m"
cp -r ahi $PREFIX/$TARGET/include/ >>$LOGFILES/part7_additional_ndk.log 2>>$LOGFILES/part7_additional_ndk_err.log
cd $SOURCES

# Part 8: Compile GCC Targets
echo -e "\e[1m\e[37m8. Compile Libraries\e[0m"

echo -e -n "\e[0m\e[36m   * clib2:\e[30m copy | "
mkdir -p $BUILDS/build-$CLIB2_NAME
cd $BUILDS/build-$CLIB2_NAME
cp -r $SOURCES/clib2/library/* $BUILDS/build-$CLIB2_NAME
echo -e -n "patch | "
for p in `ls $WORKSPACE/_install/recipes/patches/clib2/*.p`; do patch -d $BUILDS/build-$CLIB2_NAME <$p -p0 >>$LOGFILES/part8_clib2_patch.log 2>>$LOGFILES/part8_clib2_patch_err.log; done 
echo -e -n "customise | "
cp -r $WORKSPACE/_install/recipes/files/clib2/* $BUILDS/build-$CLIB2_NAME >>$LOGFILES/part8_clib2_patch.log 2>>$LOGFILES/part8_clib2_patch_err.log
echo -e -n "make | "
PATH=$PREFIX/bin:$PATH make -f GNUmakefile.68k >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
echo -e "install\e[0m"
cp -r $BUILDS/build-$CLIB2_NAME/include/* $PREFIX/$TARGET/clib2/include >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
cp -r $BUILDS/build-$CLIB2_NAME/lib/* $PREFIX/$TARGET/lib >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
ln -sf $PREFIX/$TARGET/lib/ncrt0.o $PREFIX/$TARGET/lib/crt0.o >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
cd $SOURCES

cd $BUILDS/build-$GCC_NAME
echo -e -n "\e[0m\e[36m   * libiberty:\e[30m make | " 
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 all-target-libiberty >>$LOGFILES/part8_libiberty_make.log 2>>$LOGFILES/part8_libiberty_make_err.log
echo -e "install\e[0m"
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 install-target-libiberty >>$LOGFILES/part8_libiberty_install.log 2>>$LOGFILES/part8_libiberty_install_err.log

echo -e -n "\e[0m\e[36m   * libstdc++-v3:\e[30m make | " 
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 all-target-libstdc++-v3 >>$LOGFILES/part8_libstdc_make.log 2>>$LOGFILES/part8_libstdc_make_err.log
echo -e -n "install | "
AUTOCONF=$GCC_AUTOCONF AUTOHEADER=$GCC_AUTOHEADER AUTOM4TE=$GCC_AUTOM4TE PATH="$PREFIX/bin:$PATH" \
make -j1 install-target-libstdc++-v3 >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log
echo -e "includes\e[0m"
mkdir -p $PREFIX/$TARGET/include/libstdc++ 
cp -r -L $TARGET/libstdc++-v3/include/* $PREFIX/$TARGET/include/libstdc++ >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log
cp -r -L $TARGET/libstdc++-v3/include/$TARGET/bits/* $PREFIX/$TARGET/include/libstdc++/bits >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log
cp $SOURCES/$GCC_NAME/libstdc++-v3/libsupc++/*.h $PREFIX/$TARGET/include/libstdc++ >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log
cp $SOURCES/$GCC_NAME/libstdc++-v3/libsupc++/new $PREFIX/$TARGET/include/libstdc++ >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log
cp $SOURCES/$GCC_NAME/libstdc++-v3/libsupc++/exception $PREFIX/$TARGET/include/libstdc++ >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log
cp $SOURCES/$GCC_NAME/libstdc++-v3/libsupc++/typeinfo $PREFIX/$TARGET/include/libstdc++ >>$LOGFILES/part8_libstdc_install.log 2>>$LOGFILES/part8_libstdc_install_err.log

cd $SOURCES

echo -e -n "\e[0m\e[36m   * libnix:\e[30m configure | "
mkdir -p $BUILDS/build-$LIBNIX_NAME
cd $BUILDS/build-$LIBNIX_NAME
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
    >>$LOGFILES/part8_libnix_configure.log 2>>$LOGFILES/part8_libnix_configure_err.log   
echo -e -n "make | "
CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
make $CPU >>$LOGFILES/part8_libnix_make.log 2>>$LOGFILES/part8_libnix_make_err.log
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part8_libnix_make.log 2>>$LOGFILES/part8_libnix_make_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * libnix:\e[30m ixemul headers | "
cp -r $PREFIX/$TARGET/sys-include/* $PREFIX/$TARGET/libnix/include >>$LOGFILES/part8_ixemul_headers.log 2>>$LOGFILES/part8_ixemul_headers_err.log
cp -r $SOURCES/$LIBNIX_NAME/sources/headers/stabs.h $PREFIX/$TARGET/libnix/include >>$LOGFILES/part8_ixemul_headers.log 2>>$LOGFILES/part8_ixemul_headers_err.log

echo -e -n "libamiga | "
mv lib $LIBAMIGA_NAME >>$LOGFILES/part8_libamiga.log 2>>$LOGFILES/part8_libamiga_err.log
cp -r $LIBAMIGA_NAME/* $PREFIX/$TARGET/libnix/lib >>$LOGFILES/part8_libamiga.log 2>>$LOGFILES/part8_libamiga_err.log

echo -e -n "libm | "
mv contrib/libm $LIBM_NAME
rm -r contrib
cp -f $WORKSPACE/_install/recipes/files/libm/config.* $LIBM_NAME
mkdir -p $BUILDS/build-$LIBM_NAME
cd $BUILDS/build-$LIBM_NAME
CC="$PREFIX/bin/$TARGET-gcc -noixemul" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$LIBM_NAME/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part8_libm_configure.log 2>>$LOGFILES/part8_libm_configure_err.log  
make $CPU >>$LOGFILES/part8_libm_make.log 2>>$LOGFILES/part8_libm_make_err.log
make $CPU install >>$LOGFILES/part8_libm_make.log 2>>$LOGFILES/part8_libm_make_err.log
cd $SOURCES

echo -e "libdebug\e[0m"
mkdir -p $BUILDS/build-$LIBDEBUG_NAME
cd $BUILDS/build-$LIBDEBUG_NAME
touch $SOURCES/$LIBDEBUG_NAME/configure
CC="$PREFIX/bin/$TARGET-gcc -noixemul" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$LIBDEBUG_NAME/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part8_libdebug_configure.log 2>>$LOGFILES/part8_libdebug_configure_err.log  
make $CPU >>$LOGFILES/part8_libdebug_make.log 2>>$LOGFILES/part8_libdebug_make_err.log 
make $CPU install >>$LOGFILES/part8_libdebug_make.log 2>>$LOGFILES/part8_libdebug_make_err.log 
cd $SOURCES

echo -e "\e[0m\e[36m   * organise target directory for clib2 and libnix support\e[30m"
mv $PREFIX/$TARGET/libs* $PREFIX/$TARGET/lib
mv $PREFIX/$TARGET/lib/libb $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libb32 $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libm020 $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libamiga.a $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libc.a $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libdebug.a $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libm.a $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libnet.a $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/libunix.a $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv $PREFIX/$TARGET/lib/n* $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
ln -sf $PREFIX/$TARGET/clib2/lib/ncrt0.o $PREFIX/$TARGET/clib2/lib/crt0.o >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
mv -f $PREFIX/lib/gcc/$TARGET/3.4.6/specs $PREFIX/lib/gcc/$TARGET/3.4.6/specs.original >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log
cp -f $WORKSPACE/_install/recipes/files.wd/specs.346 $PREFIX/lib/gcc/$TARGET/3.4.6/specs >>$LOGFILES/part8_clib2_organise.log 2>>$LOGFILES/part8_clib2_organise_err.log

# PART 9: Cleanup
echo -e "\e[1m\e[37m9. Cleanup\e[0m\e[36m"
rm -rf $PREFIX/etc
rm -rf $PREFIX/include
rm -rf $PREFIX/share
rm -rf $PREFIX/info
rm -rf $PREFIX/man
rm -rf $PREFIX/$TARGET/sys-include
rm -rf $PREFIX/$TARGET/lib/crt0.o
rm -rf $PREFIX/$TARGET/libstdc++/include/Makefile
rm -rf $PREFIX/$TARGET/include/libstdc++/$TARGET 

# PART 10: Bonus SDK
echo -e "\e[1m\e[37m10. Bonus Libs/SDK\e[0m\e[36m"

echo -e -n "\e[0m\e[36m   * $LIBSDL_NAME:\e[30m configure | "
mkdir -p $BUILDS/build-$LIBSDL_NAME
cd $BUILDS/build-$LIBSDL_NAME
cp -r $SOURCES/$LIBSDL_NAME/* $BUILDS/build-$LIBSDL_NAME >>$LOGFILES/part10_libSDL.log 2>>$LOGFILES/part10_libSDL_err.log
echo -e -n "make | "
make >>$LOGFILES/part10_libSDL.log 2>>$LOGFILES/part10_libSDL_err.log
echo -e "install\e[0m"
cp libSDL.a $PREFIX/$TARGET/lib >>$LOGFILES/part10_libSDL.log 2>>$LOGFILES/part10_libSDL_err.log
mkdir -p $PREFIX/$TARGET/include/sdl
cp include/SDL/* $PREFIX/$TARGET/include/sdl >>$LOGFILES/part10_libSDL.log 2>>$LOGFILES/part10_libSDL_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $LIBOGG_NAME:\e[30m patch | "
cp -rf $WORKSPACE/_install/recipes/files.wd/$LIBOGG_NAME/* $SOURCES/$LIBOGG_NAME >>$LOGFILES/part10_libogg_patch.log 2>>$LOGFILES/part10_libogg_patch_err.log
echo -e -n "make | "
mkdir -p $BUILDS/build-$LIBOGG_NAME
cd $BUILDS/build-$LIBOGG_NAME
CC="$PREFIX/bin/$TARGET-gcc -noixemul" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$LIBOGG_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part10_libogg_configure.log 2>>$LOGFILES/part10_libogg_configure_err.log  
echo -e -n "make | "
make $CPU >>$LOGFILES/part10_libogg_make.log 2>>$LOGFILES/part10_libogg_make_err.log   
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part10_libogg_make.log 2>>$LOGFILES/part10_libogg_make_err.log   
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $LIBVORBIS_NAME:\e[30m configure | "
mkdir -p $BUILDS/build-$LIBVORBIS_NAME
cd $BUILDS/build-$LIBVORBIS_NAME
CFLAGS="-I$PREFIX/$TARGET/include" \
LDFLAGS="-L$PREFIX/$TARGET/lib"  \
CC="$PREFIX/bin/$TARGET-gcc -static-libgcc" \
$SOURCES/$LIBVORBIS_NAME/configure \
    --prefix=$PREFIX/$TARGET \
    --host=$TARGET \
    --build=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part10_libvorbis_configure.log 2>>$LOGFILES/part10_libvorbis_configure_err.log  
echo -e -n "make | "
make $CPU >>$LOGFILES/part10_libvorbis_make.log 2>>$LOGFILES/part10_libvorbis_make_err.log   
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part10_libvorbis_make.log 2>>$LOGFILES/part10_libvorbis_make_err.log   
cd $SOURCES

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

