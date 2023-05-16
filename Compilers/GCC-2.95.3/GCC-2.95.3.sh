# ApolloCrossDev GCC-2.95.3 Install Script v0.7
# 
# Installation:
# 1. Enter Compilers/GCC-2.95.3 directory
# 2. Type "./GCC-2.95.3.sh" and hit ENTER
#
# Instructions:
# 1. Create Projects/<mysource> directory
# 2. Copy Projects/make-gcc295 into <mysource> 
# 3. Read make-gcc295 for compile instructions

EDITION=GNU-2.95.3
VERSION=0.7
CPU=-j16

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=m68k-amigaos
export PATH=$PREFIX/bin:$PATH

CC=
CXX=
FLAGS='-g -O2'

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
rm -f -r $PREFIX
rm -f -r $LOGFILES
rm -f -r $SOURCES
echo "   * Create Directories" 
mkdir -p $LOGFILES
mkdir -p $SOURCES
mkdir -p $PREFIX
mkdir -p $PREFIX/bin
mkdir -p $PREFIX/etc
mkdir -p $PREFIX/$TARGET
mkdir -p $PREFIX/$TARGET/bin
mkdir -p $PREFIX/$TARGET/ndk
mkdir -p $PREFIX/$TARGET/ndk/include
mkdir -p $PREFIX/$TARGET/ndk/include/inline
mkdir -p $PREFIX/$TARGET/ndk/include/lvo
mkdir -p $PREFIX/$TARGET/ndk/lib
mkdir -p $PREFIX/$TARGET/ndk/lib/fd
mkdir -p $PREFIX/$TARGET/ndk/lib/sfd
mkdir -p $PREFIX/$TARGET/libnix

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential m4 gawk autoconf automake flex bison expect dejagnu texinfo lhasa git subversion \
     make wget libgmp-dev libmpfr-dev libmpc-dev gettext texinfo ncurses-dev rsync libreadline-dev rename \
>>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Download Sources
echo -e "\e[1m\e[37m3. Download Sources\e[0m\e[36m"
cd $SOURCES
echo -e -n "\e[36m   * GNU Sources:\e[30m $BINUTILS_NAME |" 
git clone --progress $BINUTILS_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo " $GCC_NAME" 
git clone --progress $GCC_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo -e -n "\e[36m   * Libraries\e[30m: $CLIB2_NAME |"
git clone --progress $CLIB2_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo -n " $LIBNIX_NAME |" 
git clone --progress $LIBNIX_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo -n " $LIBDEBUG_NAME |" 
git clone --progress $LIBDEBUG_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo -n " $IXEMUL_NAME |" 
wget -nc $IXEMUL_DOWNLOAD -a $LOGFILES/part3.log
echo -n " $LIBAMIGA_NAME |" 
wget -nc $LIBAMIGA_DOWNLOAD -a $LOGFILES/part3.log
echo " $LIBM_NAME" 
wget -nc $LIBM_DOWNLOAD -a $LOGFILES/part3.log
echo -e -n "\e[36m   * Tools:\e[30m $FD2SFD_NAME |"
git clone --progress $FD2SFD_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo -n " $SFDC_NAME |" 
git clone --progress $SFDC_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo " $FD2PRAGMA_NAME" 
git clone --progress $FD2PRAGMA_DOWNLOAD 2>>$LOGFILES/part3_err.log
echo -e "\e[0m\e[36m   * NKS's:\e[30m $NDK39_NAME |"
wget -nc $NDK39_DOWNLOAD -a $LOGFILES/part5.log
mv download.php?id=3 $NDK39_ARCHIVE

# PART 4: Tools
echo -e "\e[1m\e[37m4. Install Tools\e[0m\e[36m"
echo "   * $FD2SFD_NAME" 
cd $FD2SFD_NAME
./configure \
    --prefix=$PREFIX \
    >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
make >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
cp fd2sfd $PREFIX/bin >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
cp cross/share/$TARGET/alib.h $PREFIX/$TARGET/ndk/include/inline >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
cd $SOURCES
echo "   * $FD2PRAGMA_NAME" 
cd $FD2PRAGMA_NAME
./configure \
    >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
