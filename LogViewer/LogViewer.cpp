global WM_Window g_logViewerMainWindow {};

internal void EntryPoint_Async_Update(void)
{

}

internal void GUIApp_Init(WM_Window mainWinwow)
{
    Unused(mainWinwow);

    /// TODO: load from config
    const Position2D<SInt32> position { .x = 100, .y = 100 };
    const Size2D<UInt16> size { .width  = 800, .height = 600 };
    const FlagType<WM_WindowFlags> flags { Flag_NoFlags<WM_WindowFlags>() };
    const String8 title { String8_CreateFromLiteral("Log Viewer") };
    g_logViewerMainWindow = WindowManager_OpenWindow(position, size, flags, title);
    
    
    // WindowManager_DoFirstPaint(g_logViewerMainWindow);
}

internal void GUIApp_Run(WM_Window mainWinwow)
{
    Unused(mainWinwow);

    for(Bool8 quit { false }; !quit;)
    {
    }
}
