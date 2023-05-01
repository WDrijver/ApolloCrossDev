ApolloCrossDev will help (future) Amiga/Apollo developers to setup Cross Compilers on Linux (Debian/Ubunto 64-bits),
offering selection of the best toolchains created by skilled Amiga developers in the past 20 years.

* GCC-2.95.3        : Krystian Bacławski (cahirwpz)
* GCC-3.4.6         : NetSurf Development Team
* GCC 6.5.0         : Stefan Franke (Bebbo)
* GCC 12.2          : Bartman (Abyss)
* VBCC/VASM/VLINK   : Dr.Barthelmann & Frank Wille

Requirements:
1. Debian of Ubuntu 64-bit Linux Virtual Machine
2. Microsoft Visual Studio Code
3. Basic Git tools (sudo apt-get install git)
4. ApolloExplorer

Installation:
1. Open Microsoft Visual Studio Code
2. Clone ApolloCrossDev: copy repository hyperlink | CTRL+Shift+P git clone | paste repository hyperlink
3. Click "Open in New Window" | Right-click "Compilers" | Select "Open in integrated Terminal"
4. Enter each Compiler directory (example: "cd GCC-2.95") and start Install script (example: "./GCC-2.95.sh")

Instructions:
1. Create Projects/<mysource> directory for your *.c and *.s sourcefiles (example: Projects/hello)
2. Copy Project/make-<compiler> file(s) to <mysource> (example: Projects/hello/make-gcc295 to use GCC 2.95.3)
3. Type "make -f make-<mycompiler>" or read make-<compiler> file for further Compile instructions
