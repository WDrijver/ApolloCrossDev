/********************************************************************
 *** 
 *** Program:  ApolloWHDSet.c
 *** Purpose:  GUI to set WHDLoad game tooltypes
 *** Authors:  Flype and TuKo
 *** Target:   AmigaOS 3.x
 *** Compiler: GCC 2.95.3, SAS/C 6.59 
 *** 
 ********************************************************************/

#define MUI_OBSOLETE

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dos/dos.h>
#include <dos/dosasl.h>
#include <dos/dosextens.h>
#include <dos/rdargs.h>
#include <exec/exec.h>
#include <exec/types.h>
#include <exec/execbase.h>
#include <intuition/classes.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>
#include <libraries/asl.h>
#include <libraries/gadtools.h>
#include <libraries/mui.h>
#include <workbench/icon.h>
#include <workbench/startup.h>
#include <workbench/workbench.h>
#include <utility/tagitem.h>

#include <clib/alib_protos.h>
#include <clib/asl_protos.h>
#include <clib/dos_protos.h>
#include <clib/exec_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/graphics_protos.h>
#include <clib/icon_protos.h>
#include <clib/intuition_protos.h>
#include <clib/muimaster_protos.h>
#include <clib/utility_protos.h>

#include <proto/asl.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/gadtools.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/utility.h>

#include "ApolloWHDSet.h"

/********************************************************************
 *** READARGS
 ********************************************************************/

enum
{
    OPT_FILE,
    OPT_COUNT
};

/********************************************************************
 *** PRESETS
 ********************************************************************/

#define MAX_PRESETS (11)

Preset_t Presets[MAX_PRESETS] = 
{
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
//    TU  SS  DM  ZO  ZV  ZH  SC  ITEM SHORT LABEL
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    {  1,  0,  1,  3, 18, -1, -1, NULL, "0", "Safest"                     },
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
    {  0,  0,  1,  3, -1, -1, -1, NULL, "1", "Accelerated 320x256 (PAL)"  },
    {  1,  0,  1,  3, -1, -1, -1, NULL, "2", "A500 320x256 (PAL)"         },
    {  2,  0,  1,  3, -1, -1, -1, NULL, "3", "A1200 320x256 (PAL)"        },
    {  0,  0,  0,  2, -1, -1, -1, NULL, "4", "Accelerated 320x240 (NTSC)" },
    {  1,  0,  0,  2, -1, -1, -1, NULL, "5", "A500 320x240 (NTSC)"        },
    {  2,  0,  0,  2, -1, -1, -1, NULL, "6", "A1200 320x240 (NTSC)"       },
    {  0,  0,  0,  1, -1, -1, -1, NULL, "7", "Accelerated 320x200 (NTSC)" },
    {  1,  0,  0,  1, -1, -1, -1, NULL, "8", "A500 320x200 (NTSC)"        },
    {  2,  0,  0,  1, -1, -1, -1, NULL, "9", "A1200 320x200 (NTSC)"       },
//  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    
};

/********************************************************************
 *** PROTOTYPES
 ********************************************************************/

BOOL GetArgValue(STRPTR args, STRPTR arg, ULONG *value);

BOOL GetEnvDefaultDrawer(STRPTR buffer);
BOOL GetEnvAcceptPattern(STRPTR buffer);
BOOL GetEnvRejectPattern(STRPTR buffer);
BOOL GetEnvWBInfo(STRPTR buffer);
BOOL GetEnvWBLoad(STRPTR buffer);

BOOL LoadFromIcon(STRPTR fullname);
BOOL LoadFromReadArgs(STRPTR fullname);
BOOL LoadFromRequester(STRPTR fullname);

VOID ProcessEvents(VOID);
VOID ProcessEvent_About(VOID);
VOID ProcessEvent_AboutMUI(VOID);
VOID ProcessEvent_Copy(VOID);
VOID ProcessEvent_DisableAll(VOID);
VOID ProcessEvent_EnableAll(VOID);
VOID ProcessEvent_Iconify(VOID);
VOID ProcessEvent_Information(VOID);
VOID ProcessEvent_Launch(VOID);
VOID ProcessEvent_Open(VOID);
VOID ProcessEvent_Paste(VOID);
VOID ProcessEvent_Presets(ULONG id);
VOID ProcessEvent_Reload(VOID);
VOID ProcessEvent_Save(VOID);
VOID ProcessEvent_Settings(VOID);

BOOL OpenLibs(VOID);
VOID CloseLibs(VOID);

ObjApp_t * CreateApp(VOID);
VOID DisposeApp(ObjApp_t * object);

ULONG main(ULONG argc, STRPTR argv[]);

/********************************************************************
 *** GLOBALS
 ********************************************************************/

// LONG __stack = 65536;

struct Library * MUIMasterBase;

extern struct Library       * AslBase;
extern struct DosLibrary    * DOSBase;
extern struct Library       * GadToolsBase;
extern struct GfxBase       * GfxBase;
extern struct Library       * IconBase;
extern struct IntuitionBase * IntuitionBase;
extern struct ExecBase      * SysBase;
extern struct Library       * UtilityBase;
extern struct WBStartup     * _WBenchMsg;

STRPTR TEMPLATE = "FILE/A";

ObjApp_t *appMain = NULL;

APTR MN_Main_File,
        MN_Main_Open,
        MN_Main_Save,
        MN_Main_Info,
        MN_Main_About,
        MN_Main_AboutMUI,
        MN_Main_Settings,
        MN_Main_Iconify,
        MN_Main_Quit,
    MN_Main_Edit,
        MN_Main_Reload,
        MN_Main_Copy,
        MN_Main_Paste,
        MN_Main_EnableAll,
        MN_Main_DisableAll,
    MN_Main_Presets;

APTR GROUP_ROOT_1,
    GR_Title,
        LA_Title,
    GR_Tooltypes,
        LA_Filename,    TX_Filename,    BT_Filename,
        LA_GameSpeed,   CY_GameSpeed,   CB_GameSpeed,
        LA_Superscalar, CY_Superscalar, CB_Superscalar,
        LA_DisplayMode, CY_DisplayMode, CB_DisplayMode,
        LA_ZoomMode,    CY_ZoomMode,    CB_ZoomMode,
        LA_ZoomVerticalShift,   SL_ZoomVerticalShift,   CB_ZoomVerticalShift,
        LA_ZoomHorizontalShift,   SL_ZoomHorizontalShift,   CB_ZoomHorizontalShift,
        LA_ForceMode,   CY_ForceMode,   CB_ForceMode,
        LA_Scanlines,   CY_Scanlines,   CB_Scanlines,
    GR_Buttons,
        BT_Open, 
        BT_Save,
        BT_Launch, 
        BT_Quit;

/********************************************************************
 *** 
 *** GetArgValue()
 *** 
 ********************************************************************/

BOOL GetArgValue(STRPTR args, STRPTR arg, ULONG *value)
{
    STRPTR p;
    
    if (p = strstr(args, arg))
    {
        *value = strtoul(p + strlen(arg) + 1, 0, 10);
        return (TRUE);
    }
    
    return (FALSE);
}

/********************************************************************
 *** 
 *** GetEnvAcceptPattern()
 *** 
 ********************************************************************/

