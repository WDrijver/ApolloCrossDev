# Bebbo's gdbserver for the Amiga

V1.0: seems to be usable - going public :-)

# License
bgdbserver is published under GNU GENERAL PUBLIC LICENSE V3 - see COPYING

# Usage
Build the executable and copy it to your Amiga.
## Debug a a distinct program (with optional parameters):

Run

    bgdbserver test 1 2 3 4

This will listen on port 2345. To use a different port specify it as first argument with a colon

    bgdbserver :3456 test 1 2 3 4

Now port 3456 is used.

Run m68k-amigaos-gdb on the debugger side and enter

    target remote :2345

If you specifed a different port use the latter one.
This will connect gdb with your program.

Happy debugging.

## Run a rsh fake shell

This was developed for a better integration into Eclipse. Now Eclipse is capable to command which executable is the debug target.

Run

    bgdbserver

And a very rsh fake is listening on port 514. Just enough to get the command from Eclipse to launch the bgdbserver for the given command. Read more here: https://franke.ms/amiga/gdb-eclipse.wiki

It's again possible to use a different port:

    bgdbserver :987

will run the rsh fake on port 987.

# Version Infos
## 1.1
added support to break a running program using CTRL+C
## 1.2
fix breaking a running program 
## 1.3
ensure break is inside segment list
