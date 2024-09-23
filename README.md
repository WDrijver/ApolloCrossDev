ApolloCrossDev is intended to help (future) Amiga/Apollo developers to setup a Cross Compiler in a Virtual Ubuntu Linux.

Content:
1. GCC 6.5.0 Toolchain (Apollo optimised Fork from excellent Amiga-GCC by Stefan -Bebbo- Franke)
2. VASM Assembler (Apollo optimised Assembler from Dr. Volker Barthelsmann & Frank Wille) 
3. Amiga OS NDK's (1.3, 3.9 and 3.2)
4. Additional SDK (SDL, FreeType, Open-GL, MUI5)  

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

Other Resources:
1. Apollo 68080 AMMX/SAGA Basic Documentation            : http://apollo-core.com/index.htm?page=coding
2. Apollo Devpac - Native V4 MonAm Assembler + Debugger  : Integrated in ApolloOS
3. Apollo ASM-ONE - Native V4 Assembler                  : Integrated in ApolloOS
4. ApolloOS - Apollo V4 Open Source OS Distro Image      : http://www.apollo-core.com/downloads/ApolloOS_R9.4.2-Release.img.zip
