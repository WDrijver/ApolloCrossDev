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
CLIB2_NAME=clib2
CLIB2_DOWNLOAD=https://github.com/adtools/clib2
LIBNIX_NAME=libnix
LIBNIX_DOWNLOAD=https://github.com/adtools/libnix
FD2SFD_NAME=fd2sfd
FD2SFD_DOWNLOAD=https://github.com/adtools/fd2sfd
SFDC_NAME=sfdc
SFDC_DOWNLOAD=https://github.com/adtools/sfdc
LIBDEBUG_NAME=libdebug
LIBDEBUG_DOWNLOAD=https://github.com/adtools/libdebug
FD2PRAGMA_NAME=fd2pragma
FD2PRAGMA_DOWNLOAD=https://github.com/adtools/fd2pragma.git
IXEMUL_NAME=ixemul-48.2
IXEMUL_DOWNLOAD=http://downloads.sf.net/project/amiga/ixemul.library/48.2/ixemul-src.lha
IXEMUL_ARCHIVE=ixemul-src.lha
LIBAMIGA_NAME=libamiga
LIBAMIGA_DOWNLOAD=ftp://ftp.exotica.org.uk/mirrors/geekgadgets/amiga/m68k/snapshots/990529/bin/libamiga-bin.tgz
LIBAMIGA_ARCHIVE=libamiga-bin.tgz
LIBM_NAME=libm-5.4
LIBM_DOWNLOAD=ftp://ftp.exotica.org.uk/mirrors/geekgadgets/amiga/m68k/snapshots/990529/src/libm-5.4-src.tgz
LIBM_ARCHIVE=libm-5.4-src.tgz

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
mkdir -p $PREFIX/bin $PREFIX/etc $PREFIX/$TARGET/bin $PREFIX/$TARGET/ndk $PREFIX/$TARGET/sys-include
mkdir -p $PREFIX/$TARGET/ndk/include $PREFIX/$TARGET/ndk/lib $PREFIX/$TARGET/ndk/include/inline
mkdir -p $PREFIX/$TARGET/ndk/include/lvo  $PREFIX/$TARGET/ndk/lib/fd $PREFIX/$TARGET/ndk/lib/sfd
mkdir -p $PREFIX/$TARGET/clib2 $PREFIX/$TARGET/clib2/include $PREFIX/$TARGET/clib2/lib
mkdir -p $PREFIX/$TARGET/libnix $PREFIX/$TARGET/libnix/include $PREFIX/$TARGET/libnix/lib

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
echo -e "\e[36m   * On first run: please be patient"
sudo apt -y update >>$LOGFILES/part2_linux_updates.log 2>>$LOGFILES/part2_linux_updates_err.log
sudo apt -y install build-essential m4 gawk autoconf automake flex bison expect dejagnu texinfo lhasa git subversion \
     make wget libgmp-dev libmpfr-dev libmpc-dev gettext texinfo ncurses-dev rsync libreadline-dev rename gperf gcc-multilib \
     >>$LOGFILES/part2_linux_updates.log 2>>$LOGFILES/part2_linux_updates_err.log

# PART 3: Unpack Archives
cd $ARCHIVES
echo -e "\e[1m\e[37m3. Unpack Source Archives\e[0m\e[36m"
for f in *.tar*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done 
for f in *.tgz*; do tar xfk $f --directory $SOURCES >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log; done 
lha -xw=$SOURCES $IXEMUL_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
lha -xw=$SOURCES $NDK39_ARCHIVE >>$LOGFILES/part3_unpack.log 2>>$LOGFILES/part3_unpack_err.log
cd $SOURCES

