ApolloCrossDev is intended to help (future) Amiga/Apollo developers to setup a Cross Compiler in a Virtual Ubuntu or Debian Linux.

**Content:**
1. GCC 6.5.0 Toolchain (Apollo optimised Fork from excellent Amiga-GCC by Stefan -Bebbo- Franke)
2. VASM Assembler (Apollo optimised Assembler from Dr. Volker Barthelsmann & Frank Wille) 
3. Amiga OS Native Development Kits (1.3, 3.9 and 3.2)
4. Additional Software Development Kits (SDL, FreeType, Open-GL, MUI5)
5. GDB-Server running native on Apollo V4 for remote debugging on Linux in Visual Studio Code
6. C/C++/ASM Source-Code examples with automated Build from Makefiles and remote Debug enabled
7. Apollo Library with some basic Apollo C++ and ASM routines, including Debug on Serial Output (TTY)

**Requirements:**
1. Ubuntu 24.04.3 LTS amd64 (Intel) is the supported Linux (25.10 will fail) installed in a VM (8Gb+ RAM|50Gb+ HD|4+ Cores) 
2. Git tools installed from CLI (open terminal and type "sudo apt install git")
3. Microsoft Visual Studio Code (VS-Code) downloaded and installed (https://code.visualstudio.com)
4. Microsoft Visual Studio Code Extensions:
   - Microsoft C/C++ (Intellisense, debugging and code browsing)
   - Microsoft C/C++ Extension Pack (collection of popular extensions)
   - Microsoft C/C++ Themes (semantic colorization)
   - Microsoft Makefile Tools (support for C/C++ Makefiles)
   - Amiga Assembly by Paul Raingeard (Amiga ASM Support) 

**Installation:**
1. Open VS-Code Click "View" and select "Open Command Palette" (or press CTRL+Shift+P)
2. In drop-down list Select "git clone" and type "https://github.com/WDrijver/ApolloCrossDev"  
3. After download Click "Open" and accept "trust the authors" and Click "Open Workspace" in the right corner popup window
4. Select "Terminal" in the menu and then "New Terminal"
5. In the terminal windows type "./GCC-6.50.sh" to install the Toolchain
6. After the installation is finished type "sudo nano ~/.bashrc"
7. At the end of the file add: export AMIGAHOST="IP-ADDRESS" and save (for example: export AMIGAHOST ="192.168.2.100")
8. Close the terminal window, close Visual Studio Code 

**Test Compiler and File-Transfer:**
1. Click "Terminal" in menu and select "Run Build Task" (or press CTRL-Shift-B)
2. Select "ApolloDemo" from the drop-down List
3. DoubleClick ApolloDemo Icon on your Apollo V4 (in the target path specified in tasks.json)

**Test Remote GDB Debugger:**
1. Set Breakpoint(s) in <projectname> source code by clicking left from the line number (red dot appears)
2. Select "Run" and select "Start Debugging" (or press F5)
3. Select the <projectname> you want to Remote Debug from the drop-down List
4. DoubleClick your <projectname> Icon on your Apollo V4 (in the target path specified in tasks.json)
5. Use Debug Controls (step-over, step-into, etc.) to 
6. Open 

**Test Serial Terminal Debugger:**
(NOTE: Serial Debug requires PL2303 USB to Serial cable + TTY Terminal Program | Settings: 115200-8-1-N)

**Create a new Project:**
1. Right-Click Projects Folder and select "New Folder..." to create a new <projectname> Folder
2. Right-Click <projectname> Folder and select "New File..." to create .c .cpp and .s source files
3. Copy make-gcc650 template from Projects/_makefiles to <projectname> Folder
4. Change the Project/<projectname> line and make other customizations to make-gcc650 if needed
5. Create your C and/or Assembler code using .c for C, .cpp for C++ and .s for ASM
6. Right-Click <projectname> Folder and select "Open in integrated Terminal"
7. Type "make -f make-gcc650" to Compile

TIP:  make-gcc contains additional information on Compiler Options
      please take you time to read this information carefully if your new to GCC/Amiga

--------------------------------------------------------------------------------------------------------------------

For further ApolloCrossDev installation help a video guide is available at: <TODO>>

**Other Resources:**
1. Apollo 68080 AMMX/SAGA Basic Coding Documentation  : http://apollo-core.com/index.htm?page=coding
2. Tommo's Apollo Developer Documentation Links       : 
3. ApolloOS - Apollo V4 Open Source OS Distro Image   : http://www.apollo-computer.com/downloads.php

**Apollo 68080 C/C++ opcode support:**
C/C++ Compiler current opcode support for Apollo 68080    : cmpiw.l + addiw.l + dbra.l + bcc.s+
C/C++ Compiler future  opcode support for Apollo 68080    : moviw.l + mov3q + clr.q + move2.b + movs.b/w + movz.b/w

