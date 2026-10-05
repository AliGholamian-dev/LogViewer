struct Win32WindowMangerWindow
{
    Win32WindowMangerWindow *nextFree;
    HWND hwnd;
    HDC hdc;
    WINDOWPLACEMENT lastWindowPlacement;
    Float32 dpi;
    Bool32 firstPaintDone;
    Bool32 maximized;
};

struct Win32WindowManagerState
{
    Arena *arena;
    HINSTANCE hInstance;
    LPCTSTR windowClassName { L"graphical-window" };
    Win32WindowMangerWindow *freeWindow;
};

global Win32WindowManagerState g_win32WindowManagerState {};
global WM_EventList g_win32EventList { };
global Arena *g_win32EventArena { nullptr };

internal WM_Window Win32_WindowManger_GetWindowFromWin32Window(Win32WindowMangerWindow *win32Window)
{
    return WM_Window
    {
        .impl = win32Window
    };
}

internal Win32WindowMangerWindow *Win32_WindowManger_GetWin32WindowFromHWND(HWND)
{
  Win32WindowMangerWindow *win32Window { nullptr };
  /// TODO:
//   for(Win32WindowMangerWindow *window = g_win32WindowManagerState.first_window; window != nullptr; window = window->next)
//   {
//     if(w->hwnd == hwnd)
//     {
//       result = w;
//       break;
//     }
//   }
  return win32Window;
}

internal Win32WindowMangerWindow *Win32_WindowManger_AllocateWindow(void)
{
    Win32WindowMangerWindow *win32Window { g_win32WindowManagerState.freeWindow };
    if (win32Window != nullptr)
    {
        g_win32WindowManagerState.freeWindow = g_win32WindowManagerState.freeWindow->nextFree;
    }
    else
    {
        win32Window = Arena_PushType<Win32WindowMangerWindow>(g_win32WindowManagerState.arena);
    }
    Memory_Zero(win32Window, sizeof(*win32Window));
    win32Window->lastWindowPlacement.length = sizeof(WINDOWPLACEMENT);
    return win32Window;
}

internal void Win32_WindowManger_ReleaseWindow(Win32WindowMangerWindow *win32Window)
{
    ReleaseDC(win32Window->hwnd, win32Window->hdc);
    DestroyWindow(win32Window->hwnd);
    win32Window->nextFree = g_win32WindowManagerState.freeWindow;
    g_win32WindowManagerState.freeWindow = win32Window;
}

internal WM_Event *Win32_WindowManger_PushNewEventToEventList(Arena *arena, WM_EventList *eventList, WM_EventKind kind)
{   
    WM_Event *event = Arena_PushTypeAndZero<WM_Event>(arena);
    event->next = nullptr;
    event->prev = eventList->last;
    
    if (eventList->last != nullptr)
    {
        eventList->last->next = event;
    }
    else
    {
        eventList->first = event;
    }
    eventList->last = event;
    eventList->count = eventList->count  + 1;
    event->timestamp = TimestampClock::Now();
    event->kind = kind;
    return event;
}

internal WM_Event *Win32_WindowManger_PushEvent(WM_EventKind kind, Win32WindowMangerWindow *win32Window)
{
    WM_Event *event { Win32_WindowManger_PushNewEventToEventList(g_win32EventArena, &g_win32EventList, kind) };
    event->window = Win32_WindowManger_GetWindowFromWin32Window(win32Window);
    event->modifiers = WindowManager_GetModifiers(event->window);
    return event;
}

internal LRESULT CALLBACK Win32_WindowManager_WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT result { 0 };
    if (g_win32EventArena == nullptr)
    {
        result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
    }
    else
    {
        Win32WindowMangerWindow *win32Window { Win32_WindowManger_GetWin32WindowFromHWND(hwnd) };
        WM_Window window { Win32_WindowManger_GetWindowFromWin32Window(win32Window) };
        // switch (uMsg)
        // {
        //     default:
        //     {
        //         result = DefWindowProcW(hwnd, uMsg, wParam, lParam);
        //     } break;
        // }
    }

    return result;
}

