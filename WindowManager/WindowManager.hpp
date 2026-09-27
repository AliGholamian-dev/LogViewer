#ifndef WINDOW_MANAGER_WINDOW_MANAGER_HPP
#define WINDOW_MANAGER_WINDOW_MANAGER_HPP

struct WM_Window
{
    void* impl;
};

internal void WindowManager_Init(void);
internal void WindowManager_DeInit(void);

internal WM_Window WindowManager_OpenWindow();
internal void WindowManager_CloseWindow(WM_Window window);

#endif // WINDOW_MANAGER_WINDOW_MANAGER_HPP
