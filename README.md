ApolloCrossDev is intended to help (future) Amiga/Apollo developers to setup Cross Compilers on Linux (Debian/Ubuntu x64 architecture only), offering a selection of the best toolchains which I  reverse engineered from the excellent work by some of the most skilled and dedicated Amiga developers in the past 20 years;

* GCC-2.95.3        : based on toolchain-m68k by Krystian Bacławski (cahirwpz)
* GCC-3.4.6         : based on NetSurf Toolchain by NetSurf Development Team
* GCC 6.5.0         : based on Amiga-GCC by Stefan Franke (Bebbo)
* GCC 12.2          : based on vscode-amiga-debug from Abyss (Bartman)
* VBCC/VASM/VLINK   : based on Hasenbraten VBCC from Dr.Barthelmann & Frank Wille

Requirements:
1. Debian or Ubuntu x64 architecture installed in a Virtual Machine
2. Microsoft Visual Studio Code downloaded and installed
3. Basic Git tools installed from CLI (sudo apt install git)
4. ApolloExplorer (https://github.com/ronybeck/ApolloExplorer)

Installation:
1. Open Microsoft Visual Studio Code
2. Clone ApolloCrossDev: Copy repository hyperlink | CTRL+Shift+P git clone | Paste repository hyperlink
3. Click "Open in New Window" | Click "Compilers" + "ApolloCrossDev.code-workspace" + "Open Workspace"
4. Open Terminal: Right-click "Compilers" | Select "Open in integrated Terminal"
5. Enter Compiler directory (example: "cd GCC-6.50") and start Install script (example: "./GCC-6.50.sh")

Instructions:
1. Create Projects/mysource directory for your *.c and *.s sourcefiles (example: Projects/hello)
2. Copy your choice of make-xxxyyy makefile(s) (example: Projects/hello/make-gcc2953 to use GCC 2.95.3)
3. Type "make -f make-xxxyyy" or read make file for Compile instructions (example: make -f make-gcc346)
