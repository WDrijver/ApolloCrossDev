# ApolloCrossDev VBCC Install Script v0.5

EDITION=VBCC
VERSION=0.5
CPU=-j16

WORKSPACE="`pwd`"
SOURCES=$WORKSPACE/_sources
LOGFILES=$WORKSPACE/_logs
PREFIX=$WORKSPACE/ApolloCrossDev
TARGET=bin
NDK=include
export PATH=$PREFIX/$TARGET:$PATH

VBCC_VC=http://www.ibaug.de/vbcc/vbcc.tar.gz
VBCC_M68K=http://phoenix.owl.de/vbcc/current/vbcc_target_m68k-amigaos.lha
VASM=http://sun.hasenbraten.de/vasm/daily/vasm.tar.gz
VDA_M68K=http://sun.hasenbraten.de/~frank/projects/download/vdam68k.tar.gz
VLINK=http://sun.hasenbraten.de/vlink/daily/vlink.tar.gz
CONFIG=http://phoenix.owl.de/vbcc/2022-05-22/vbcc_unix_config.tar.gz

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev $EDITION Edition v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# PART 1: Clean the House
sudo echo -e "\e[1m\e[37m1. Clean the House\e[0m\e[36m"
rm -f -r $PREFIX
mkdir -p $PREFIX
rm -f -r $LOGFILES
mkdir -p $LOGFILES
rm -f -r $SOURCES
mkdir -p $SOURCES
cd $SOURCES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential gawk flex bison expect dejagnu texinfo lhasa git subversion >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Download Sources
cd $SOURCES
echo -e "\e[1m\e[37m3. Download Sources\e[0m\e[36m"
echo -e "   * VBCC Compiler" 
wget -nc $VBCC_VC -a $LOGFILES/part3.log
echo "   * VBCC M68K Target" 
wget -nc $VBCC_M68K -a $LOGFILES/part3.log
echo "   * VASM Compiler" 
wget -nc $VASM -a $LOGFILES/part3.log
echo "   * VLINK Binary Linker" 
wget -nc $VLINK -a $LOGFILES/part3.log
echo "   * UNIX Config Files" 
wget -nc $VDA_M68K -a $LOGFILES/part3.log
echo "   * UNIX Config Files" 
wget -nc $CONFIG -a $LOGFILES/part3.log

# PART 4: Unpack VBCC-Sources
echo -e "\e[1m\e[37m4. Unpack Sources\e[0m\e[36m"
for f in *.tar*; do tar xfk $f >>$LOGFILES/part4.log 2>>$LOGFILES/part4_err.log; done 

# Part 5: Compile VBCC
echo -e "\e[1m\e[37m5. Compile VBCC\e[0m\e[36m"
cd vbcc
echo "   * Install"
mkdir -p bin
for i in {1..29}
do
  echo >>input.txt
done
make TARGET=m68k >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log <input.txt
make TARGET=m68ks >>$LOGFILES/part5.log 2>>$LOGFILES/part5_err.log <input.txt
cd ..

# Part 6: Configure M68K Target
echo -e "\e[1m\e[37m6. Configure M68K Target\e[0m\e[36m"
echo "   * Installing LHA" 
sudo apt -y install lhasa >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log
echo "   * Extracting Archive" 
lha -x vbcc_target_m68k-amigaos.lha >>$LOGFILES/part6.log 2>>$LOGFILES/part6_err.log

# Part 7: Compile VASM
echo -e "\e[1m\e[37m7. Compile VASM\e[0m\e[36m"
cd vasm
echo "   * Install"
make CPU=m68k SYNTAX=mot >>$LOGFILES/part7.log 2>>$LOGFILES/part7_err.log 
cd ..

# Part 8: Compile VLINK
echo -e "\e[1m\e[37m8. Compile VLINK\e[0m\e[36m"
cd vlink
echo "   * Install"
make >>$LOGFILES/part8.log 2>>$LOGFILES/part8_err.log 
cd ..

# Part 9: Compose bin
echo -e "\e[1m\e[37m9. Compose bin\e[0m\e[36m"
mkdir -p $PREFIX/$TARGET
cp vbcc/bin/vbcc* vbcc/bin/vc vbcc/bin/vprof $PREFIX/$TARGET
cp vasm/vasmm68k_mot vasm/vobjdump $PREFIX/$TARGET
cp vlink/vlink $PREFIX/$TARGET
cp -r vbcc_target_m68k-amigaos/* $PREFIX/$TARGET
cp config/* $PREFIX/$TARGET/config
rm $PREFIX/$TARGET/Install*

# Part 10: Download NDK's
echo -e "\e[1m\e[37m10. Download NDK's\e[0m\e[36m"
echo "   * Installing LHA" 
sudo apt -y install lhasa >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc http://aminet.net/dev/misc/NDK3.2.lha -a $LOGFILES/part10.log
echo "   * Extracting Archive" 
lha -xw=$PREFIX/$NDK/NDK3.2 NDK3.2.lha >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cd ..
mkdir NDK3.9
cd NDK3.9
echo "   * Download NDK3.9 from os.amigaworld.de" 
wget -nc https://os.amigaworld.de/download.php?id=3 -a $LOGFILES/part10.log
mv download.php?id=3 NDK39.lha
echo "   * Extracting Archive" 
lha -xw=$PREFIX/$NDK NDK39.lha >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
mv $PREFIX/$NDK/NDK_3.9 $PREFIX/$NDK/NDK3.9
rm -r $PREFIX/$NDK/ndk_3.9
rm $PREFIX/$NDK/NDK_3.9.info
cd ..
mkdir NDKMUI
cd NDKMUI
echo "   * Download MUI 5.0 SDK from GitHub" 
wget -nc https://github.com/amiga-mui/muidev/releases/download/MUI-5.0-20210831/MUI-5.0-20210831-os3.lha -a $LOGFILES/part10.log
echo "   * Extracting Archive" 
lha -x MUI-5.0-20210831-os3.lha >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
mv SDK/MUI $PREFIX/$NDK/NDKMUI
cd ..
cd $PREFIX/$NDK
echo "   * Cloning ApolloDevPac" 
git clone --progress https://github.com/WDrijver/ApolloDevPac 2>>$LOGFILES/part10_err.log
cd ..

# FINISH
echo " "
echo -e "\e[1m\e[32mFINISHED\e[0m"
echo " "
exit