internal void WindowManager_Init(void)
{
    {
        const ArenaParams windowManagerArenaParams
        {
            .reserveSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : MB(1),
            .commitSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : KB(64),
            .optionalBackingBuffer = nullptr,
            .configFlags = SystemInfo_Get()->largePagesAllowed ? Flag_ConvertEnumToValue<ArenaConfigs>(ArenaConfigs::LargePages) : Flag_NoFlags<ArenaConfigs>()
        };
        g_win32WindowManagerState.arena = Arena_Allocate(&windowManagerArenaParams);
    }
    g_win32WindowManagerState.hInstance = GetModuleHandle(nullptr);
    
    if(!SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2))
    {
        /// TODO: Handle or log
    }
    
    {
        WNDCLASSEX windowClass 
        { 
            .cbSize = sizeof(windowClass),
            .style = CS_VREDRAW|CS_HREDRAW,
            .lpfnWndProc = Win32_WindowManager_WindowProcedure,
            .cbClsExtra = 0, /// TODO:
            .cbWndExtra = 0, /// TODO:
            .hInstance = g_win32WindowManagerState.hInstance,
            .hIcon = nullptr, /// TODO: From resources when added and bundled in exe
            .hCursor = LoadCursor(nullptr, IDC_ARROW), /// TODO: From resources when added and bundled in exe
            .hbrBackground = nullptr,  /// TODO:
            .lpszMenuName = nullptr,  /// TODO:
            .lpszClassName = g_win32WindowManagerState.windowClassName,
            .hIconSm = nullptr  /// TODO:
        };
        ATOM windowAtom { RegisterClassEx(&windowClass) };
        Unused(windowAtom);
    }

    g_win32WindowManagerState.freeWindow = nullptr;
}

internal void WindowManager_DeInit(void)
{
    NoOp();
}

internal WM_Window WindowManager_OpenWindow(const Position2D<SInt32> position, const Size2D<UInt16> size, const FlagType<WM_WindowFlags> flags, const String8 title)
{   
    HWND hwnd { 0 };
    {
        TempArena scratchArena { ThreadContext_BeginScratchArena(nullptr, 0) };
        const String16 title16 { String16_CreateFromString8(scratchArena.arena, title) };
        hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
            g_win32WindowManagerState.windowClassName, 
            (LPCWSTR)title16.str,
            WS_OVERLAPPEDWINDOW | WS_SIZEBOX, /// TODO: 
            Flag_CheckBitIsSet<WM_WindowFlags>(flags, WM_WindowFlags::UseDefaultPosition) ?  CW_USEDEFAULT : position.x,
            Flag_CheckBitIsSet<WM_WindowFlags>(flags, WM_WindowFlags::UseDefaultPosition) ?  CW_USEDEFAULT : position.y,
            Flag_CheckBitIsSet<WM_WindowFlags>(flags, WM_WindowFlags::UseDefaultSize) ?  CW_USEDEFAULT : SafeCast<UInt16, int>(size.width),
            Flag_CheckBitIsSet<WM_WindowFlags>(flags, WM_WindowFlags::UseDefaultSize) ?  CW_USEDEFAULT : SafeCast<UInt16, int>(size.height),
            0, 
            nullptr,
            g_win32WindowManagerState.hInstance,
            nullptr);
        DragAcceptFiles(hwnd, TRUE);
        ThreadContext_EndScratchArena(scratchArena);
    }

    Win32WindowMangerWindow *win32Window { Win32_WindowManger_AllocateWindow() };
    {
        win32Window->hwnd = hwnd;
        win32Window->hdc = GetDC(hwnd);
        win32Window->dpi = SafeCast<UINT, Float32>(GetDpiForWindow(hwnd));
        win32Window->firstPaintDone = false;
        win32Window->maximized = false;
    }
    
    return Win32_WindowManger_GetWindowFromWin32Window(win32Window);
}

internal void WindowManager_CloseWindow(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    Win32_WindowManger_ReleaseWindow(win32Window);
}

internal Bool8 WindowManager_GetIsFocused(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    const HWND activeHWND { GetForegroundWindow() };
    return activeHWND == win32Window->hwnd;
}

internal Bool8 WindowManager_GetIsFullscreen(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    const LONG window_style{GetWindowLong(win32Window->hwnd, GWL_STYLE)};
    return !(window_style & WS_OVERLAPPEDWINDOW);
}

internal Bool8 WindowManager_GetIsMaximized(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    return !!(IsZoomed(win32Window->hwnd));
}

internal Bool8 WindowManager_GetIsMinimized(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    return !!(IsIconic(win32Window->hwnd));
}

