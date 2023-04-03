/********************************************************************
 *** 
 *** Program:  ApolloWHDSet.h
 *** Purpose:  GUI to set WHDLoad game tooltypes
 *** Authors:  Flype and TuKo
 *** Target:   AmigaOS 3.x
 *** Compiler: GCC 2.95.3, SAS/C 6.59 
 *** 
 ********************************************************************/

#include <exec/types.h>

/********************************************************************
 *** 
 *** SAS/C
 *** 
 ********************************************************************/

#ifdef __SASC
#define REG(x) register __ ## x
#define ASM    __asm
#define SAVEDS __saveds
int CXBRK(void) { return (0); }
int _CXBRK(void) { return (0); }
void chkabort(void) { }
#endif

/********************************************************************
 *** 
 *** APP
 *** 
 ********************************************************************/

#define APP_NAME        "ApolloWHDSet"
#define APP_BASE        "APOLLOWHDSET"
#define APP_DATE        "27.2.2023"
#define APP_VERSION     "0.3.3"
#define APP_VERSTRING   APP_NAME " " APP_VERSION " (" APP_DATE ")"
#define APP_AUTHORS     "Flype, based on ideas from TuKo, Willem"
#define APP_COPYRIGHT   "Copyright 2023 by " APP_AUTHORS
#define APP_DESCRIPTION "GUI to set WHDLoad game tooltypes"
#define APP_HELPFILE    "PROGDIR:ApolloWHDSet.guide"

#define MAX_PATHNAME    (1024)
#define MAX_TOOLTYPES   (4096)

/********************************************************************
 *** 
 *** WHDLOAD
 *** 
 ********************************************************************/

#define WHDLOAD_NAME     "WHDLoad"
#define WHDLOAD_TT_NAME  "ExecutePostDisk"
#define WHDLOAD_TT_TOOL  "C:ApolloControl"

/********************************************************************
 *** 
 *** MUI
 *** 
 ********************************************************************/

#ifndef IPTR
#define IPTR ULONG
#endif

#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

#define MakeCycle(a) (CycleObject, \
    MUIA_Frame, MUIV_Frame_Button, \
    MUIA_Cycle_Entries, a, End)

#define MakeMenuBar() (MUI_MakeObject(MUIO_Menuitem, \
    NM_BARLABEL, 0, 0, 0))

#define MakeMenuItem(a, b) (MenuitemObject, \
    MUIA_Menuitem_Title, a, \
    MUIA_Menuitem_Shortcut, b, End)

#define MakeSlider(a, b, c) (SliderObject, \
    MUIA_Frame, MUIV_Frame_Slider, \
    MUIA_Slider_Min, b, \
    MUIA_Slider_Max, c, \
    MUIA_Slider_Level, a, End)

#define MakeText(a) (TextObject, \
    MUIA_Background, MUII_TextBack, \
    MUIA_Frame, MUIV_Frame_Text, \
    MUIA_Text_Contents, a, \
    MUIA_Text_SetMin, TRUE, End)

#define ButtonPressed(a, b) DoMethod(a, \
    MUIM_Notify, MUIA_Pressed, FALSE, \
    object->App, 2, MUIM_Application_ReturnID, b)

#define CheckMarkChange(a, b) DoMethod(a, \
    MUIM_Notify, MUIA_Selected, MUIV_EveryTime,  \
    b, 3, MUIM_Set, MUIA_Disabled, MUIV_NotTriggerValue)

#define MenuItemTrigger(a, b) DoMethod(a, \
    MUIM_Notify, MUIA_Menuitem_Trigger, MUIV_EveryTime, \
    object->App, 2, MUIM_Application_ReturnID, b)

/********************************************************************
 *** 
 *** EVENTS
 *** 
 ********************************************************************/

typedef enum
{
    EVENT_ABOUT = 100,
    EVENT_ABOUT_MUI,
    EVENT_COPY,
    EVENT_DISABLE_ALL,
    EVENT_ENABLE_ALL,
    EVENT_ICONIFY,
    EVENT_INFO,
    EVENT_LAUNCH,
    EVENT_OPEN,
    EVENT_PASTE,
    EVENT_QUIT,
    EVENT_RELOAD,
    EVENT_SAVE,
    EVENT_SETTINGS,
    EVENT_PRESET = 200,
} ProcessEvent_t;

/********************************************************************
 *** 
 *** PRESETS
 *** 
 ********************************************************************/

typedef struct Preset
{
    LONG    pr_GameSpeed;
    LONG    pr_Superscalar;
    LONG    pr_DisplayMode;
    LONG    pr_ZoomMode;
    LONG    pr_ZoomVerticalShift;
    LONG    pr_ZoomHorizontalShift;
    LONG    pr_ForceMode;
    LONG    pr_Scanlines;
    APTR    pr_MN_Item;
    STRPTR    pr_MN_Shortcut;
    STRPTR    pr_MN_Label;
} Preset_t;

/********************************************************************
 *** 
 *** PROFILES
 *** 
 ********************************************************************/

typedef struct Profile
{
    // Enabled
    ULONG    pr_bGameSpeed;
    ULONG    pr_bSuperscalar;
    ULONG    pr_bDisplayMode;
    ULONG    pr_bZoomMode;
    ULONG    pr_bZoomVerticalShift;
    ULONG    pr_bZoomHorizontalShift;
    ULONG    pr_bForceMode;
    ULONG    pr_bScanlines;
    // Values
    ULONG    pr_lGameSpeed;
    ULONG    pr_lSuperscalar;
    ULONG    pr_lDisplayMode;
    ULONG    pr_lZoomMode;
    ULONG    pr_lZoomVerticalShift;
    ULONG    pr_lZoomHorizontalShift;
    ULONG    pr_lForceMode;
    ULONG    pr_lScanlines;
} Profile_t;

/********************************************************************
 *** 
 *** OBJAPP
 *** 
 ********************************************************************/

typedef struct ObjApp
{
    APTR          App;
    APTR          MN_Main;
    APTR          WI_Main;
    UBYTE         Buffer[MAX_PATHNAME];
    UBYTE         Filename[MAX_PATHNAME];
    Profile_t     ClipBoard[2];
    CONST_STRPTR  DisplayMode[3];
    CONST_STRPTR  GameSpeed[4];
    CONST_STRPTR  Scanlines[3];
    CONST_STRPTR  ForceMode[3];
    CONST_STRPTR  Superscalar[3];
    CONST_STRPTR  ZoomMode[5];
} ObjApp_t;

/********************************************************************
 *** 
 *** END OF FILE
 *** 
 ********************************************************************/
