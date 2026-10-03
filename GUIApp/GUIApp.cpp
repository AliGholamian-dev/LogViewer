global WM_Window g_mainWindow {};

#if !defined(NEED_MAIN_WINDOW)
    #define NEED_MAIN_WINDOW 1
#endif

internal void EntryPoint_Main_InitApplication(void)
{
    WindowManager_Init();
    #if NEED_MAIN_WINDOW
    {
        const Position2D<SInt32> position { .x = 0, .y = 0 };
        const Size2D<UInt16> size { .width  = 0, .height = 0 };
        FlagType<WM_WindowFlags> flags { Flag_NoFlags<WM_WindowFlags>() };
        flags = Flag_SetBit<WM_WindowFlags>(flags, WM_WindowFlags::UseDefaultPosition);
        flags = Flag_SetBit<WM_WindowFlags>(flags, WM_WindowFlags::UseDefaultSize);
        const String8 title { String8_CreateFromLiteral("") };
        g_mainWindow = WindowManager_OpenWindow(position, size, flags, title);
    }
    #endif
}

internal void EntryPoint_Main_RunApplication(void)
{
    GUIApp_Init(g_mainWindow);
    /// TODO: Maybe poll and translate window events here and pass to run?
    GUIApp_Run(g_mainWindow);

    #if NEED_MAIN_WINDOW
        WindowManager_CloseWindow(g_mainWindow);
    #endif
    WindowManager_DeInit();
}
