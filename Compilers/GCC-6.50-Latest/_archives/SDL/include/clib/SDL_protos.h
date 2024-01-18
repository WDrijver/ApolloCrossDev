#ifndef SDL_PROTOS

#include "SDL_types.h"

#define SDL_PROTOS

#if 0
typedef void * SDL_RWops;
typedef void * FILE;
typedef void * SDL_Surface;
typedef void * SDL_Rect;
typedef void * SDL_Event;
typedef void * SDL_PixelFormat;
typedef void * SDL_VideoInfo;
typedef long SDL_eventaction;
#endif

/* General */
int SDL_Init(Uint32 flags);
void SDL_Quit(void);
int SDL_InitSubSystem(Uint32 flags);
void SDL_QuitSubSystem(Uint32 flags);
Uint32 SDL_WasInit(Uint32 flags);


/* RWOps */
SDL_RWops * SDL_RWFromFile(const char *file, const char *mode);
SDL_RWops * SDL_RWFromFP(FILE *fp, int autoclose);
SDL_RWops * SDL_RWFromMem(void *mem, int size);
SDL_RWops * SDL_AllocRW(void);
void SDL_FreeRW(SDL_RWops *area);


/* Gfx */
SDL_Surface * SDL_LoadBMP_RW(SDL_RWops *src, int freesrc);
SDL_Surface * SDL_DisplayFormat(SDL_Surface *surface);
void SDL_FreeSurface(SDL_Surface *surface);
int SDL_FillRect
		(SDL_Surface *dst, SDL_Rect *dstrect, Uint32 color);
int SDL_UpperBlit
			(SDL_Surface *src, SDL_Rect *srcrect,
			 SDL_Surface *dst, SDL_Rect *dstrect);
int SDL_Flip(SDL_Surface *screen);
void SDL_UpdateRects
		(SDL_Surface *screen, int numrects, SDL_Rect *rects);
void SDL_UpdateRect
		(SDL_Surface *screen, Sint32 x, Sint32 y, Uint32 w, Uint32 h);
const SDL_VideoInfo * SDL_GetVideoInfo(void);
SDL_Surface *SDL_SetVideoMode
			(int width, int height, int bpp, Uint32 flags);
Uint32 SDL_MapRGB
			(SDL_PixelFormat *format, Uint8 r, Uint8 g, Uint8 b);
Uint32 SDL_MapRGBA(SDL_PixelFormat *format,
				   Uint8 r, Uint8 g, Uint8 b, Uint8 a);

char *SDL_VideoDriverName(char *namebuf, int maxlen);
SDL_Surface * SDL_GetVideoSurface(void);
int SDL_VideoModeOK(int width, int height, int bpp, Uint32 flags);
SDL_Rect ** SDL_ListModes(SDL_PixelFormat *format, Uint32 flags);
int SDL_SetGamma(float red, float green, float blue);
int SDL_SetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue);
int SDL_GetGammaRamp(Uint16 *red, Uint16 *green, Uint16 *blue);
int SDL_SetColors(SDL_Surface *surface, 
			SDL_Color *colors, int firstcolor, int ncolors);
int SDL_SetPalette(SDL_Surface *surface, int flags,
				   SDL_Color *colors, int firstcolor,
				   int ncolors);
void SDL_GetRGB(Uint32 pixel, SDL_PixelFormat *fmt,
				Uint8 *r, Uint8 *g, Uint8 *b);
void SDL_GetRGBA(Uint32 pixel, SDL_PixelFormat *fmt,
				 Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a);
