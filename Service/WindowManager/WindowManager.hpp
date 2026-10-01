#ifndef SERVICE_WINDOW_MANAGER_WINDOW_MANAGER_HPP
#define SERVICE_WINDOW_MANAGER_WINDOW_MANAGER_HPP

/// TODO: Maybe instead of global/static window state in platform port, introduce a WM struct
struct WM_Window
{
    void* impl;
};

internal void WindowManager_Init(void);
internal void WindowManager_DeInit(void);
internal WM_Window WindowManager_OpenWindow(const Position2D<SInt32> position, const Size2D<UInt16> size, const String8 title);
internal void WindowManager_CloseWindow(WM_Window window);

#endif // SERVICE_WINDOW_MANAGER_WINDOW_MANAGER_HPP
