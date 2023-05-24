    #include <proto/exec.h>
    #include <exec/libraries.h>
    #include <dos/dos.h> 
    #include <proto/intuition.h>
    #include <intuition/intuition.h> 
    
    struct IntuitionBase *IntuitionBase;
    
    int main(int argc, char *argv[])
    {
      
        struct EasyStruct es = {      sizeof(struct EasyStruct), 0UL,      "Requester", "Hello Apollo", "Ok"    };
      
        IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 36);
    
        if (!IntuitionBase)  return RETURN_FAIL;
    
        EasyRequestArgs(NULL, &es, NULL, NULL);
        CloseLibrary((struct Library *)IntuitionBase);
    
        return RETURN_OK;
    }
