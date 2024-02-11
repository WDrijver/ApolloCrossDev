#include <proto/exec.h>
#include <proto/dos.h>
//#include <clib/exec_protos.h>

int main(int argc, void *argv[])
{
    struct Library *SysBase;
    struct Library *DOSBase;

    SysBase = *((struct Library **)4UL);
    DOSBase = OpenLibrary("dos.library", 0);

    if (DOSBase) {
        Write(Output(), "Hello Amiga!\n", 13);
        CloseLibrary(DOSBase);
    }

    return(0);
}
