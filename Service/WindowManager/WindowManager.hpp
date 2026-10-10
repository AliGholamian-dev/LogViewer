#ifndef SERVICE_WINDOW_MANAGER_WINDOW_MANAGER_HPP
#define SERVICE_WINDOW_MANAGER_WINDOW_MANAGER_HPP

/// TODO: Maybe instead of global/static window state in platform port, introduce a WM struct

enum class WM_WindowFlags : UInt8
{
    UseDefaultPosition = (1 << 0),
    UseDefaultSize     = (1 << 1),
};

struct WM_Window
{
    void* impl;
};

enum class WM_EventKind : UInt8 
{
    Null,
    Count
};

enum class WM_Modifiers
{
    Ctrl  = (1 << 0),
    Shift = (1 << 1),
    Alt   = (1 << 2),
};

struct WM_Event
{
    WM_Event *next;
    WM_Event *prev;
    TimePoint<TimestampClock> timestamp;
    WM_Window window;
    WM_EventKind kind;
    FlagType<WM_Modifiers> modifiers;
};

struct WM_EventList
{
    SizeType count;
    WM_Event *first;
    WM_Event *last;
};

internal void WindowManager_Init(void);
internal void WindowManager_DeInit(void);

internal WM_Window WindowManager_OpenWindow(const Position2D<SInt32> position, const Size2D<UInt16> size, const FlagType<WM_WindowFlags> flags, const String8 title);
internal void WindowManager_CloseWindow(WM_Window window);
internal Bool8 WindowManager_GetIsFocused(WM_Window window);
internal Bool8 WindowManager_GetIsFullscreen(WM_Window window);
internal Bool8 WindowManager_GetIsMaximized(WM_Window window);
internal Bool8 WindowManager_GetIsMinimized(WM_Window window);
internal FlagType<WM_Modifiers> WindowManager_GetModifiers(WM_Window window);
internal void WindowManager_SetTitle(WM_Window window, const String8 title);
internal void WindowManager_Focus(WM_Window window);
internal void WindowManager_SetFullscreen(WM_Window window, const Bool8 fullscreen);
internal void WindowManager_SetMaximized(WM_Window window, const Bool8 maximized);
internal void WindowManager_SetMinimized(WM_Window window, const Bool8 minimized);
internal void WindowManager_BringToFront(WM_Window window);
internal void WindowManager_DoFirstPaint(WM_Window window);

internal WM_Event *WindowManager_PushNewEventToEventList(Arena *arena, WM_EventList *eventList, WM_EventKind eventKind);
internal void WindowManager_SendWakeupEvent(void);
internal WM_EventList WindowManager_GetEvents(Arena *arena, const Bool8 wait);

#endif // SERVICE_WINDOW_MANAGER_WINDOW_MANAGER_HPP
