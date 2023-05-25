#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <intuition/screens.h>

struct IntuitionBase *IntuitionBase;

int main(void) {

    struct NewScreen Screen1 = {
     0,0,640,480,8,             /* Screen of 640 x 480 of depth 8 (2^8 = 256 colours)    */
     DETAILPEN, BLOCKPEN,
     HIRES,                     /* see graphics/view.h for view modes */
     PUBLICSCREEN,              /* Screen types */
     NULL,                      /* Text attributes (use defaults) */
     "My First Screen", 
     NULL,
     NULL
     };
     struct Screen *MyFirstScreen;
     struct Screen *MySecondScreen;

     IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 36);
     if (!IntuitionBase)  return RETURN_FAIL;

     MyFirstScreen = OpenScreen(&Screen1);
     Delay(500);
     CloseScreen(MyFirstScreen); 

     MySecondScreen = OpenScreenTags(NULL, 
     SA_Left, 0, SA_Top, 0, SA_Width, 640, SA_Height, 480,
     SA_Depth, 8, SA_Title, "My Second Screen",
     SA_Type, PUBLICSCREEN,
     SA_SysFont, 1,
     TAG_DONE);
   
     Delay(500);
     CloseScreen(&MySecondScreen);
     return (0);
}