SDL_Surface *SDL_CreateRGBSurface
			(Uint32 flags, int width, int height, int depth, 
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
SDL_Surface *SDL_CreateRGBSurfaceFrom(void *pixels,
			int width, int height, int depth, int pitch,
			Uint32 Rmask, Uint32 Gmask, Uint32 Bmask, Uint32 Amask);
int SDL_LockSurface(SDL_Surface *surface);
void SDL_UnlockSurface(SDL_Surface *surface);
int SDL_SaveBMP_RW(SDL_Surface *surface, SDL_RWops *dst, int freedst);
int SDL_SetColorKey(SDL_Surface *surface, Uint32 flag, Uint32 key);
int SDL_SetAlpha(SDL_Surface *surface, Uint32 flag, Uint8 alpha);
SDL_bool SDL_SetClipRect(SDL_Surface *surface, const SDL_Rect *rect);
void SDL_GetClipRect(SDL_Surface *surface, SDL_Rect *rect);
SDL_Surface *SDL_ConvertSurface
			(SDL_Surface *src, SDL_PixelFormat *fmt, Uint32 flags);
SDL_Surface * SDL_DisplayFormatAlpha(SDL_Surface *surface);

/* Overlay */
SDL_Overlay *SDL_CreateYUVOverlay(int width, int height,
				Uint32 format, SDL_Surface *display);
int SDL_LockYUVOverlay(SDL_Overlay *overlay);
void SDL_UnlockYUVOverlay(SDL_Overlay *overlay);
int SDL_DisplayYUVOverlay(SDL_Overlay *overlay, SDL_Rect *dstrect);
void SDL_FreeYUVOverlay(SDL_Overlay *overlay);

/* GL */
int SDL_GL_LoadLibrary(const char *path);
void *SDL_GL_GetProcAddress(const char* proc);
int SDL_GL_SetAttribute(SDL_GLattr attr, int value);
int SDL_GL_GetAttribute(SDL_GLattr attr, int* value);
void SDL_GL_SwapBuffers(void);
void SDL_GL_UpdateRects(int numrects, SDL_Rect* rects);
void SDL_GL_Lock(void);
void SDL_GL_Unlock(void);

/* WM */
void SDL_WM_SetCaption(const char *title, const char *icon);
void SDL_WM_GetCaption(char **title, char **icon);
void SDL_WM_SetIcon(SDL_Surface *icon, Uint8 *mask);
int SDL_WM_IconifyWindow(void);
int SDL_WM_ToggleFullScreen(SDL_Surface *surface);
SDL_GrabMode SDL_WM_GrabInput(SDL_GrabMode mode);

/* timer */

Uint32 SDL_GetTicks(void);
void SDL_Delay(Uint32 ms);
int SDL_SetTimer(Uint32 interval, SDL_TimerCallback callback);
SDL_TimerID SDL_AddTimer(Uint32 interval, SDL_NewTimerCallback callback, void *param);
SDL_bool SDL_RemoveTimer(SDL_TimerID t);

/* events */
void SDL_PumpEvents(void);
int SDL_PollEvent(SDL_Event *event);
int SDL_WaitEvent(SDL_Event *event);
int SDL_PeepEvents(SDL_Event *events, int numevents,
				SDL_eventaction action, Uint32 mask);
int SDL_PushEvent(SDL_Event *event);
void SDL_SetEventFilter(SDL_EventFilter filter);
SDL_EventFilter SDL_GetEventFilter(void);
Uint8 SDL_EventState(Uint8 type, int state);

/* joystick */
int SDL_NumJoysticks(void);
const char *SDL_JoystickName(int device_index);
SDL_Joystick *SDL_JoystickOpen(int device_index);
int SDL_JoystickOpened(int device_index);
int SDL_JoystickIndex(SDL_Joystick *joystick);
int SDL_JoystickNumAxes(SDL_Joystick *joystick);
int SDL_JoystickNumBalls(SDL_Joystick *joystick);
int SDL_JoystickNumHats(SDL_Joystick *joystick);
int SDL_JoystickNumButtons(SDL_Joystick *joystick);
void SDL_JoystickUpdate(void);
int SDL_JoystickEventState(int state);
Sint16 SDL_JoystickGetAxis(SDL_Joystick *joystick, int axis);
Uint8 SDL_JoystickGetHat(SDL_Joystick *joystick, int hat);
int SDL_JoystickGetBall(SDL_Joystick *joystick, int ball, int *dx, int *dy);
Uint8 SDL_JoystickGetButton(SDL_Joystick *joystick, int button);
void SDL_JoystickClose(SDL_Joystick *joystick);

/* keyboard */
int SDL_EnableUNICODE(int enable);
int SDL_EnableKeyRepeat(int delay, int interval);
Uint8 * SDL_GetKeyState(int *numkeys);
SDLMod SDL_GetModState(void);
void SDL_SetModState(SDLMod modstate);
char * SDL_GetKeyName(SDLKey key);

/* mouse */
Uint8 SDL_GetMouseState(int *x, int *y);
Uint8 SDL_GetRelativeMouseState(int *x, int *y);
void SDL_WarpMouse(Uint16 x, Uint16 y);
SDL_Cursor *SDL_CreateCursor(Uint8 *data, Uint8 *mask, int w, int h, int hot_x, int hot_y);
void SDL_SetCursor(SDL_Cursor *cursor);
SDL_Cursor * SDL_GetCursor(void);
void SDL_FreeCursor(SDL_Cursor *cursor);
int SDL_ShowCursor(int toggle);

/* application */
Uint8 SDL_GetAppState(void);

/* error */
void SDL_SetError(const char *fmt,args...);
void SDL_SetErrorA(const char *fmt,unsigned long *arglist);
char *SDL_GetError(void);
void SDL_ClearError(void);

/* audio */
int SDL_AudioInit(const char *driver_name);
void SDL_AudioQuit(void);
char *SDL_AudioDriverName(char *namebuf, int maxlen);
int SDL_OpenAudio(SDL_AudioSpec *desired, SDL_AudioSpec *obtained);
SDL_audiostatus SDL_GetAudioStatus(void);
void SDL_PauseAudio(int pause_on);
SDL_AudioSpec *SDL_LoadWAV_RW(SDL_RWops *src, int freesrc,
		 SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len);
void SDL_FreeWAV(Uint8 *audio_buf);
int SDL_BuildAudioCVT(SDL_AudioCVT *cvt,
		Uint16 src_format, Uint8 src_channels, int src_rate,
		Uint16 dst_format, Uint8 dst_channels, int dst_rate);
int SDL_ConvertAudio(SDL_AudioCVT *cvt);
void SDL_MixAudio(Uint8 *dst, const Uint8 *src, Uint32 len, int volume);
void SDL_LockAudio(void);
void SDL_UnlockAudio(void);
void SDL_CloseAudio(void);

/* thread */
SDL_Thread * SDL_CreateThread(int (*fn)(void *), void *data);
Uint32 SDL_ThreadID(void);
Uint32 SDL_GetThreadID(SDL_Thread *thread);
void SDL_WaitThread(SDL_Thread *thread, int *status);
void SDL_KillThread(SDL_Thread *thread);

/* version */
const SDL_version * SDL_Linked_Version(void);

/* extensions */
int SDL_SoftStretch(SDL_Surface *, SDL_Rect *, SDL_Surface *, SDL_Rect *);
#endif
