# ApolloCrossDev Build Script v0.2

EDITION=VBCC
VERSION=0.2
CPU=-j16

VBCC_VC=http://www.ibaug.de/vbcc/vbcc.tar.gz
VBCC_M68K=http://phoenix.owl.de/vbcc/current/vbcc_target_m68k-amigaos.lha
VASM=http://sun.hasenbraten.de/vasm/daily/vasm.tar.gz
VLINK=http://sun.hasenbraten.de/vlink/daily/vlink.tar.gz
CONFIG=http://phoenix.owl.de/vbcc/2022-05-22/vbcc_unix_config.tar.gz

PREFIX="`pwd`/bin"
LOGFILES="`pwd`/logs"
SOURCEFILES="`pwd`/sources"
NDKFILES="`pwd`/ndk"
SETTINGS="`pwd`/../Sources/.vscode"

# INIT Terminal
clear
echo -e "\e[1m\e[37m########## \e[31mApollo\e[1;30mCrossDev $EDITION Edition v$VERSION \e[37m ##########\e[0m\e[36m"
echo " "
echo -e "\e[1m\e[37m0. Sudo Password\e[0m"

# PART 1: Clean the House
sudo echo -e "\e[1m\e[37m1. Clean the House\e[0m\e[36m"
rm -f -r $PREFIX $LOGFILES $SOURCEFILES $NDKFILES $SETTINGS/.bashrc
mkdir -p $PREFIX $LOGFILES $SOURCEFILES $NDKFILES

# PART 2: Update Linux Packages 
echo -e "\e[1m\e[37m2. Update Linux Packages\e[0m\e[36m"
sudo apt -y update >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log
sudo apt -y install build-essential git subversion >>$LOGFILES/part2.log 2>>$LOGFILES/part2_err.log

# PART 3: Download VBCC Sources
cd $SOURCEFILES
echo -e "\e[1m\e[37m3. Download VBCC-Sources\e[0m\e[36m"
echo -e "   * VBCC Compiler" 
wget -nc $VBCC_VC -a $LOGFILES/part3.log
echo "   * VBCC M68K Target" 
wget -nc $VBCC_M68K -a $LOGFILES/part3.log
echo "   * VASM Compiler" 
wget -nc $VASM -a $LOGFILES/part3.log
echo "   * VLINK Binary Linker" 
wget -nc $VLINK -a $LOGFILES/part3.log
echo "   * UNIX Config Files" 
wget -nc $CONFIG -a $LOGFILES/part3.log

# PART 4: Unpack VBCC-Sources
echo -e "\e[1m\e[37m4. Unpack VBCC-Sources\e[0m\e[36m"
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
cp vbcc/bin/vbcc* vbcc/bin/vc vbcc/bin/vprof $PREFIX
cp vasm/vasmm68k_mot vasm/vobjdump $PREFIX
cp vlink/vlink $PREFIX
cp -r vbcc_target_m68k-amigaos/* $PREFIX
cp config/* $PREFIX/config
rm $PREFIX/Install*

# Part 10: Download NDK's
echo -e "\e[1m\e[37m10. Download NDK's\e[0m\e[36m"
echo "   * Installing LHA" 
sudo apt -y install lhasa >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
mkdir NDK3.2
cd NDK3.2
echo "   * Download NDK3.2.lha from AmiNet" 
wget -nc http://aminet.net/dev/misc/NDK3.2.lha -a $LOGFILES/part10.log
echo "   * Extracting Archive" 
lha -xw=$NDKFILES/NDK3.2 NDK3.2.lha >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
cd ..
mkdir NDK3.9
cd NDK3.9
echo "   * Download NDK3.9 from os.amigaworld.de" 
wget -nc https://os.amigaworld.de/download.php?id=3 -a $LOGFILES/part10.log
mv download.php?id=3 NDK39.lha
echo "   * Extracting Archive" 
lha -xw=$NDKFILES NDK39.lha >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
mv $NDKFILES/NDK_3.9 $NDKFILES/NDK3.9
rm -r $NDKFILES/ndk_3.9
rm $NDKFILES/NDK_3.9.info
cd ..
mkdir NDKMUI
cd NDKMUI
echo "   * Download MUI 5.0 SDK from GitHub" 
wget -nc https://github.com/amiga-mui/muidev/releases/download/MUI-5.0-20210831/MUI-5.0-20210831-os3.lha -a $LOGFILES/part10.log
echo "   * Extracting Archive" 
lha -x MUI-5.0-20210831-os3.lha >>$LOGFILES/part10.log 2>>$LOGFILES/part10_err.log
mv SDK/MUI $NDKFILES/NDKMUI
cd ..
cd $NDKFILES
echo "   * Cloning ApolloDevPac" 
git clone --progress https://github.com/WDrijver/ApolloDevPac 2>>$LOGFILES/part10_err.log
cd ..

# Part 11: Create ApolloCrossDev Terminal for VSC
echo -e "\e[1m\e[37m11. Create Terminal for VSC\e[0m\e[36m"
echo "   * Copy local .bashrc to $SETTINGS" 
cp $HOME/.bashrc $SETTINGS
echo "   * Adding VBCC=$PREFIX to $SETTINGS" 
echo "export VBCC=$PREFIX" >>$SETTINGS/.bashrc
echo "export PATH=\$VBCC:\$PATH" >>$SETTINGS/.bashrc

# FINISH
echo " "
echo -e "\e[1m\e[31mFINISHED\e[0m"
echo " "
echo -e "\e[1m\e[37mPlease restart VSC and enjoy \e[31mApollo\e[1;30mCrossDev\e[0m\e[36m"
echo " "
exit
