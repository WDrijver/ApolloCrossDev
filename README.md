# ApolloCrossDev is intended to help (future) Amiga/Apollo developers to setup a Cross Compiler in a Virtual Ubuntu Linux.

## Content:
ApolloCrossDev is based on the excellent Amiga-GCC repositories by Stefan -Bebbo- Franke
1. BinUtils (ar=archiver | as=GNU ASM | ld=linker | nm=symbols | objdump=objects | ranlib=libindex)
2. GNU-GCC Compilers for C and C++ (6.50 Stable | **6.50 Latest** | 13.1/2/3 Beta | 15.2 Beta)
3. VASM Assembler (Apollo optimised Assembler from Dr. Volker Barthelsmann & Frank Wille) 
4. Amiga OS Native Development Kits (1.3, 3.9 and 3.2 = default)
5. Libraries/Includes (MUI5, SDL-Base+Mixer+TTF+Image, Vorbis, Ogg, GL, FreeType, Timidity, ZLib, JPeg, PNG)
6. GDB-Server running native on Apollo V4 for remote debugging on Linux in Visual Studio Code
7. C/C++/ASM Source-Code examples with automated Build from Makefiles and remote Debug enabled
8. Apollo Sourcecode collection with some basic Apollo C++ and ASM routines, including Debug on Serial Output (TTY)
9. ApolloExplorer = Great Client/Server File Transfer Utility created by @ronybeck 

## Requirements:
1. Ubuntu 24.04.3 LTS amd64 (Intel) is the supported Linux (25.10 will fail) installed in a VM (8Gb+ RAM|50Gb+ HD|4+ Cores) 
2. Git tools installed from CLI: open terminal and type: sudo apt install git
3. Microsoft Visual Studio Code (VS-Code) downloaded and installed (https://code.visualstudio.com)
4. Microsoft Visual Studio Code Extensions:
   - Microsoft C/C++ (Intellisense, debugging and code browsing)
   - Microsoft C/C++ Themes (UI Theme Pack - high contrast recommeded)
   - Microsoft C/C++ Extension Pack (collection of popular extensions)
   - Microsoft Makefile Tools (support for C/C++ Makefiles)

## Installation:
1. Open VS-Code Click "View" and select "Open Command Palette" (or press CTRL+Shift+P)
2. In drop-down list Select "git clone" and type https://github.com/WDrijver/ApolloCrossDev  
3. After download Click "Open" and accept "trust the authors" and Click "Open Workspace" in the right corner popup window
4. Select "Terminal" in the menu and then "New Terminal"
5. In the terminal windows type ./GCC-6.50-Latest.sh to install the current ApolloCrossDev Toolchain
6. After the installation is finished type sudo nano ~/.bashrc
7. At the end of the file add: export AMIGAHOST="IP-ADDRESS" and save (for example: export AMIGAHOST ="192.168.2.100")

## Test Compiler and File-Transfer:
1. Click "Terminal" in menu and select "Run Build Task" (or press CTRL-Shift-B)
2. Select "ApolloDemo" from the drop-down List
3. Watch ApolloCrossDev Compile (Step #1) + Link (Step #2) and Upload (Step #3) the ApolloDemo Project
4. DoubleClick RAM Disk on your Apollo V4 and then doubleclick ApolloDemo to experience Apollo V4 power

## Test Remote GDB Debugger:
1. Set Breakpoint(s) in <projectname> source code by clicking left from the line number (red dot appears)
2. Select "Run" and select "Start Debugging" (or press F5)
3. Select the <projectname> you want to Remote Debug from the drop-down List
4. DoubleClick your <projectname> Icon on your Apollo V4 (in the target path specified in tasks.json)
5. Use Debug Controls (step-over, step-into, etc.) to 
6. Open 

## Test Serial Terminal Debugger:
(NOTE: Serial Debug requires PL2303 USB to Serial cable + TTY Terminal Program | Settings: 115200-8-1-N)

## Create a new Project:
1. Right-Click Projects Folder and select "New Folder..." to create a new <projectname> Folder
2. Right-Click <projectname> Folder and select "New File..." to create .c .cpp and .s source files
3. Copy Makefile template from Projects/_makefiles to <projectname> Folder
4. Change the Project/<projectname> line and make other customizations to Makefile if needed
5. Create your C and/or Assembler code using .c for C, .cpp for C++ and .s for ASM
6. Right-Click <projectname> Folder and select "Open in integrated Terminal"
7. Type "make" to Compile

TIP:  The Makefile template contains additional information on Compiler Options
      please take you time to read this information carefully if your new to GCC/Amiga

--------------------------------------------------------------------------------------------------------------------

## Other Resources:
1. Docs folder containing a collection of Amiga and Apollo Reference Manuals (PDF)
2. Apollo 68080 AMMX/SAGA Basic Coding Documentation  : http://apollo-core.com/index.htm?page=coding
3. ApolloOS - Apollo V4 Open Source OS Distro Image   : http://www.apollo-computer.com/downloads.php

## Apollo 68080 C/C++ opcode support:
C/C++ Compiler current opcode support for Apollo 68080    : cmpiw.l + addiw.l + dbra.l + moviw.l + bcc.s+ 
C/C++ Compiler future  opcode support for Apollo 68080    : mov3q + clr.q + move2.b + movs.b/w + movz.b/w

