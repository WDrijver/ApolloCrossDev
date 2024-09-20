ApolloCrossDev is intended to help (future) Amiga/Apollo developers to setup Cross Compilers on Ubuntu Linux, offering a selection of the best toolchains which I  reverse engineered from the excellent work by some of the most skilled and dedicated Amiga developers in the past 20 years;

- GCC-2.95.3        : based on toolchain-m68k by Krystian Bacławski (cahirwpz)
- GCC-3.4.6         : based on NetSurf Toolchain by NetSurf Development Team
- GCC 6.5.0         : based on Amiga-GCC by Stefan Franke (Bebbo) - PREFERRED CHOICE (*)
- GCC 12.2          : based on vscode-amiga-debug from Abyss (Bartman)
- VBCC/VASM/VLINK   : based on Hasenbraten VBCC from Dr.Barthelmann & Frank Wille

(*) Apollo Development Team advises to work with the GCC 6.5.0 "Stable" Toolchain (see below).

Requirements:
1. Ubuntu 24.x LTS amd64 (Intel) | arm64 (Mac Silicon) installed in a VM (8Gb+ RAM|100Gb+ HD|8+ Cores) 
2. Microsoft Visual Studio Code (VS-Code) downloaded and installed (https://code.visualstudio.com)
3. Git tools installed from CLI (open terminal and type "sudo apt install git")
4. ApolloExplorer downloaded and installed (https://github.com/ronybeck/ApolloExplorer)

Installation:
1. Open VS-Code, Open Command Palette (CTRL+Shift+P), Select "git clone" and type "https://github.com/WDrijver/ApolloCrossDev"  
2. After download Click "Open" and accept "trust the authors" and Click "Open Workspace" in the right corner popup window
3. Click "Compilers" in the Explorer (left) to collapse all supported Compiler Toolchains
4. Right-Click on the Compiler of Choice (Preferredd = GCC-6.50-Stable) and Select "Open in integrated Terminal"
5. In the terminal windows type "./<Compiler>.sh" to install (for example: "./GCC-6.50-Stable.sh")

For further ApolloCrossDev installation help a video guide is available at: xxx

Test Compiler:
1. Right-click Projects/Hello-Intuition folder and choose "Open in Integrated Terminal"
2. Type "make -f <Makefile Name>" to compile (for example: "make -f Makefile-GCC-6.50")
3. Open ApolloExplorer Client in Ubuntu and Open ApolloExplorer Server on Apollo V4
4. Browse to the preferred location on V4 and drag Hello-Intuition file 

Create Project:
1. Create Projects/<mysource> folder to include all your *.c and *.s sourcefiles (example: Projects/hello)
2. Copy makefile template from Project/_makefiles into your folder (example: Projects/hello/make-gcc650-stable)
3. Type "make -f make-xxxyyy" or read make file for further Compile instructions (example: make -f make-gcc650-stable)

GCC-6.50:
Apollo Team has chosen GCC 6.50 as preferred version for developing software for Apollo V4 Series.
GCC-6.50-Stable is the Apollo controlled version that is advised for your Cross Development Compiler.
GCC-6.50-Apollo-XXXX versions are all alpha/beta versions for testing new Apollo 68080 instructions.
GCC-6.50-Apollo-ALL1 is the alpha/beta version in which all new Apollo-XXXX instructions are included.
GCC-6.50-Latest is the latest version from Stefan -Bebbo- Franke repository, not under Apollo control.

LIBRARIES: ApolloCrossDev GCC-6.50-Stable includes cross compiling SDL v1.2 and MUI 5.0 libraries
INCLUDES : ApolloCrossDev GCC-6.50-Stable includes AmigaOS 3.9 | 3.2 NDK (C/C++) and DevPac (ASM)

Other Resources:
1. Apollo 68080 AMMX/SAGA Basic Documentation            : http://apollo-core.com/index.htm?page=coding
2. Apollo Devpac - Native V4 MonAm Assembler + Debugger  : Integrated in ApolloOS
3. Apollo ASM-ONE - Native V4 Assembler                  : Integrated in ApolloOS
4. ApolloOS - Apollo V4 Open Source OS Distro Image      : http://www.apollo-core.com/downloads/ApolloOS_R9.4.2-Release.img.zip
