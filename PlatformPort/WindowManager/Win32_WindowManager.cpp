struct Win32WindowManagerState
{
    Arena *arena;
    HINSTANCE hInstance;
    LPCTSTR windowClassName { L"graphical-window" };
};

global Win32WindowManagerState g_win32WindowManagerState {};

internal LRESULT Win32_WindowManager_WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    /// TODO: Produce events to be consumed by main loop, even the WM_PAINT
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

internal void WindowManager_Init(void)
{
    {
        const ArenaParams windowManagerArenaParams
        {
            .reserveSizeInBytes = MB(64),
            .commitSizeInBytes = KB(64),
            .optionalBackingBuffer = nullptr,
            .configFlags = Flag_NoFlags<FlagType<ArenaConfigs>>()
        };
        g_win32WindowManagerState.arena = Arena_Allocate(&windowManagerArenaParams);
    }
    g_win32WindowManagerState.hInstance = Win32_GetModule();
    
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
            .cbClsExtra = 0,
            .cbWndExtra = 0,
            .hInstance = g_win32WindowManagerState.hInstance,
            .hIcon = NULL, /// TODO: From resources when added and bundled in exe
            .hCursor = LoadCursor(NULL, IDC_ARROW),
            .hbrBackground = NULL,
            .lpszMenuName = NULL,
            .lpszClassName = g_win32WindowManagerState.windowClassName,
            .hIconSm = NULL
        };
        ATOM windowAtom { RegisterClassEx(&windowClass) };
       Unused(windowAtom);
    }
}

internal void WindowManager_DeInit(void)
{
    NoOp();
}

internal WM_Window WindowManager_OpenWindow()
{   
    HWND hwnd { 0 };
    {
        hwnd = CreateWindowExW(WS_EX_APPWINDOW, 
            g_win32WindowManagerState.windowClassName, 
            L"Log Viewer", 
            WS_OVERLAPPEDWINDOW | WS_SIZEBOX, 
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0, 
            0,
            g_win32WindowManagerState.hInstance,
            0);
        DragAcceptFiles(hwnd, 1);
    }

    /// TODO: This is where your architecture falls apart, you need to put everything in one file
    /// or have awin32 specific platform infor which you pass around in you code (wether it should have common base or get through API)
    ShowWindow(hwnd, SW_SHOWNORMAL);
    return WM_Window{};
}

internal void WindowManager_CloseWindow(WM_Window window)
{

}