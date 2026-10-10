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
