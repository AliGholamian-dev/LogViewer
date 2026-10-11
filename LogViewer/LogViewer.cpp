/// TODO: Introduce window states which holds each window handle and some state
struct LogViewerState
{
    WM_Window mainWindow;
    Arena* frameArena;
    SizeType framesRequestedCount;
};

global LogViewerState g_logViewerState {};

/// TODO: Change to open windos from config
internal void LogViewer_OpenMainWindow(void)
{
    /// TODO: load from config
    const Position2D<SInt32> position { .x = 100, .y = 100 };
    const Size2D<UInt16> size { .width  = 800, .height = 600 };
    const FlagType<WM_WindowFlags> flags { Flag_NoFlags<WM_WindowFlags>() };
    const String8 title { String8_CreateFromLiteral("Log Viewer") };
    g_logViewerState.mainWindow = WindowManager_OpenWindow(position, size, flags, title);
    WindowManager_DoFirstPaint(g_logViewerState.mainWindow);
}

internal void LogViewer_InitFrameArena(void)
{
    const SizeType clampedReserveSize { ClampTop<SizeType>(SystemInfo_Get()->largePageSize, MB(16)) };
    const ArenaParams frameArenaParams
    {
        .reserveSizeInBytes = SystemInfo_Get()->largePagesAllowed ?  clampedReserveSize : MB(16),
        .commitSizeInBytes = SystemInfo_Get()->largePagesAllowed ? clampedReserveSize : KB(64),
        .optionalBackingBuffer = nullptr,
        .configFlags = SystemInfo_Get()->largePagesAllowed ? Flag_ConvertEnumToValue<ArenaConfigs>(ArenaConfigs::LargePages) : Flag_NoFlags<ArenaConfigs>()
    };
    g_logViewerState.frameArena = Arena_Allocate(&frameArenaParams);
}

internal void LogViewer_RequestFrame(void)
{
    g_logViewerState.framesRequestedCount = 4;
}

internal void LogViewer_MarkFrameProcessed(void)
{
    if(g_logViewerState.framesRequestedCount > 0)
    {
        g_logViewerState.framesRequestedCount -= 1;
    }
}

internal void EntryPoint_Async_Update(void)
{

}

internal void GUIApp_Init(WM_Window mainWinwow)
{
    Unused(mainWinwow);
    LogViewer_OpenMainWindow();
    LogViewer_InitFrameArena();
}

internal void GUIApp_Run(WM_Window mainWinwow)
{
    Unused(mainWinwow);
    /// TODO: This is the place to react to command line, maybe have exec mode, and in noraml hui mode run the event loop in a separate function, for now not needed
    for(Bool8 quit { false }; !quit;)
    {
        Arena_Clear(g_logViewerState.frameArena);
        WM_EventList events 
        {
            .count = 0,
            .first = nullptr,
            .last = nullptr
        };
        events = WindowManager_GetEvents(g_logViewerState.frameArena, g_logViewerState.framesRequestedCount == 0);
        for(WM_Event *event = events.first, *nextEvent = nullptr; event != nullptr; event = nextEvent)
        {
            /// TODO: Produce commands and move this to a function
            switch (event->kind)
            {
                case WM_EventKind::WindowClose:
                {
                    /// TODO: Do this in the command not here
                    if(WindowManager_CheckWindowsMatch(event->window, g_logViewerState.mainWindow))
                    {
                        quit = true;
                    }
                } break;
                case WM_EventKind::WindowPaint:
                {
                } break;
                case WM_EventKind::Null:
                case WM_EventKind::Count:
                default:
                {
                } break;
            }
            nextEvent = event->next;
            WindowManager_EatEvent(&events, event);
        }
        LogViewer_MarkFrameProcessed();
    }
}