make >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
cp fd2pragma $PREFIX/bin 
cp Include/inline/* $PREFIX/$TARGET/ndk/include/inline 
cd $SOURCES
echo "   * $SFDC_NAME" 
cd $SFDC_NAME
./configure \
    --prefix=$PREFIX \
    >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
make >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
make install >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
cd $SOURCES

# PART 5: AmigaOS 3.9 NDK
echo -e "\e[1m\e[37m5. Amiga OS 3.9 NDK\e[0m\e[36m"
echo -e "\e[0m\e[36m   * Unpack $NDK39_ARCHIVE\e[0m"
lha -xw=$SOURCES $NDK39_ARCHIVE >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
echo -e "\e[0m\e[36m   * Patch AmigaOS NDK 3.9\e[0m"
for p in `ls $WORKSPACE/_install/patches/$NDK39_NAME/Include/include_h/devices/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/include_h/devices <$p >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$NDK39_NAME/Include/include_h/graphics/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/include_h/graphics <$p >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$NDK39_NAME/Include/sfd/*.diff`; do patch -d $SOURCES/$NDK39_NAME/Include/sfd <$p >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done 
cp $WORKSPACE/_install/patches/$NDK39_NAME/Include/include_h/proto/* $SOURCES/$NDK39_NAME/Include/include_h/proto >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
echo -e "\e[0m\e[36m   * Copy NDK Directories to $TARGET\e[0m" 
cp -r  $SOURCES/NDK_3.9/Include/include_h/* $PREFIX/$TARGET/ndk/include >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r  $SOURCES/NDK_3.9/Include/include_i/* $PREFIX/$TARGET/ndk/include >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r  $SOURCES/NDK_3.9/Include/fd/* $PREFIX/$TARGET/ndk/lib/fd >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r  $SOURCES/NDK_3.9/Include/sfd/* $PREFIX/$TARGET/ndk/lib/sfd >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r  $SOURCES/NDK_3.9/Include/linker_libs/* $PREFIX/$TARGET/ndk/lib >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
cp -r  $SOURCES/NDK_3.9/Documentation/Autodocs $PREFIX/$TARGET/ndk/doc >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log
echo -e -n "\e[0m\e[36m   * Create Include files from SFD: \e[30mProto |" 
cd $PREFIX/$TARGET/ndk/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=proto --output=$PREFIX/$TARGET/ndk/include/proto/$name >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done
cd $PREFIX/$TARGET/ndk/include/proto
rename -f 's/_lib.sfd/.h/' ./*.sfd
echo -n " Inline |" 
cd $PREFIX/$TARGET/ndk/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=macros --output=$PREFIX/$TARGET/ndk/include/inline/$name >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done
cd $PREFIX/$TARGET/ndk/include/inline
rename -f 's/_lib.sfd/.h/' ./*.sfd
echo " LVO"
cd $PREFIX/$TARGET/ndk/lib/sfd
for name in `ls *.sfd`; do $PREFIX/bin/sfdc $name --target=$TARGET --mode=lvo --output=$PREFIX/$TARGET/ndk/include/lvo/$name >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log; done
cd $PREFIX/$TARGET/ndk/include/lvo
rename -f 's/.sfd/.i/' ./*.sfd
cd $SOURCES

# Part 6: Compile BinUtils
echo -e "\e[1m\e[37m6. Compile $BINUTILS_NAME"
echo -e "\e[0m\e[36m   * Configure Binutils\e[0m"
mkdir -p build-binutils
cd build-binutils
CFLAGS="$FLAGS -m32 -std=gnu11" \
CXXFLAGS="$FLAGS -m32 -std=gnu++11" \
LDFLAGS="-m32" \
../$BINUTILS_NAME/configure \
    --prefix="$PREFIX" \
    --infodir="$PREFIX/$TARGET/info" \
    --mandir="$PREFIX/share/man" \
    --disable-nls \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -e "\e[0m\e[36m   * Build Binutils ($CPU)\e[0m"
make $CPU >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo -e "\e[0m\e[36m   * Install Binutils ($CPU)\e[0m"
make $CPU install-binutils >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make $CPU install-gas >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make $CPU install-ld >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
make $CPU install-info >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
cd $SOURCES

# Part 7: Compile GCC
echo -e "\e[1m\e[37m5. Compile $GCC_NAME\e[0m"
echo -e "\e[0m\e[36m   * Unpack ixemul\e[0m"
lha -xw=$SOURCES $IXEMUL_ARCHIVE >>$LOGFILES/part7.log 2>>$LOGFILES/part5_err.log
mv ixemul $IXEMUL_NAME
echo -e "\e[0m\e[36m   * Patch ixemul\e[0m"
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done  
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/general/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/general <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/glue/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/glue <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/ixnet/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/ixnet <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/library/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/library <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/stdio/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdio <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/stdlib/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/stdlib <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/string/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/string <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
for p in `ls $WORKSPACE/_install/patches/$IXEMUL_NAME/utils/*.diff`; do patch -d $SOURCES/$IXEMUL_NAME/utils <$p >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log; done 
mkdir -p build-gcc
cd build-gcc
echo -e "\e[0m\e[36m   * Configure GCC\e[0m"
CFLAGS="$FLAGS" \
CXXFLAGS="$FLAGS" \
../$GCC_NAME/configure \
    --prefix="$PREFIX" \
    --infodir="$PREFIX/$TARGET/info" \
    --mandir="$PREFIX/share/man" \
    --host=i686-linux-gnu \
    --build=i686-linux-gnu \
    --target=$TARGET \
    --enable-languages=c,c++ \
    --enable-version-specific-runtime-libs \
    --with-headers=$SOURCES/$IXEMUL_NAME/include \
    >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo -e "\e[0m\e[36m   * Build GCC - Run #1\e[0m"
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make $CPU all-gcc >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo -e "\e[0m\e[36m   * Install GCC - Run #1\e[0m"
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make -j1 install-gcc >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cd $SOURCES

echo -e "\e[0m\e[36m   * Install Header files\e[0m"
cp -r $IXEMUL_NAME/include $PREFIX/$TARGET/libnix/include

echo -e "\e[0m\e[36m   * Unpack libamiga\e[0m"
tar xfk $LIBAMIGA_ARCHIVE >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
mv lib $LIBAMIGA_NAME
cp -r $LIBAMIGA_NAME $PREFIX/$TARGET/libnix/lib

echo -e "\e[0m\e[36m   * Build libnix\e[0m"
mkdir -p build-libnix
cd build-libnix
$SOURCES/$LIBNIX_NAME/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log   
echo -e "\e[0m\e[36m   * Install libnix\e[0m"
CC="$PREFIX/bin/$TARGET-gcc" \
CPP="$PREFIX/bin/$TARGET-gcc -E" \
AR="$PREFIX/bin/$TARGET-ar" \
AS="$PREFIX/bin/$TARGET-as" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
LD="$PREFIX/bin/$TARGET-ld" \
make -j1 >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
make -j1 install >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
cp -r $SOURCES/$LIBNIX_NAME/sources/headers/stabs.h $PREFIX/$TARGET/libnix/include
cd $SOURCES

echo -e "\e[0m\e[36m   * Unpack libm\e[0m"
tar xfk $LIBM_ARCHIVE >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
mv contrib/libm $LIBM_NAME
rm -r contrib

echo -e "\e[0m\e[36m   * Build libm\e[0m"
mkdir -p build-libm
cd build-libm

CC="$PREFIX/bin/$TARGET-gcc -noixemul" \
AR="$PREFIX/bin/$TARGET-ar" \
RANLIB="$PREFIX/bin/$TARGET-ranlib" \
$SOURCES/$LIBM_NAME/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=$TARGET \
    >>$LOGFILES/libm_config.log 2>>$LOGFILES/libm_config_err.log  

echo -e "\e[0m\e[36m   * Install libm\e[0m"
make -j1 >>$LOGFILES/libm_make.log 2>>$LOGFILES/libm_make_err.log
make -j1 install >>$LOGFILES/libm_make_install.log 2>>$LOGFILES/libm_make_install_err.log




exit

echo -e "\e[0m\e[36m   * Build GCC - Run #2\e[0m"
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make $CPU all-gcc >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log
echo -e "\e[0m\e[36m   * Install GCC - Run #2\e[0m"
MAKEINFO="makeinfo" \
CFLAGS_FOR_TARGET="-noixemul" \
make -j1 install-gcc >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log

cd $SOURCES

exit

# PART 7: Amiga OS NDK's
echo -e "\e[1m\e[37m7. Amiga NDK's"
mkdir -p NDK3.2
cd NDK3.2
echo -e "\e[0m\e[36m   * NDK 3.2\e[0m"
wget -nc $NDK32_DOWNLOAD -a $LOGFILES/part7.log
lha -xw=$PREFIX/include/NDK3.2 NDK3.2.lha >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log


LIBNIX_NAME=2.1
LIBNIX_DOWNLOAD=https://github.com/adtools/libnix

echo "   * libnix-$LIBNIX_NAME"
git clone --progress $LIBNIX_DOWNLOAD >>$LOGFILES/part3.log 2>>$LOGFILES/part3_err.log

echo -e "\e[0m\e[36m   * Configure libnix\e[0m"
mkdir -p built-libnix
cd built-libnix
$SOURCES/libnix/configure \
    --prefix=$PREFIX/$TARGET/libnix \
    --host=i686-linux-gnu \
    --target=m68k-amigaos

make \
    CC=$PREFIX/bin/m68k-amigaos-gcc \
    CPP=$PREFIX/bin/m68k-amigaos-gcc -E \
    AR=$PREFIX/bin/m68k-amigaos-ar \
    AS=$PREFIX/bin/m68k-amigaos-as \
    RANLIB=$PREFIX/bin/m68k-amigaos-ranlib \
    LD=$PREFIX/bin/m68k-amigaos-ld \

cd $SOURCES

exit

# PART 8: Cleanup
echo -e "\e[1m\e[37m8. Cleanup\e[0m\e[36m"
cd $PREFIX
rm -rf info
rm -rf man

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit

# SCRAPBOOK

# Part 4: Compile BinUtils
echo -e "\e[1m\e[37m4. Compile $BINUTILS_NAME\e[0m\e[36m"
mkdir -p build-binutils
echo -e "\e[0m\e[36m   * Configure Binutils\e[0m"
cd build-binutils
../$BINUTILS_NAME/configure \
    --disable-nls \
    --prefix="$PREFIX" \
    --host=i686-linux-gnu \
    --target=$TARGET >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Build Binutils ($CPU)\e[0m"
make $CPU >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
echo -e "\e[0m\e[36m   * Install Binutils ($CPU)\e[0m"
make $CPU install-binutils >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
make $CPU install-gas >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
make $CPU install-ld >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log
cd ..

# PART 6: Amiga Libs/Includes
echo -e "\e[1m\e[37m6. Amiga Libraries\e[0m\e[36m"
echo "   * libnix"
cp -r $WORKSPACE/_install/libnix $PREFIX/$TARGET
echo "   * clib2"
cp -r $WORKSPACE/_install/clib2 $PREFIX/$TARGET
