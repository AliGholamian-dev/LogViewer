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

internal LaneContext ThreadContext_SetLaneContext(LaneContext laneContext)
{
    ThreadContext *threadContext { ThreadContext_GetSelected() };
    LaneContext restore { threadContext->laneContext };
    threadContext->laneContext = laneContext;
    return restore;
}

internal void ThreadContext_WaitOnLaneBarrier(void *broadcastData, SizeType broadcastSize, UInt64 broadcastSourceLaneIndex)
{
    ThreadContext *threadContext { ThreadContext_GetSelected() };
  
    const SizeType clampedBroadcastSize { ClampTop<SizeType>(broadcastSize, sizeof(threadContext->laneContext.broadcastMemory[0])) };

    if(broadcastData != nullptr && Lane_GetIndex() == broadcastSourceLaneIndex)
    {
        Memory_Copy(threadContext->laneContext.broadcastMemory, broadcastData, clampedBroadcastSize);
    }

    Barrier_Wait(threadContext->laneContext.barrier);

    if(broadcastData != nullptr && Lane_GetIndex() != broadcastSourceLaneIndex)
    {
        Memory_Copy(broadcastData, threadContext->laneContext.broadcastMemory, clampedBroadcastSize);
    }

    if(broadcastData != nullptr)
    {
        Barrier_Wait(threadContext->laneContext.barrier);
    }
}

LaneContext Lane_SetContext(LaneContext laneContext)
{
    return ThreadContext_SetLaneContext(laneContext);
}

UInt64 Lane_GetIndex(void)
{
    return ThreadContext_GetSelected()->laneContext.laneIndex;
}

UInt64 Lane_GetCount(void)
{
    return ThreadContext_GetSelected()->laneContext.laneCount;
}

UInt64 Lane_GetIndexFromTaskIndex(const UInt64 index)
{
    return index % Lane_GetCount();
}

Range1<UInt64> Lane_GetRange(const UInt64 elementCount)
{
    return Math_GetSubdivisionRange(Lane_GetIndex(), Lane_GetCount(), elementCount);
}

void Lane_Sync()
{
    ThreadContext_WaitOnLaneBarrier(nullptr, 0, 0);
}

void Lane_SyncAndBroadcastData(void* data, SizeType dataSize, UInt64 sourceLaneIndex)
{
    ThreadContext_WaitOnLaneBarrier(data, dataSize, sourceLaneIndex);
}
