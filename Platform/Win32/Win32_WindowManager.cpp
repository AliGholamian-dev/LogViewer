struct Win32WindowMangerWindow
{
    Win32WindowMangerWindow *nextFree;
    HWND hwnd;
    HDC hdc;
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
    return win32Window;
}

internal void Win32_WindowManger_ReleaseWindow(Win32WindowMangerWindow *win32Window)
{
    ReleaseDC(win32Window->hwnd, win32Window->hdc);
    DestroyWindow(win32Window->hwnd);
    win32Window->nextFree = g_win32WindowManagerState.freeWindow;
    g_win32WindowManagerState.freeWindow = win32Window;
}

internal LRESULT CALLBACK Win32_WindowManager_WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /// TODO: Produce events to be consumed by main loop, even the WM_PAINT
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
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

internal WM_Window WindowManager_OpenWindow(const Position2D<SInt32> position, const Size2D<UInt16> size, const String8 title)
{   
    HWND hwnd { 0 };
    {
        TempArena scratchArena { ThreadContext_BeginScratchArena(nullptr, 0) };
        const String16 title16 { String16_CreateFromString8(scratchArena.arena, title) };
        hwnd = CreateWindowEx(WS_EX_APPWINDOW, 
            g_win32WindowManagerState.windowClassName, 
            (WCHAR*)title16.str,
            WS_OVERLAPPEDWINDOW | WS_SIZEBOX, /// TODO: 
            position.x,
            position.y,
            SafeCast<UInt16, int>(size.width),
            SafeCast<UInt16, int>(size.height),
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
    
    return WM_Window
    {
        .impl = win32Window
    };
}

internal void WindowManager_CloseWindow(WM_Window window)
{
    Win32WindowMangerWindow *win32Window { static_cast<Win32WindowMangerWindow*>(window.impl) };
    Win32_WindowManger_ReleaseWindow(win32Window);
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
