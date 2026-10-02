C_LINKAGE thread_static ThreadContext *t_threadLocalThreadContext { };

internal ThreadContext *ThreadContext_Allocate(void)
{
    
    const ArenaParams threadContextScratchAren0aParams
    {
        .reserveSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : MB(1),
        .commitSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : KB(64),
        .optionalBackingBuffer = nullptr,
        .configFlags = SystemInfo_Get()->largePagesAllowed ? Flag_ConvertEnumToValue<ArenaConfigs>(ArenaConfigs::LargePages) : Flag_NoFlags<ArenaConfigs>()
    };
    const ArenaParams threadContextScratchAren1aParams
    {
        .reserveSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : MB(1),
        .commitSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : KB(64),
        .optionalBackingBuffer = nullptr,
        .configFlags = SystemInfo_Get()->largePagesAllowed ? Flag_ConvertEnumToValue<ArenaConfigs>(ArenaConfigs::LargePages) : Flag_NoFlags<ArenaConfigs>()
    };
    Arena *arena0 { Arena_Allocate(&threadContextScratchAren0aParams) };
    Arena *arena1 { Arena_Allocate(&threadContextScratchAren1aParams) };
    ThreadContext *threadContext { Arena_PushType<ThreadContext>(arena0) };
    threadContext->scratchArenas[0] = arena0;
    threadContext->scratchArenas[1] = arena1;
    return threadContext;
}

internal void ThreadContext_Release(ThreadContext *threadContext)
{
    Assert(threadContext != nullptr, "Null thread context");

    Arena_Release(threadContext->scratchArenas[1]);
    Arena_Release(threadContext->scratchArenas[0]);
}

internal void ThreadContext_Select(ThreadContext *threadContext)
{
    Assert(threadContext != nullptr, "Null thread context");
    
    t_threadLocalThreadContext = threadContext;
}

internal ThreadContext *ThreadContext_GetSelected(void)
{
    return t_threadLocalThreadContext;
}

internal Arena *ThreadContext_GetScratchArena(Arena **conflicts, SizeType count)
{
    ThreadContext *threadContext { ThreadContext_GetSelected() };
    Arena *scratchArena { nullptr };
    Arena **arenaPtr = threadContext->scratchArenas;
    for(SizeType i { 0 }; i < ArrayCount(threadContext->scratchArenas); i += 1, arenaPtr += 1)
    {
        Arena **conflictPtr = conflicts;
        Bool8 hasConflict { false };
        for(SizeType j { 0 }; j < count; j += 1, conflictPtr += 1)
        {
            if(*arenaPtr == *conflictPtr)
            {
                hasConflict = true;
                break;
            }
        }
        if(!hasConflict)
        {
            scratchArena = *arenaPtr;
            break;
        }
    }
    return scratchArena;
}

internal TempArena ThreadContext_BeginScratchArena(Arena **conflicts, SizeType count)
{
    return TempArena_Begin(ThreadContext_GetScratchArena(conflicts, count));
}

internal void ThreadContext_EndScratchArena(TempArena scratchArena)
{
    TempArena_End(scratchArena);
}