internal FlagType<WM_Modifiers> WindowManager_GetModifiers(WM_Window window)
{
    Unused(window);
    FlagType<WM_Modifiers> modifiers { Flag_NoFlags<WM_Modifiers>() };
    if (GetKeyState(VK_CONTROL) & 0x8000)
    {
        modifiers = Flag_SetBit(modifiers, WM_Modifiers::Ctrl);
    }
    if (GetKeyState(VK_SHIFT) & 0x8000)
    {
        modifiers = Flag_SetBit(modifiers, WM_Modifiers::Shift);
    }
    if (GetKeyState(VK_MENU) & 0x8000)
    {
        modifiers = Flag_SetBit(modifiers, WM_Modifiers::Alt);
    }
    return modifiers;
}

internal WM_EventList WindowManager_GetEvents(Arena *arena, const Bool8 wait)
{
    g_win32EventArena = arena;
    g_win32EventList.count = 0;
    g_win32EventList.first = nullptr;
    g_win32EventList.last = nullptr;
    MSG msg { };
    if (!wait || GetMessage(&msg, 0, 0, 0))
    {
        Bool8 firstWait { wait };
        for (; firstWait || PeekMessageW(&msg, 0, 0, 0, PM_REMOVE); firstWait = false)
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return g_win32EventList;
}

internal void WindowManager_SetTitle(WM_Window window, const String8 title)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    TempArena scratchArena { ThreadContext_BeginScratchArena(nullptr, 0) };
    const String16 title16 { String16_CreateFromString8(scratchArena.arena, title) };
    SetWindowText(win32Window->hwnd, (LPCWSTR)title16.str);
    ThreadContext_EndScratchArena(scratchArena);
}

internal void WindowManager_SetPosition(WM_Window window, const Position2D<SInt32> position)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    SetWindowPos(win32Window->hwnd, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

internal void WindowManager_SetSize(WM_Window window, const Size2D<UInt16> size)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    SetWindowPos(win32Window->hwnd, nullptr, 0, 0, SafeCast<UInt16, int>(size.width), SafeCast<UInt16, int>(size.height), SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
}

internal void WindowManager_Focus(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    SetForegroundWindow(win32Window->hwnd);
    SetFocus(win32Window->hwnd);
}

internal void WindowManager_SetFullscreen(WM_Window window, const Bool8 fullscreen)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    const LONG windowStyle { GetWindowLong(win32Window->hwnd, GWL_STYLE) };
    if(fullscreen)
    {
        const Bool8 isFullscreenAlready { WindowManager_GetIsFullscreen(window) };
        if(!isFullscreenAlready)
        {
            GetWindowPlacement(win32Window->hwnd, &win32Window->lastWindowPlacement);
        }
        MONITORINFO monitorInfo { .cbSize = sizeof(monitorInfo) };
        if(GetMonitorInfo(MonitorFromWindow(win32Window->hwnd, MONITOR_DEFAULTTOPRIMARY), &monitorInfo))
        {
            SetWindowLong(win32Window->hwnd, GWL_STYLE, windowStyle & ~WS_OVERLAPPEDWINDOW);
            SetWindowPos(win32Window->hwnd, HWND_TOP, monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.top, monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left, monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        }
    }
    else
    {
        SetWindowLong(win32Window->hwnd, GWL_STYLE, windowStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(win32Window->hwnd, &win32Window->lastWindowPlacement);
        SetWindowPos(win32Window->hwnd, 0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
}

internal void WindowManager_SetMaximized(WM_Window window, const Bool8 maximized)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    if (win32Window->firstPaintDone)
    {
        ShowWindow(win32Window->hwnd, maximized ? SW_MAXIMIZE : SW_RESTORE);
    }
    else
    {
        win32Window->maximized = maximized;
    }
}

internal void WindowManager_SetMinimized(WM_Window window, const Bool8 minimized)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    if (minimized != WindowManager_GetIsMinimized(window))
    {
        ShowWindow(win32Window->hwnd, minimized ? SW_MINIMIZE : SW_RESTORE);
    }
}

internal void WindowManager_BringToFront(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    BringWindowToTop(win32Window->hwnd);
}

internal void WindowManager_DoFirstPaint(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    win32Window->firstPaintDone = true;
    ShowWindow(win32Window->hwnd, SW_SHOW);
    if(win32Window->maximized)
    {
        ShowWindow(win32Window->hwnd, SW_MAXIMIZE);
    }
}