BOOL GetEnvAcceptPattern(STRPTR pattern)
{
    BOOL result = FALSE;
    
    if (GetVar("ApolloWHDSet/ACCEPTPATTERN", pattern, MAX_PATHNAME, GVF_GLOBAL_ONLY))
    {
        result = TRUE;
    }
    
    if (!result)
    {
        strcpy(pattern, "(#?.info)");
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** GetEnvRejectPattern()
 *** 
 ********************************************************************/

BOOL GetEnvRejectPattern(STRPTR pattern)
{
    BOOL result = FALSE;
    
    if (GetVar("ApolloWHDSet/REJECTPATTERN", pattern, MAX_PATHNAME, GVF_GLOBAL_ONLY))
    {
        result = TRUE;
    }
    
    if (!result)
    {
        strcpy(pattern, "~(#?.info)");
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** GetEnvDefaultDrawer()
 *** 
 ********************************************************************/

BOOL GetEnvDefaultDrawer(STRPTR drawer)
{
    BOOL result = FALSE;
    
    if (GetVar("ApolloWHDSet/DEFDRAWER", drawer, MAX_PATHNAME, GVF_GLOBAL_ONLY))
    {
        BPTR lock;
        
        if (lock = Lock(drawer, ACCESS_READ))
        {
            UnLock(lock);
            result = TRUE;
        }
    }
    
    if (!result)
    {
        strcpy(drawer, "RAM:");
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** GetEnvWBInfo()
 *** 
 ********************************************************************/

BOOL GetEnvWBInfo(STRPTR command)
{
    BOOL result = FALSE;
    
    if (GetVar("ApolloWHDSet/WBINFO", command, MAX_PATHNAME, GVF_GLOBAL_ONLY) > 0)
    {
        result = TRUE;
    }
    
    if (!result)
    {
        strcpy(command, "C:WBInfo");
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** GetEnvWBLoad()
 *** 
 ********************************************************************/

BOOL GetEnvWBLoad(STRPTR command)
{
    BOOL result = FALSE;
    
    if (GetVar("ApolloWHDSet/WBLOAD", command, MAX_PATHNAME, GVF_GLOBAL_ONLY) > 0)
    {
        result = TRUE;
    }
    
    if (!result)
    {
        strcpy(command, "C:WBLoad");
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** LoadFromArgs()
 *** 
 ********************************************************************/

BOOL LoadFromArgs(STRPTR fullname)
{
    BOOL result = FALSE;
    LONG args[OPT_COUNT];
    struct RDArgs * rdargs;
    args[OPT_FILE] = NULL;
    
    if (rdargs = ReadArgs(TEMPLATE, args, NULL))
    {
        BPTR lock;
        
        STRPTR filename = (STRPTR)args[OPT_FILE];
        
        // Try with filename
        
        if (lock = Lock(filename, ACCESS_READ))
        {
            strncpy(fullname, filename, MAX_PATHNAME);
            UnLock(lock);
            result = TRUE;
        }
        else
        {
            // Try with filename + ".info"
            
            UBYTE tmp[MAX_PATHNAME];
            sprintf(tmp, "%s.info", filename);
            
            if (lock = Lock(tmp, ACCESS_READ))
            {
                strncpy(fullname, tmp, MAX_PATHNAME);
                UnLock(lock);
                result = TRUE;
            }            
        }
        
        FreeArgs(rdargs);
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** LoadFromRequesterFilter()
 *** 
 ********************************************************************/

SAVEDS ASM LONG LoadFromRequesterFilter(
    REG(a0) struct Hook * hook, 
    REG(a1) struct AnchorPath * anchor, 
    REG(a2) struct FileRequester * fr)
{
    BOOL result = FALSE;
    ULONG length;
    STRPTR filename;
    static UBYTE fullname[MAX_PATHNAME];
    
    // Accept drawers
    
    if (anchor->ap_Info.fib_EntryType == 2)
    {
        return (TRUE);
    }
    
    // Check if the filename ends with ".info"
    
    filename = anchor->ap_Info.fib_FileName;
    length = strlen(filename);
    
    
    if ((length > 5) && (stricmp(filename + length - 5, ".info") == 0))
    {
        struct DiskObject *diskObject;
        
        // Get the filename
        
        strcpy(fullname, fr->rf_Dir);
        AddPart(fullname, filename, MAX_PATHNAME);
        fullname[strlen(fullname) - 5] = 0;
        
        // Open the icon (.info)
        
        if (diskObject = GetDiskObject(fullname))
        {
            if (diskObject->do_Type == WBPROJECT)
            {
                // Check the default tool
                
                if (strstr(diskObject->do_DefaultTool, WHDLOAD_NAME))
                {
                    // File is accepted
                    result = TRUE;
                }
            }
            
            FreeDiskObject(diskObject);
        }
    }
    
    return (result);
}

static const struct Hook LoadFromRequesterHook = {
    { 0, 0 }, 
    (VOID *)LoadFromRequesterFilter, 
    NULL, NULL
};

/********************************************************************
 *** 
 *** LoadFromRequester()
 *** 
 ********************************************************************/

BOOL LoadFromRequester(STRPTR filename)
{
    BOOL result = FALSE;
    
    struct Window * window;
    struct FileRequester *request;
    
    static ULONG rx = 25;
    static ULONG ry = 50;
    static ULONG rw = 400;
    static ULONG rh = 400;
    static UBYTE rd[MAX_PATHNAME];
    /*
    static UBYTE ap[MAX_PATHNAME];
    static UBYTE rp[MAX_PATHNAME];
    */
    // Open the ASL file requester
    /*
    GetEnvAcceptPattern(ap);
    GetEnvRejectPattern(rp);
    */
    if (appMain && strlen(appMain->Filename))
    {
        STRPTR part = PathPart(appMain->Filename);
        memset(rd, 0, MAX_PATHNAME);
        strncpy(rd, appMain->Filename, part - appMain->Filename);
    }
    else
    {
        GetEnvDefaultDrawer(rd);
    }
    
    if (appMain && appMain->WI_Main)
    {
        get(appMain->WI_Main, MUIA_Window_Window, &window);
    }
    
    if (request = MUI_AllocAslRequestTags(ASL_FileRequest,
        ASLFR_Window,          window,
        ASLFR_TitleText,       "Select a icon file (.info)",
        ASLFR_InitialLeftEdge, rx,
        ASLFR_InitialTopEdge,  ry,
        ASLFR_InitialWidth,    rw,
        ASLFR_InitialHeight,   rh,
        ASLFR_InitialDrawer,   rd,
        ASLFR_InitialPattern,  "#?.info",
        ASLFR_DoMultiSelect,   FALSE,
        ASLFR_DoPatterns,      TRUE,
        ASLFR_DoSaveMode,      FALSE,
        ASLFR_PopToFront,      TRUE,
        ASLFR_RejectIcons,     FALSE,
        ASLFR_SleepWindow,     TRUE,
        ASLFR_UserData,        appMain,
        ASLFR_Flags1,          FRF_FILTERFUNC | FRF_DOPATTERNS,
        ASLFR_FilterFunc,      &LoadFromRequesterHook,
        TAG_DONE))
    {
        if (MUI_AslRequestTags(request, TAG_DONE))
        {
            // Remember the ASL requester properties
            
            rx = request->fr_LeftEdge;
            ry = request->fr_TopEdge;
            rw = request->fr_Width;
            rh = request->fr_Height;
            
            if (*request->fr_File)
            {
                BPTR lock;
                
                // Store the file path/name
                
                filename[0] = 0;
                AddPart(filename, request->fr_Drawer, MAX_PATHNAME);
                AddPart(filename, request->fr_File,   MAX_PATHNAME);
                
                if (lock = Lock(filename, ACCESS_READ))
                {
                    UnLock(lock);
                    result = TRUE;
                }
            }
        }
        
        MUI_FreeAslRequest(request);
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** LoadFromIcon()
 *** 
 ********************************************************************/

BOOL LoadFromIcon(STRPTR fullname)
{
    BOOL result = FALSE;
    ULONG length = strlen(fullname);
    struct DiskObject *diskObject;
    static UBYTE message[MAX_PATHNAME];
    static UBYTE filename[MAX_PATHNAME];
    
    // Remove the .info extension
    
    if ((length > 5) && (stricmp(fullname + length - 5, ".info") == 0))
    {
        fullname[length - 5] = 0;
    }
    
    // Load from icon
    
    if (diskObject = GetDiskObject(fullname))
    {
        STRPTR toolType;
        
        BOOL bLoad        = TRUE;
        BOOL bGameSpeed   = FALSE;
        BOOL bSuperscalar = FALSE;
        BOOL bDisplayMode = FALSE;
        BOOL bZoomMode    = FALSE;
        BOOL bZoomVerticalShift   = FALSE;
        BOOL bZoomHorizontalShift   = FALSE;
        BOOL bForceMode   = FALSE;
        BOOL bScanlines   = FALSE;
        
        ULONG opt_GameSpeed   = 0;
        ULONG opt_Superscalar = 0;
        ULONG opt_DisplayMode = 0;
        ULONG opt_ZoomMode    = 0;
        ULONG opt_ZoomVerticalShift   = 0;
        ULONG opt_ZoomHorizontalShift   = 0;
        ULONG opt_ForceMode   = 0;
        ULONG opt_Scanlines   = 0;
        
        // Check icon type
        
        if (diskObject->do_Type == WBPROJECT)
        {
            // Check icon default tool
            
            if (strstr(diskObject->do_DefaultTool, WHDLOAD_NAME) == 0)
            {
                // WARNING: DefaultTool isnt WHDLoad
                
                sprintf(message, "\33c"
                "\0338The icon default tool is not \"\33b%s\33n\" !\n\n"
                "\0332Do you want to continue ?", WHDLOAD_NAME);
                
                if (MUI_RequestA(appMain->App, appMain->WI_Main, 0, 
                    "Warning", "_Continue|_Cancel", message, TAG_END) == 0)
                {
                    FreeDiskObject(diskObject);
                    bLoad = FALSE;
                }
            }
        }
        else
        {
            // WARNING: Not a Project icon type
            
            sprintf(message, "\33c"
            "\0338The icon is not of type \"\33b%s\33n\" !\n\n"
            "\0332Do you want to continue ?", "Project");
            
            if (MUI_RequestA(appMain->App, appMain->WI_Main, 0, 
                "Warning", "_Continue|_Cancel", message, TAG_END) == 0)
            {
                FreeDiskObject(diskObject);
                bLoad = FALSE;
            }
        }
        
        if (bLoad)
        {
            // Extract tooltypes values
            
            if (toolType = FindToolType(diskObject->do_ToolTypes, WHDLOAD_TT_NAME))
            {
                bGameSpeed   = GetArgValue(toolType, "TU", &opt_GameSpeed);
                bSuperscalar = GetArgValue(toolType, "SS", &opt_Superscalar);
                bDisplayMode = GetArgValue(toolType, "DM", &opt_DisplayMode);
                bZoomMode    = GetArgValue(toolType, "ZO", &opt_ZoomMode);
                bZoomVerticalShift   = GetArgValue(toolType, "ZV", &opt_ZoomVerticalShift);
                bZoomHorizontalShift   = GetArgValue(toolType, "ZH", &opt_ZoomHorizontalShift);
                bForceMode   = GetArgValue(toolType, "FM", &opt_ForceMode);
                bScanlines   = GetArgValue(toolType, "SC", &opt_Scanlines);
            }
            
            // Set UI values
            
            sprintf(filename, "%s.info", FilePart(fullname));
            set(TX_Filename,    MUIA_Text_Contents, filename);
            set(CB_GameSpeed,   MUIA_Selected, bGameSpeed);
            set(CB_Superscalar, MUIA_Selected, bSuperscalar);
            set(CB_DisplayMode, MUIA_Selected, bDisplayMode);
            set(CB_ZoomMode,    MUIA_Selected, bZoomMode);
            set(CB_ZoomVerticalShift,   MUIA_Selected, bZoomVerticalShift);
            set(CB_ZoomHorizontalShift,   MUIA_Selected, bZoomHorizontalShift);
            set(CB_ForceMode,   MUIA_Selected, bForceMode);
            set(CB_Scanlines,   MUIA_Selected, bScanlines);
            set(CY_GameSpeed,   MUIA_Cycle_Active, opt_GameSpeed);
            set(CY_Superscalar, MUIA_Cycle_Active, opt_Superscalar);
            set(CY_DisplayMode, MUIA_Cycle_Active, opt_DisplayMode);
            set(CY_ZoomMode,    MUIA_Cycle_Active, opt_ZoomMode);
            set(SL_ZoomVerticalShift,   MUIA_Slider_Level, opt_ZoomVerticalShift);
            set(SL_ZoomHorizontalShift,   MUIA_Slider_Level, opt_ZoomHorizontalShift);
            set(CY_ForceMode,   MUIA_Cycle_Active, opt_ForceMode);
            set(CY_Scanlines,   MUIA_Cycle_Active, opt_Scanlines);
            
            result = TRUE;
        }
        
        FreeDiskObject(diskObject);
    }
    else
    {
        MUI_RequestA(appMain->App, appMain->WI_Main, 
            0, "Error", "_OK", "GetDiskObject() failed!", TAG_END);
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** SetFilename()
 *** 
 ********************************************************************/

VOID SetFilename(STRPTR filename)
{
    // Store the opened filename
    
    strncpy(appMain->Filename, filename, MAX_PATHNAME);
}

/********************************************************************
 *** 
 *** ProcessEvent_About()
 *** 
 ********************************************************************/

VOID ProcessEvent_About(VOID)
{
    MUI_RequestA(appMain->App, appMain->WI_Main, 0, 
        "About", "_OK", "\33c"
        "\0338" APP_NAME        "\n\n"
        "\0332" APP_VERSTRING   "\n"
                APP_DESCRIPTION "\n"
                APP_COPYRIGHT   "\n\n"
        "This is a MUI-Application.\n"
        "MUI is copyrighted by Stefan Stuntz.", 
        TAG_END);
}

/********************************************************************
 *** 
 *** ProcessEvent_AboutMUI()
 *** 
 ********************************************************************/

VOID ProcessEvent_AboutMUI(VOID)
{
    Object *win = AboutmuiObject,
        MUIA_Window_RefWindow, appMain->WI_Main,
        MUIA_Aboutmui_Application, appMain->App,
        End;
    
    set(win, MUIA_Window_Open, TRUE);
}

/********************************************************************
 *** 
 *** ProcessEvent_Copy()
 *** 
 ********************************************************************/

VOID ProcessEvent_Copy(VOID)
{
    Profile_t *profile;
    
    if (profile = &appMain->ClipBoard[0])
    {
        get(CB_GameSpeed,   MUIA_Selected,     &profile->pr_bGameSpeed);
        get(CB_Superscalar, MUIA_Selected,     &profile->pr_bSuperscalar);
        get(CB_DisplayMode, MUIA_Selected,     &profile->pr_bDisplayMode);
        get(CB_ZoomMode,    MUIA_Selected,     &profile->pr_bZoomMode);
        get(CB_ZoomVerticalShift,   MUIA_Selected,     &profile->pr_bZoomVerticalShift);
        get(CB_ZoomHorizontalShift,   MUIA_Selected,     &profile->pr_bZoomHorizontalShift);
        get(CB_ForceMode,   MUIA_Selected,     &profile->pr_bForceMode);
        get(CB_Scanlines,   MUIA_Selected,     &profile->pr_bScanlines);
        
        get(CY_GameSpeed,   MUIA_Cycle_Active, &profile->pr_lGameSpeed);
        get(CY_Superscalar, MUIA_Cycle_Active, &profile->pr_lSuperscalar);
        get(CY_DisplayMode, MUIA_Cycle_Active, &profile->pr_lDisplayMode);
        get(CY_ZoomMode,    MUIA_Cycle_Active, &profile->pr_lZoomMode);
        get(SL_ZoomVerticalShift,   MUIA_Slider_Level, &profile->pr_lZoomVerticalShift);
        get(SL_ZoomHorizontalShift,   MUIA_Slider_Level, &profile->pr_lZoomHorizontalShift);
        get(CY_ForceMode,   MUIA_Cycle_Active, &profile->pr_lForceMode);
        get(CY_Scanlines,   MUIA_Cycle_Active, &profile->pr_lScanlines);
    }
}

/********************************************************************
 *** 
 *** ProcessEvent_DisableAll()
 *** 
 ********************************************************************/

VOID ProcessEvent_DisableAll(VOID)
{
    set(CB_GameSpeed,   MUIA_Selected, FALSE);
    set(CB_Superscalar, MUIA_Selected, FALSE);
    set(CB_DisplayMode, MUIA_Selected, FALSE);
    set(CB_ZoomMode,    MUIA_Selected, FALSE);
    set(CB_ZoomVerticalShift,   MUIA_Selected, FALSE);
    set(CB_ZoomHorizontalShift,   MUIA_Selected, FALSE);
    set(CB_ForceMode,   MUIA_Selected, FALSE);
    set(CB_Scanlines,   MUIA_Selected, FALSE);
}

/********************************************************************
 *** 
 *** ProcessEvents_DropFile()
 *** 
 ********************************************************************/

SAVEDS ASM LONG ProcessEvents_DropFile(
    REG(a2) APTR object, 
    REG(a1) struct AppMessage **appMessage)
{
    ULONG i;
    struct WBArg *wbArg;
    struct AppMessage *appMsg = *appMessage;
    
    // Activate the window
    
    set(appMain->WI_Main, MUIA_Window_Activate, TRUE);
    
    // Obtain the first dropped file
    
    for (wbArg = appMsg->am_ArgList, i = 0; i < appMsg->am_NumArgs; i++, wbArg++)
    {
        NameFromLock(wbArg->wa_Lock, appMain->Buffer, MAX_PATHNAME);
        AddPart(appMain->Buffer, wbArg->wa_Name, MAX_PATHNAME);
        
        if (LoadFromIcon(appMain->Buffer))
        {
            SetFilename(appMain->Buffer);
        }
        
        break; // Stop on first file
    }
    
    return (0);
}

static const struct Hook AppMsgHook = {
    { 0, 0 }, 
    (VOID *)ProcessEvents_DropFile, 
    NULL, NULL
};

/********************************************************************
 *** 
 *** ProcessEvent_EnableAll()
 *** 
 ********************************************************************/

VOID ProcessEvent_EnableAll(VOID)
{
    set(CB_GameSpeed,   MUIA_Selected, TRUE);
    set(CB_Superscalar, MUIA_Selected, TRUE);
    set(CB_DisplayMode, MUIA_Selected, TRUE);
    set(CB_ZoomMode,    MUIA_Selected, TRUE);
    set(CB_ZoomVerticalShift,   MUIA_Selected, TRUE);
    set(CB_ZoomHorizontalShift,   MUIA_Selected, TRUE);
    set(CB_ForceMode,   MUIA_Selected, TRUE);
    set(CB_Scanlines,   MUIA_Selected, TRUE);
}

/********************************************************************
 *** 
 *** ProcessEvent_Iconify()
 *** 
 ********************************************************************/

VOID ProcessEvent_Iconify(VOID)
{
    set(appMain->App, 
        MUIA_Application_Iconified, TRUE);
}

/********************************************************************
 *** 
 *** ProcessEvents_Information()
 *** 
 ********************************************************************/

VOID ProcessEvent_Information(VOID)
{
    static UBYTE CommandName[MAX_PATHNAME];
    
    if (strlen(appMain->Filename) > 0)
    {
        // C:WBInfo command tool
        
        GetEnvWBInfo(CommandName);
        
        sprintf(appMain->Buffer, "%s \"%s\"\n", 
            CommandName, appMain->Filename);
        
        if (!Execute(appMain->Buffer, NULL, NULL))
        {
            MUI_RequestA(appMain->App, appMain->WI_Main, 
                0, "Error", "_OK", "Execute() failed!", TAG_END);
        }
    }
}

/********************************************************************
 *** 
 *** ProcessEvents_Launch()
 *** 
 ********************************************************************/

VOID ProcessEvent_Launch(VOID)
{
    static UBYTE CommandName[MAX_PATHNAME];
    
    if (strlen(appMain->Filename) > 0)
    {
        // C:WBLoad command tool
        
        memset(CommandName, 0, MAX_PATHNAME);
        
        GetEnvWBLoad(CommandName);
        
        sprintf(appMain->Buffer, "%s \"%s\"\n", 
            CommandName, appMain->Filename);
        
        if (!Execute(appMain->Buffer, NULL, NULL))
        {
            MUI_RequestA(appMain->App, appMain->WI_Main, 
                0, "Error", "_OK", "Execute() failed!", TAG_END);
        }
    }
}

/********************************************************************
 *** 
 *** ProcessEvents_Open()
 *** 
 ********************************************************************/

VOID ProcessEvent_Open(VOID)
{
    if (LoadFromRequester(appMain->Buffer))
    {
        if (LoadFromIcon(appMain->Buffer))
        {
            SetFilename(appMain->Buffer);
        }
    }
}

/********************************************************************
 *** 
 *** ProcessEvent_Paste()
 *** 
 ********************************************************************/

VOID ProcessEvent_Paste(VOID)
{
    Profile_t *profile;
    
    if (profile = &appMain->ClipBoard[0])
    {
        set(CB_GameSpeed,   MUIA_Selected,     profile->pr_bGameSpeed);
        set(CB_Superscalar, MUIA_Selected,     profile->pr_bSuperscalar);
        set(CB_DisplayMode, MUIA_Selected,     profile->pr_bDisplayMode);
        set(CB_ZoomMode,    MUIA_Selected,     profile->pr_bZoomMode);
        set(CB_ZoomVerticalShift,   MUIA_Selected,     profile->pr_bZoomVerticalShift);
        set(CB_ZoomHorizontalShift,   MUIA_Selected,     profile->pr_bZoomHorizontalShift);
        set(CB_ForceMode,   MUIA_Selected,     profile->pr_bForceMode);
        set(CB_Scanlines,   MUIA_Selected,     profile->pr_bScanlines);
        
        set(CY_GameSpeed,   MUIA_Cycle_Active, profile->pr_lGameSpeed);
        set(CY_Superscalar, MUIA_Cycle_Active, profile->pr_lSuperscalar);
        set(CY_DisplayMode, MUIA_Cycle_Active, profile->pr_lDisplayMode);
        set(CY_ZoomMode,    MUIA_Cycle_Active, profile->pr_lZoomMode);
        set(SL_ZoomVerticalShift,   MUIA_Slider_Level, profile->pr_lZoomVerticalShift);
        set(SL_ZoomHorizontalShift,   MUIA_Slider_Level, profile->pr_lZoomHorizontalShift);
        set(CY_ForceMode,   MUIA_Cycle_Active, profile->pr_lForceMode);
        set(CY_Scanlines,   MUIA_Cycle_Active, profile->pr_lScanlines);
    }
}

/********************************************************************
 *** 
 *** ProcessEvent_Presets()
 *** 
 ********************************************************************/

VOID ProcessEvent_Presets(ULONG id)
{
    if (id < MAX_PRESETS)
    {
        Preset_t p = Presets[id];
        
        set(CB_GameSpeed,   MUIA_Selected, p.pr_GameSpeed   >= 0);
        set(CB_Superscalar, MUIA_Selected, p.pr_Superscalar >= 0);
        set(CB_DisplayMode, MUIA_Selected, p.pr_DisplayMode >= 0);
        set(CB_ZoomMode,    MUIA_Selected, p.pr_ZoomMode    >= 0);
        set(CB_ZoomVerticalShift,   MUIA_Selected, p.pr_ZoomVerticalShift   >= 0);
        set(CB_ZoomHorizontalShift,   MUIA_Selected, p.pr_ZoomHorizontalShift   >= 0);
        set(CB_ForceMode,   MUIA_Selected, p.pr_ForceMode   >= 0);
        set(CB_Scanlines,   MUIA_Selected, p.pr_Scanlines   >= 0);
        
        set(CY_GameSpeed,   MUIA_Cycle_Active, p.pr_GameSpeed   < 0 ? 0 : p.pr_GameSpeed  );
        set(CY_Superscalar, MUIA_Cycle_Active, p.pr_Superscalar < 0 ? 0 : p.pr_Superscalar);
        set(CY_DisplayMode, MUIA_Cycle_Active, p.pr_DisplayMode < 0 ? 0 : p.pr_DisplayMode);
        set(CY_ZoomMode,    MUIA_Cycle_Active, p.pr_ZoomMode    < 0 ? 0 : p.pr_ZoomMode   );
        set(SL_ZoomVerticalShift,   MUIA_Slider_Level, p.pr_ZoomVerticalShift   < 0 ? 0 : p.pr_ZoomVerticalShift  );
        set(SL_ZoomHorizontalShift,   MUIA_Slider_Level, p.pr_ZoomHorizontalShift   < 0 ? 0 : p.pr_ZoomHorizontalShift  );
        set(CY_ForceMode,   MUIA_Cycle_Active, p.pr_ForceMode   < 0 ? 0 : p.pr_ForceMode  );
        set(CY_Scanlines,   MUIA_Cycle_Active, p.pr_Scanlines   < 0 ? 0 : p.pr_Scanlines  );
    }
}

/********************************************************************
 *** 
 *** ProcessEvents_Reload()
 *** 
 ********************************************************************/

VOID ProcessEvent_Reload(VOID)
{
    if (strlen(appMain->Filename) > 0)
    {
        LoadFromIcon(appMain->Filename);
    }
}

/********************************************************************
 *** 
 *** ProcessEvents_Save()
 *** 
 ********************************************************************/

VOID ProcessEvent_Save(VOID)
{
    BOOL bRemove = FALSE;
    
    struct DiskObject *diskObject;
    
    static UBYTE buffer[MAX_PATHNAME];
    static UBYTE message[MAX_PATHNAME];
    static UBYTE filename[MAX_PATHNAME];
    static UBYTE template[MAX_PATHNAME];
    static STRPTR newToolTypes[MAX_TOOLTYPES];
    
    ULONG opt_Count = 0;
    ULONG opt_Filename;
    ULONG bGameSpeed,   opt_GameSpeed;
    ULONG bSuperscalar, opt_Superscalar;
    ULONG bDisplayMode, opt_DisplayMode;
    ULONG bZoomMode,    opt_ZoomMode;
    ULONG bZoomVerticalShift,   opt_ZoomVerticalShift;
    ULONG bZoomHorizontalShift,   opt_ZoomHorizontalShift;
    ULONG bForceMode,   opt_ForceMode;
    ULONG bScanlines,   opt_Scanlines;
    
    // Get the filename
    
    sprintf(filename, "%s.info", FilePart(appMain->Filename));
    
    // Get UI values
    
    get(CB_GameSpeed,   MUIA_Selected, &bGameSpeed);
    get(CB_Superscalar, MUIA_Selected, &bSuperscalar);
    get(CB_DisplayMode, MUIA_Selected, &bDisplayMode);
    get(CB_ZoomMode,    MUIA_Selected, &bZoomMode);
    get(CB_ZoomVerticalShift,   MUIA_Selected, &bZoomVerticalShift);
    get(CB_ZoomHorizontalShift,   MUIA_Selected, &bZoomHorizontalShift);
    get(CB_ForceMode,   MUIA_Selected, &bForceMode);
    get(CB_Scanlines,   MUIA_Selected, &bScanlines);
    
    get(TX_Filename,    MUIA_Text_Contents, &opt_Filename);
    get(CY_GameSpeed,   MUIA_Cycle_Active,  &opt_GameSpeed);
    get(CY_Superscalar, MUIA_Cycle_Active,  &opt_Superscalar);
    get(CY_DisplayMode, MUIA_Cycle_Active,  &opt_DisplayMode);
    get(CY_ZoomMode,    MUIA_Cycle_Active,  &opt_ZoomMode);
    get(SL_ZoomVerticalShift,   MUIA_Slider_Level,  &opt_ZoomVerticalShift);
    get(SL_ZoomHorizontalShift,   MUIA_Slider_Level,  &opt_ZoomHorizontalShift);
    get(CY_ForceMode,   MUIA_Cycle_Active,  &opt_ForceMode);
    get(CY_Scanlines,   MUIA_Cycle_Active,  &opt_Scanlines);
    
    // Build the tooltype
    
    strcpy(template, WHDLOAD_TT_NAME);
    strcat(template, "=");
    strcat(template, WHDLOAD_TT_TOOL);
    strcat(template, " >NIL:");
    
    if (bGameSpeed  ) { opt_Count++; sprintf(buffer, " TU %lu", opt_GameSpeed  ); strcat(template, buffer); };
    if (bSuperscalar) { opt_Count++; sprintf(buffer, " SS %lu", opt_Superscalar); strcat(template, buffer); };
    if (bDisplayMode) { opt_Count++; sprintf(buffer, " DM %lu", opt_DisplayMode); strcat(template, buffer); };
    if (bZoomMode   ) { opt_Count++; sprintf(buffer, " ZO %lu", opt_ZoomMode   ); strcat(template, buffer); };
    if (bZoomVerticalShift  ) { opt_Count++; sprintf(buffer, " ZV %lu", opt_ZoomVerticalShift  ); strcat(template, buffer); };
    if (bZoomHorizontalShift  ) { opt_Count++; sprintf(buffer, " ZH %lu", opt_ZoomHorizontalShift  ); strcat(template, buffer); };
    if (bForceMode  ) { opt_Count++; sprintf(buffer, " FM %lu", opt_ForceMode  ); strcat(template, buffer); };
    if (bScanlines  ) { opt_Count++; sprintf(buffer, " SC %lu", opt_Scanlines  ); strcat(template, buffer); };
    
    // Check if there is at least one option to save
    
    if (opt_Count > 0)
    {
        sprintf(message, "\33c"
            "\0332Save \"\33b%s\33n\" to \"\33b%s\33n\" ?\n\n"
            "\0338%s", 
            WHDLOAD_TT_NAME, filename, template);
        
        bRemove = FALSE;
    }
    else
    {
        sprintf(message,  "\33c"
            "\0338There isnt any option to save.\n\n"
            "\0332Remove \"\33b%s\33n\" from \"\33b%s\33n\" ?", 
            WHDLOAD_TT_NAME, filename);
        
        bRemove = TRUE;
    }
    
    if (MUI_RequestA(appMain->App, appMain->WI_Main, 
        0, "Question", "_Save|_Cancel", message, TAG_END) == 0)
    {
        return; // Cancelled
    }
    
    // Save to icon
    
    if (diskObject = GetDiskObject(appMain->Filename))
    {
        newToolTypes[0] = template;
        
        if (diskObject->do_ToolTypes)
        {
            char **tt;
            ULONG n = (bRemove ? 0 : 1);
            
            for (tt = diskObject->do_ToolTypes; tt && *tt && n < MAX_TOOLTYPES - 1; tt++)
            {
                if (strstr(*tt, WHDLOAD_TT_NAME) == 0)
                {
                    newToolTypes[n] = *tt;
                    n++;
                }
            }
            
            newToolTypes[n] = NULL;
        }
        
        diskObject->do_ToolTypes = newToolTypes;
        
        if (!PutDiskObject(appMain->Filename, diskObject))
        {
            MUI_RequestA(appMain->App, appMain->WI_Main, 
                0, "Error", "_OK", "PutDiskObject() failed!", TAG_END);
        }
        
        FreeDiskObject(diskObject);
    }
    else
    {
        MUI_RequestA(appMain->App, appMain->WI_Main, 
            0, "Error", "_OK", "GetDiskObject() failed!", TAG_END);
    }
}

/********************************************************************
 *** 
 *** ProcessEvent_Settings()
 *** 
 ********************************************************************/

VOID ProcessEvent_Settings(VOID)
{
    DoMethod(appMain->App, 
        MUIM_Application_OpenConfigWindow, 0, 
        MAKE_ID('A','W','H','D'));
}

/********************************************************************
 *** 
 *** ProcessEvents()
 *** 
 ********************************************************************/

VOID ProcessEvents(VOID)
{
    BOOL running = TRUE;
    
    while (running)
    {
        ULONG signals;
        
        ULONG id = DoMethod(appMain->App, 
            MUIM_Application_Input, &signals);
        
        switch (id)
        {
        case MUIV_Application_ReturnID_Quit:
            running = FALSE;
            break;
        case EVENT_ABOUT:
            ProcessEvent_About();
            break;
        case EVENT_ABOUT_MUI:
            ProcessEvent_AboutMUI();
            break;
        case EVENT_COPY:
            ProcessEvent_Copy();
            break;
        case EVENT_DISABLE_ALL:
            ProcessEvent_DisableAll();
            break;
        case EVENT_ENABLE_ALL:
            ProcessEvent_EnableAll();
            break;
        case EVENT_ICONIFY:
            ProcessEvent_Iconify();
            break;
        case EVENT_INFO:
            ProcessEvent_Information();
            break;
        case EVENT_LAUNCH:
            ProcessEvent_Launch();
            break;
        case EVENT_OPEN:
            ProcessEvent_Open();
            break;
        case EVENT_PASTE:
            ProcessEvent_Paste();
            break;
        case EVENT_RELOAD:
            ProcessEvent_Reload();
            break;
        case EVENT_SAVE:
            ProcessEvent_Save();
            break;
        case EVENT_SETTINGS:
            ProcessEvent_Settings();
            break;
        case EVENT_PRESET+0:
            ProcessEvent_Presets(0);
            break;
        case EVENT_PRESET+1:
            ProcessEvent_Presets(1);
            break;
        case EVENT_PRESET+2:
            ProcessEvent_Presets(2);
            break;
        case EVENT_PRESET+3:
            ProcessEvent_Presets(3);
            break;
        case EVENT_PRESET+4:
            ProcessEvent_Presets(4);
            break;
        case EVENT_PRESET+5:
            ProcessEvent_Presets(5);
            break;
        case EVENT_PRESET+6:
            ProcessEvent_Presets(6);
            break;
        case EVENT_PRESET+7:
            ProcessEvent_Presets(7);
            break;
        case EVENT_PRESET+8:
            ProcessEvent_Presets(8);
            break;
        case EVENT_PRESET+9:
            ProcessEvent_Presets(9);
            break;
        }
        
        if (running && signals)
        {
            signals = Wait(signals | 
                SIGBREAKF_CTRL_C | 
                SIGBREAKF_CTRL_E | 
                SIGBREAKF_CTRL_F);
            
            if (signals & SIGBREAKF_CTRL_C)
            {
                break;
            }
            
            if ((signals & SIGBREAKF_CTRL_E) || 
                (signals & SIGBREAKF_CTRL_F))
            {
                set(appMain->App, MUIA_Application_Iconified, FALSE);
                set(appMain->WI_Main, MUIA_Window_Open, TRUE);
            }
        }
    }
}

/********************************************************************
 *** 
 *** CreateApp()
 *** 
 ********************************************************************/

ObjApp_t * CreateApp(VOID)
{
    ULONG i;
    ObjApp_t * object;
    
    if (!(object = AllocVec(sizeof(ObjApp_t), MEMF_PUBLIC | MEMF_CLEAR)))
    {
        return (NULL);
    }
    
    object->DisplayMode[0] = "Amiga NTSC";
    object->DisplayMode[1] = "Amiga PAL";
    object->DisplayMode[2] = NULL;
    
    object->GameSpeed[0]   = "Amiga full speed";
    object->GameSpeed[1]   = "Amiga 500 speed";
    object->GameSpeed[2]   = "Amiga 1200 speed";
    object->GameSpeed[3]   = NULL;
    
    object->Scanlines[0]   = "Disabled";
    object->Scanlines[1]   = "Enabled";
    object->Scanlines[2]   = NULL;
    
    object->ForceMode[0]   = "AGA";
    object->ForceMode[1]   = "OCS";
    object->ForceMode[2]   = NULL;

    object->Superscalar[0] = "Disabled";
    object->Superscalar[1] = "Enabled";
    object->Superscalar[2] = NULL;
    
    object->ZoomMode[0]    = "No zoom (full overscan)";
    object->ZoomMode[1]    = "Zoom 320x200";
    object->ZoomMode[2]    = "Zoom 320x240";
    object->ZoomMode[3]    = "Zoom 320x256";
    object->ZoomMode[4]    = NULL;
    
    LA_Title = Label(
        "\33c"
        "\33b" APP_NAME "\33n\n"
        "         " 
        APP_DESCRIPTION 
        "         ");
    
    GR_Title = GroupObject,
        MUIA_Background,         MUII_SHINE,
        MUIA_Frame,              MUIV_Frame_Group,
        MUIA_Group_Horiz,        TRUE,
        MUIA_Group_HorizSpacing, 4,
        MUIA_Group_VertSpacing,  4,
        Child,                   LA_Title,
    End;
    
    LA_Filename    = Label("Filename :");
    TX_Filename    = MakeText("");
    BT_Filename    = SimpleButton("?");
    LA_GameSpeed   = Label("Game Speed :");
    CY_GameSpeed   = MakeCycle(object->GameSpeed);
    CB_GameSpeed   = CheckMark(TRUE);
    LA_Superscalar = Label("Superscalar :");
    CY_Superscalar = MakeCycle(object->Superscalar);
    CB_Superscalar = CheckMark(TRUE);
    LA_DisplayMode = Label("Display Mode :");
    CY_DisplayMode = MakeCycle(object->DisplayMode);
    CB_DisplayMode = CheckMark(TRUE);
    LA_ZoomMode    = Label("Zoom Mode :");
    CY_ZoomMode    = MakeCycle(object->ZoomMode);
    CB_ZoomMode    = CheckMark(TRUE);
    LA_ZoomVerticalShift   = Label("Vertical Shift :");
    SL_ZoomVerticalShift   = MakeSlider(0, 0, 126);
    CB_ZoomVerticalShift   = CheckMark(TRUE);
    LA_ZoomHorizontalShift   = Label("Horizontal Shift :");
    SL_ZoomHorizontalShift   = MakeSlider(0, 0, 62);
    CB_ZoomHorizontalShift   = CheckMark(TRUE);
    LA_ForceMode   = Label("Force mode :");
    CY_ForceMode   = MakeCycle(object->ForceMode);
    CB_ForceMode   = CheckMark(TRUE);
    LA_Scanlines   = Label("Scanlines :");
    CY_Scanlines   = MakeCycle(object->Scanlines);
    CB_Scanlines   = CheckMark(TRUE);
    
    GR_Tooltypes = GroupObject,
        MUIA_Frame,              MUIV_Frame_Group, 
        MUIA_Group_Columns,      3, 
        MUIA_Group_HorizSpacing, 4, 
        MUIA_Group_VertSpacing,  4, 
        Child, LA_Filename,    Child, TX_Filename,    Child, BT_Filename, 
        Child, LA_GameSpeed,   Child, CY_GameSpeed,   Child, CB_GameSpeed, 
        Child, LA_Superscalar, Child, CY_Superscalar, Child, CB_Superscalar, 
        Child, LA_DisplayMode, Child, CY_DisplayMode, Child, CB_DisplayMode, 
        Child, LA_ZoomMode,    Child, CY_ZoomMode,    Child, CB_ZoomMode, 
        Child, LA_ZoomVerticalShift,   Child, SL_ZoomVerticalShift,   Child, CB_ZoomVerticalShift, 
        Child, LA_ZoomHorizontalShift,   Child, SL_ZoomHorizontalShift,   Child, CB_ZoomHorizontalShift, 
        Child, LA_ForceMode,   Child, CY_ForceMode,   Child, CB_ForceMode, 
        Child, LA_Scanlines,   Child, CY_Scanlines,   Child, CB_Scanlines, 
    End;
    
    BT_Open   = SimpleButton("_Open");
    BT_Save   = SimpleButton("_Save");
    BT_Launch = SimpleButton("_Launch");
    BT_Quit   = SimpleButton("_Quit");
    
    set(BT_Open,   MUIA_HorizWeight, 22);
    set(BT_Save,   MUIA_HorizWeight, 22);
    set(BT_Launch, MUIA_HorizWeight, 34);
    set(BT_Quit,   MUIA_HorizWeight, 22);
    
    GR_Buttons = GroupObject,
        MUIA_Frame, MUIV_Frame_Group,
        MUIA_Group_Horiz,        TRUE,
        MUIA_Group_HorizSpacing, 4,
        MUIA_Group_VertSpacing,  4,
        Child, BT_Open,
        Child, BT_Save,
        Child, BT_Launch,
        Child, BT_Quit,
    End;
    
    GROUP_ROOT_1 = GroupObject,
        MUIA_Group_HorizSpacing, 4,
        MUIA_Group_VertSpacing,  4,
        Child, GR_Title,
        Child, GR_Tooltypes,
        Child, GR_Buttons,
    End;
    
    object->WI_Main = WindowObject,
        MUIA_Window_Title,      APP_NAME,
        MUIA_Window_ID,         MAKE_ID('A', 'W', 'H', 'D'),
        MUIA_Window_AppWindow,  TRUE,
        MUIA_Window_SizeGadget, TRUE,
        MUIA_Window_SizeRight,  TRUE,
        WindowContents,         GROUP_ROOT_1,
    End;
    
    // Menus
    
    MN_Main_Open       = MakeMenuItem("Open...",          "O");
    MN_Main_Save       = MakeMenuItem("Save",             "S");
    MN_Main_Info       = MakeMenuItem("Information...",   "I");
    MN_Main_About      = MakeMenuItem("About...",         "?");
    MN_Main_AboutMUI   = MakeMenuItem("About MUI...",     "A");
    MN_Main_Settings   = MakeMenuItem("MUI Settings...",  "M");
    MN_Main_Iconify    = MakeMenuItem("Iconify",          "Y");
    MN_Main_Quit       = MakeMenuItem("Quit",             "Q");
    
    MN_Main_Reload     = MakeMenuItem("Reload from icon", "R");
    MN_Main_Copy       = MakeMenuItem("Copy",             "C");
    MN_Main_Paste      = MakeMenuItem("Paste",            "V");
    MN_Main_EnableAll  = MakeMenuItem("Enable all",       "E");
    MN_Main_DisableAll = MakeMenuItem("Disable all",      "D");
    
    for (i = 0; i < MAX_PRESETS; i++)
    {
        Presets[i].pr_MN_Item = MakeMenuItem(
            Presets[i].pr_MN_Label, 
            Presets[i].pr_MN_Shortcut);
    }
    
    MN_Main_File = MenuitemObject,
        MUIA_Menuitem_Title, "File",
        MUIA_Family_Child, MN_Main_Open,
        MUIA_Family_Child, MN_Main_Save,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_Info,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_About,
        MUIA_Family_Child, MN_Main_AboutMUI,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_Settings,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_Iconify,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_Quit,
    End;
    
    MN_Main_Edit = MenuitemObject,
        MUIA_Menuitem_Title, "Edit",
        MUIA_Family_Child, MN_Main_Reload,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_Copy,
        MUIA_Family_Child, MN_Main_Paste,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, MN_Main_EnableAll,
        MUIA_Family_Child, MN_Main_DisableAll,
    End;
    
    MN_Main_Presets = MenuitemObject,
        MUIA_Menuitem_Title, "Presets",
        MUIA_Family_Child, Presets[0].pr_MN_Item,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, Presets[7].pr_MN_Item,
        MUIA_Family_Child, Presets[4].pr_MN_Item,
        MUIA_Family_Child, Presets[1].pr_MN_Item,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, Presets[8].pr_MN_Item,
        MUIA_Family_Child, Presets[5].pr_MN_Item,
        MUIA_Family_Child, Presets[2].pr_MN_Item,
        MUIA_Family_Child, MakeMenuBar(),
        MUIA_Family_Child, Presets[9].pr_MN_Item,
        MUIA_Family_Child, Presets[6].pr_MN_Item,
        MUIA_Family_Child, Presets[3].pr_MN_Item,
    End;
    
    object->MN_Main = MenustripObject,
        MUIA_Family_Child, MN_Main_File,
        MUIA_Family_Child, MN_Main_Edit,
        MUIA_Family_Child, MN_Main_Presets,
    End;
    
    // App
    
    object->App = ApplicationObject,
        MUIA_Application_Author,      APP_AUTHORS,
        MUIA_Application_Base,        APP_BASE,
        MUIA_Application_Copyright,   APP_COPYRIGHT,
        MUIA_Application_Description, APP_DESCRIPTION,
        MUIA_Application_HelpFile,    APP_HELPFILE,
        MUIA_Application_Title,       APP_NAME,
        MUIA_Application_Version,     "$VER: " APP_VERSTRING,
        MUIA_Application_Menustrip,   object->MN_Main,
        SubWindow, object->WI_Main,
    End;
    
    if (!object->App)
    {
        FreeVec(object);
        return NULL;
    }
    
    // Menus -> Project
    
    MenuItemTrigger(MN_Main_Open,       EVENT_OPEN);
    MenuItemTrigger(MN_Main_Save,       EVENT_SAVE);
    MenuItemTrigger(MN_Main_EnableAll,  EVENT_ENABLE_ALL);
    MenuItemTrigger(MN_Main_DisableAll, EVENT_DISABLE_ALL);
    MenuItemTrigger(MN_Main_Info,       EVENT_INFO);
    MenuItemTrigger(MN_Main_About,      EVENT_ABOUT);
    MenuItemTrigger(MN_Main_AboutMUI,   EVENT_ABOUT_MUI);
    MenuItemTrigger(MN_Main_Settings,   EVENT_SETTINGS);
    MenuItemTrigger(MN_Main_Iconify,    EVENT_ICONIFY);
    MenuItemTrigger(MN_Main_Quit,       MUIV_Application_ReturnID_Quit);
    
    // Menus -> Edit
    
    MenuItemTrigger(MN_Main_Reload,     EVENT_RELOAD);
    MenuItemTrigger(MN_Main_Copy,       EVENT_COPY);
    MenuItemTrigger(MN_Main_Paste,      EVENT_PASTE);
    MenuItemTrigger(MN_Main_EnableAll,  EVENT_ENABLE_ALL);
    MenuItemTrigger(MN_Main_DisableAll, EVENT_DISABLE_ALL);
    
    // Menus -> Presets
    
    for (i = 0; i < MAX_PRESETS; i++)
    {
        MenuItemTrigger(Presets[i].pr_MN_Item, EVENT_PRESET + i);
    }
    
    // Buttons
    
    ButtonPressed(BT_Launch,   EVENT_LAUNCH);
    ButtonPressed(BT_Open,     EVENT_OPEN);
    ButtonPressed(BT_Save,     EVENT_SAVE);
    ButtonPressed(BT_Quit,     MUIV_Application_ReturnID_Quit);
    ButtonPressed(BT_Filename, EVENT_INFO);
    
    // Checkmarks
    
    CheckMarkChange(CB_GameSpeed,   CY_GameSpeed);
    CheckMarkChange(CB_Superscalar, CY_Superscalar);
    CheckMarkChange(CB_DisplayMode, CY_DisplayMode);
    CheckMarkChange(CB_ZoomMode,    CY_ZoomMode);
    CheckMarkChange(CB_ZoomVerticalShift,   SL_ZoomVerticalShift);
    CheckMarkChange(CB_ZoomHorizontalShift,   SL_ZoomHorizontalShift);
    CheckMarkChange(CB_ForceMode,   CY_ForceMode);
    CheckMarkChange(CB_Scanlines,   CY_Scanlines);
    
    // Window
    
    DoMethod(object->WI_Main, 
        MUIM_Notify, MUIA_Window_CloseRequest, TRUE, object->App, 2, 
        MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit);
    
    DoMethod(object->WI_Main, 
        MUIM_Notify, MUIA_AppMessage, MUIV_EveryTime,
        object->WI_Main, 3, MUIM_CallHook, &AppMsgHook, MUIV_TriggerValue);
    
    set(object->App, MUIA_Application_DropObject, object->WI_Main);
    set(object->WI_Main, MUIA_Window_Open, TRUE);
    
    return (object);
}

/********************************************************************
 *** 
 *** DisposeApp()
 *** 
 ********************************************************************/

VOID DisposeApp(ObjApp_t * object)
{
    if (object)
    {
        MUI_DisposeObject(object->App);
        FreeVec(object);
    }
}

/********************************************************************
 *** 
 *** OpenLibs()
 *** 
 ********************************************************************/

BOOL OpenLibs(VOID)
{
    MUIMasterBase = OpenLibrary("muimaster.library", 11);
    
    if (MUIMasterBase)
    {
        return (TRUE);
    }
    
    CloseLibs();
    
    return (FALSE);
}

/********************************************************************
 *** 
 *** CloseLibs()
 *** 
 ********************************************************************/

VOID CloseLibs(VOID)
{
    if (MUIMasterBase)
    {
        CloseLibrary((struct Library *)MUIMasterBase);
    }
}

/********************************************************************
 *** 
 *** ENTRY POINT
 *** 
 ********************************************************************/

ULONG main(ULONG argc, STRPTR argv[])
{
    ULONG result = RETURN_FAIL;
    
    if (OpenLibs())
    {
        static UBYTE filename[MAX_PATHNAME];
        
        memset(filename, 0, MAX_PATHNAME);
        
        result = RETURN_WARN;
        
        if (LoadFromArgs(filename) || LoadFromRequester(filename))
        {
            if (appMain = CreateApp())
            {
                if (LoadFromIcon(filename))
                {
                    SetFilename(filename);
                    ProcessEvents();
                    result = RETURN_OK;
                }
                
                DisposeApp(appMain);
            }
        }
        
        CloseLibs();
    }
    
    return (result);
}

/********************************************************************
 *** 
 *** END OF FILE
 *** 
 ********************************************************************/
