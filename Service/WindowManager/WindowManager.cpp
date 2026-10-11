internal Bool8 WindowManager_CheckWindowsMatch(WM_Window windowA, WM_Window windowB)
{
    return windowA.impl == windowB.impl;
}

internal WM_Event *WindowManager_PushNewEventToEventList(Arena *arena, WM_EventList *eventList, WM_EventKind eventKind)
{
    Assert(arena != nullptr, "Null arena");
    Assert(eventList != nullptr, "Null event list");
    WM_Event *event { Arena_PushTypeAndZero<WM_Event>(arena) };
    DLL_PushBack(eventList->first, eventList->last, event);
    eventList->count += 1;
    event->timestamp = GetNowTime<TimestampClock>();
    event->kind = eventKind;
    return event;
}

internal void WindowManager_EatEvent(WM_EventList *events, WM_Event *event)
{
    DLL_Remove(events->first, events->last, event);
    events->count -= 1;
}