# PART 4: Tools
echo -e "\e[1m\e[37m4. Install Tools\e[0m\e[36m"
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
cp $SOURCES/$FD2SFD_NAME/cross/share/$TARGET/alib.h $PREFIX/$TARGET/ndk/include/inline >>$LOGFILES/part4_tools_fd2sfd.log 2>>$LOGFILES/part4_tools_fd2sfd_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * $FD2PRAGMA_NAME:\e[30m configure | " 
cp -r $SOURCES/$FD2PRAGMA_NAME $BUILDS/build-$FD2PRAGMA_NAME
cd $BUILDS/build-$FD2PRAGMA_NAME
    >>$LOGFILES/part4_tools_fd2pragma.log 2>>$LOGFILES/part4_tools_fd2pragma_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part4_tools_fd2pragma.log 2>>$LOGFILES/part4_tools_fd2pragma_err.log
echo -e "install\e[0m"
cp fd2pragma $PREFIX/bin 
cp Include/inline/* $PREFIX/$TARGET/ndk/include/inline 
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

# PART 5: AmigaOS NDK
echo -e "\e[1m\e[37m5. Amiga OS NDK\e[0m\e[36m"
echo -e -n "\e[0m\e[36m   * amigaos ndk 3.9:\e[30m patch | " 
for p in `ls $WORKSPACE/_install/patches/$NDK39_NAME/Include/include_h/devices/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/include_h/devices <$p >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$NDK39_NAME/Include/include_h/graphics/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/include_h/graphics <$p >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$NDK39_NAME/Include/sfd/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/sfd <$p >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log; done 
cp $WORKSPACE/_install/patches/$NDK39_NAME/Include/include_h/proto/* $SOURCES/$NDK39_NAME/Include/include_h/proto >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
echo -e -n "headers | "
cp -r  $SOURCES/NDK_3.9/Include/include_h/* $PREFIX/$TARGET/ndk/include >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/include_i/* $PREFIX/$TARGET/ndk/include >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/fd/* $PREFIX/$TARGET/ndk/lib/fd >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/sfd/* $PREFIX/$TARGET/ndk/lib/sfd >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Include/linker_libs/* $PREFIX/$TARGET/ndk/lib >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
cp -r  $SOURCES/NDK_3.9/Documentation/Autodocs $PREFIX/$TARGET/ndk/doc >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log
echo -e -n "protos | "
cd $PREFIX/$TARGET/ndk/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=proto --output=$PREFIX/$TARGET/ndk/include/proto/$name >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log; done
cd $PREFIX/$TARGET/ndk/include/proto
rename -f 's/_lib.sfd/.h/' ./*.sfd
echo -e -n "inlines | "
cd $PREFIX/$TARGET/ndk/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=macros --output=$PREFIX/$TARGET/ndk/include/inline/$name >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log; done
cd $PREFIX/$TARGET/ndk/include/inline
rename -f 's/_lib.sfd/.h/' ./*.sfd
echo -e "lvo\e[0m"
cd $PREFIX/$TARGET/ndk/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=lvo --output=$PREFIX/$TARGET/ndk/include/lvo/$name >>$LOGFILES/part5_amigaos_ndk.log 2>>$LOGFILES/part5_amigaos_ndk_err.log; done
cd $PREFIX/$TARGET/ndk/include/lvo
rename -f 's/.sfd/.i/' ./*.sfd
cd $SOURCES

# Part 6: Compile BinUtils
echo -e "\e[1m\e[37m6. Compile Binutils"
echo -e -n "\e[0m\e[36m   * binutils:\e[30m configure | " 
mkdir -p $BUILDS/build-$BINUTILS_NAME
cd $BUILDS/build-$BINUTILS_NAME
CC=$CC32 CXX=$CXX32 \
CFLAGS=$FLAGS CXXFLAGS=$FLAGS \
$SOURCES/$BINUTILS_NAME/configure \
    --prefix="$PREFIX" \
    --target="$TARGET" \
    --disable-nls \
    --host=i686-linux-gnu \
    >>$LOGFILES/part6_binutils_configure.log 2>>$LOGFILES/part6_binutils_configure_err.log
echo -e -n "make | "
make $CPU >>$LOGFILES/part6_binutils_make.log 2>>$LOGFILES/part6_binutils_make_err.log
echo -e "install\e[0m"
make $CPU install-binutils >>$LOGFILES/part6_binutils_make.log 2>>$LOGFILES/part6_binutils_make_err.log
make $CPU install-gas >>$LOGFILES/part6_binutils_make.log 2>>$LOGFILES/part6_binutils_make_err.log
make $CPU install-ld >>$LOGFILES/part6_binutils_make.log 2>>$LOGFILES/part6_binutils_make_err.log
make $CPU install-info >>$LOGFILES/part6_binutils_make.log 2>>$LOGFILES/part6_binutils_make_err.log
cd $SOURCES

# Part 7: Compile GCC Run #1
echo -e "\e[1m\e[37m7. Compile GCC (Compiler)\e[0m"
mv ixemul $IXEMUL_NAME
echo -e -n "\e[0m\e[36m   * gcc:\e[30m patch | " 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done  
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/general/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/general <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/glue/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/glue <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/ixnet/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/ixnet <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/library/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/library <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/stdio/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdio <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/stdlib/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdlib <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/string/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/string <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/utils/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/utils <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
mkdir -p $BUILDS/build-$GCC_NAME
cd $BUILDS/build-$GCC_NAME
rm -r $SOURCES/$GCC_NAME/texinfo >>$LOGFILES/part7_patch_gcc.log 2>>$LOGFILES/part7_patch_gcc_err.log
cp -rf $WORKSPACE/_install/files.wd/gcc/* $SOURCES/$GCC_NAME >>$LOGFILES/part7_patch_gcc.log 2>>$LOGFILES/part7_patch_gcc_err.log
echo -e -n "configure | "
CC=$CC32 CXX=$CXX32 \
CFLAGS=$FLAGS CXXFLAGS=$FLAGS \
$SOURCES/$GCC_NAME/configure \
    --prefix="$PREFIX" \
    --target="$TARGET" \
    --host=i686-linux-gnu \
    --build=i686-linux-gnu \
    --enable-languages=c \
    --enable-version-specific-runtime-libs \
    --with-headers=$SOURCES/$IXEMUL_NAME/include \
    >>$LOGFILES/part7_gcc_configure.log 2>>$LOGFILES/part7_gcc_configure_err.log
echo -e -n "make | "
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make -j1 all-gcc >>$LOGFILES/part7_gcc_make.log 2>>$LOGFILES/part7_gcc_make_err.log
echo -e "install\e[0m"
MAKEINFO="makeinfo" \
FLAGS_FOR_TARGET="-noixemul" \
make -j1 install-gcc >>$LOGFILES/part7_gcc_make.log 2>>$LOGFILES/part7_gcc_make_err.log
cd $SOURCES

# Part 8: Libraries
echo -e "\e[1m\e[37m8. Compile Libraries\e[0m"

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
cp -r $SOURCES/$LIBNIX_NAME/sources/headers/stabs.h $PREFIX/$TARGET/libnix/include
cd $SOURCES

echo -e -n "\e[0m\e[36m   * libnix:\e[30m ixemul headers | "
cp -r $SOURCES/$IXEMUL_NAME/include/* $PREFIX/$TARGET/libnix/include >>$LOGFILES/part7_ixemul_headers.log 2>>$LOGFILES/part7_ixemul_headers_err.log

echo -e -n "libamiga | "
mv lib $LIBAMIGA_NAME >>$LOGFILES/part8_libamiga.log 2>>$LOGFILES/part8_libamiga_err.log
cp -r $LIBAMIGA_NAME/* $PREFIX/$TARGET/libnix/lib >>$LOGFILES/part8_libamiga.log 2>>$LOGFILES/part8_libamiga_err.log

echo -e -n "libm | "
mv contrib/libm $LIBM_NAME
rm -r contrib
cp -f $WORKSPACE/_install/files/libm/config.* $LIBM_NAME
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

echo -e -n "\e[0m\e[36m   * clib2:\e[30m configure | " 
mkdir -p $BUILDS/build-$CLIB2_NAME
cd $BUILDS/build-$CLIB2_NAME
cp -r $SOURCES/clib2/library/* $BUILDS/build-$CLIB2_NAME
echo -e -n "make | "
PATH=$PREFIX/bin:$PATH make -f GNUmakefile.68k >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
echo -e "install\e[0m"
cp -r $BUILDS/build-$CLIB2_NAME/include/* $PREFIX/$TARGET/clib2/include >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
cp -r $BUILDS/build-$CLIB2_NAME/lib/* $PREFIX/$TARGET/clib2/lib >>$LOGFILES/part8_clib2_make.log 2>>$LOGFILES/part8_clib2_make_err.log
cd $SOURCES

# Part 9: Compile GCC Run #2
echo -e "\e[1m\e[37m9. Compile GCC (Target Libs)\e[0m"
cd $BUILDS/build-$GCC_NAME
mkdir -p $TARGET/libb32 #workaround for bug in libiberty make process
echo -e -n "\e[0m\e[36m   * gcc:\e[30m make | " 
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make -j all-target >>$LOGFILES/part9_gcc_make.log 2>>$LOGFILES/part9_gcc_make_err.log
echo -e "install\e[0m"
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make -j1 install-target >>$LOGFILES/part9_gcc_make.log 2>>$LOGFILES/part9_gcc_make_err.log
cd $SOURCES

# PART 10: Cleanup
echo -e "\e[1m\e[37m10. Cleanup\e[0m\e[36m"
rm -rf $PREFIX/etc
rm -rf $PREFIX/include
rm -rf $PREFIX/share
rm -rf $PREFIX/$TARGET/include

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit

#LIBNIX + Additional Libnix Libs

echo -e "\e[0m\e[36m   * libamiga:\e[30m install\e[0m"
mv lib $LIBAMIGA_NAME >>$LOGFILES/part8_libamiga.log 2>>$LOGFILES/part8_libamiga_err.log
cp -r $LIBAMIGA_NAME/* $PREFIX/$TARGET/libnix/lib >>$LOGFILES/part8_libamiga.log 2>>$LOGFILES/part8_libamiga_err.log

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
cp -r $SOURCES/$LIBNIX_NAME/sources/headers/stabs.h $PREFIX/$TARGET/libnix/include
cd $SOURCES

echo -e -n "\e[0m\e[36m   * libm:\e[30m configure | "
mv contrib/libm $LIBM_NAME
rm -r contrib
cp -f $WORKSPACE/_install/config.* $LIBM_NAME
echo -e -n "configure | "
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
echo -e -n "make | "
make $CPU >>$LOGFILES/part8_libm_make.log 2>>$LOGFILES/part8_libm_make_err.log
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part8_libm_make.log 2>>$LOGFILES/part8_libm_make_err.log
cd $SOURCES

echo -e -n "\e[0m\e[36m   * libdebug:\e[30m configure | "
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
echo -e -n "make | "
make $CPU >>$LOGFILES/part8_libdebug_make.log 2>>$LOGFILES/part8_libdebug_make_err.log 
echo -e "install\e[0m"
make $CPU install >>$LOGFILES/part8_libdebug_make.log 2>>$LOGFILES/part8_libdebug_make_err.log 
cd $SOURCES

####################################################################

# PART 3: Prepare Sources (Sources moved locally in ApolloCrossDev Git Repo)
echo -e "\e[1m\e[37m3. Download Sources\e[0m\e[36m"
cd $SOURCES
echo -e -n "\e[36m   * GNU Sources:\e[30m $BINUTILS_NAME |" 
git clone --progress $BINUTILS_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo " $GCC_NAME" 
git clone --progress $GCC_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo -e -n "\e[36m   * Libraries\e[30m: $CLIB2_NAME |"
git clone --progress $CLIB2_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo -n " $LIBNIX_NAME |" 
git clone --progress $LIBNIX_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo -n " $LIBDEBUG_NAME |" 
git clone --progress $LIBDEBUG_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo -n " $IXEMUL_NAME |" 
wget -nc $IXEMUL_DOWNLOAD -a $LOGFILES/part3_sources.log
echo -n " $LIBAMIGA_NAME |" 
wget -nc $LIBAMIGA_DOWNLOAD -a $LOGFILES/part3_sources.log
echo " $LIBM_NAME" 
wget -nc $LIBM_DOWNLOAD -a $LOGFILES/part3_sources.log
echo -e -n "\e[36m   * Tools:\e[30m $FD2SFD_NAME |"
git clone --progress $FD2SFD_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo -n " $SFDC_NAME |" 
git clone --progress $SFDC_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo " $FD2PRAGMA_NAME" 
git clone --progress $FD2PRAGMA_DOWNLOAD 2>>$LOGFILES/part3_sources.log
echo -e "\e[0m\e[36m   * NDKS's:\e[30m $NDK39_NAME |"
wget -nc $NDK39_DOWNLOAD -a $LOGFILES/part3_sources.log
mv download.php?id=3 $NDK39_ARCHIVE

#IXEMUL removed
mv ixemul $IXEMUL_NAME
echo -e "\e[0m\e[36m   * Patch ixemul\e[0m"
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done  
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/general/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/general <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/glue/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/glue <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/ixnet/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/ixnet <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/library/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/library <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/stdio/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdio <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/stdlib/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdlib <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/string/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/string <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/utils/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/utils <$p >>$LOGFILES/part7_ixemul_patch.log 2>>$LOGFILES/part7_ixemul_patch_err.log; done 

echo -e "\e[0m\e[36m   * Install ixemul Headers\e[0m"
cp -r $SOURCES/$IXEMUL_NAME/include $PREFIX/$TARGET/libnix/include >>$LOGFILES/part7_ixemul_headers.log 2>>$LOGFILES/part7_ixemul_headers_err.log
cd $SOURCES



