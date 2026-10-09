C_LINKAGE thread_local ThreadContext *t_threadContext { nullptr };

internal ThreadContext *ThreadContext_Allocate(const ArenaParams* scratchArenaParams)
{
    const ArenaParams defaultScratchArenaParams { Arena_CreateDefaultArenaParams() };
    Arena *scratchArena { Arena_Allocate(scratchArenaParams != nullptr ? scratchArenaParams : &defaultScratchArenaParams) };
    ThreadContext *threadContext { Arena_PushTypeAndZero<ThreadContext>(scratchArena) };
    threadContext->scratchArena = scratchArena;
    return threadContext;
}

internal void ThreadContext_Release(ThreadContext *threadContext)
{
    Assert(threadContext != nullptr, "Null thread context");
    Arena_Release(threadContext->scratchArena);
}

internal void ThreadContext_Select(ThreadContext *threadContext)
{
    Assert(threadContext != nullptr, "Null thread context");
    t_threadContext = threadContext;
}

internal ThreadContext *ThreadContext_GetSelected(void)
{
    return t_threadContext;
}

internal Arena *ThreadContext_GetScratchArena(Arena **conflicts, SizeType count)
{
    ThreadContext *threadContext { ThreadContext_GetSelected() };
    Bool8 hasConflict { false };
    for(SizeType j { 0 }; j < count; ++j)
    {
        if(threadContext->scratchArena == conflicts[j])
        {
            hasConflict = true;
            break;
        }
    }
    Arena *scratchArena { nullptr };
    if(!hasConflict)
    {
        scratchArena = threadContext->scratchArena;
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

internal void ThreadContext_WaitOnLaneBarrierAndBroadcastData(void *broadcastData, SizeType broadcastSize, SizeType broadcastSourceLaneIndex)
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

SizeType Lane_GetIndex(void)
{
    return ThreadContext_GetSelected()->laneContext.index;
}

SizeType Lane_GetCount(void)
{
    return ThreadContext_GetSelected()->laneContext.count;
}

Range1<SizeType> Lane_GetRange(const SizeType elementCount)
{
    return GetSubdivisionIndexRange<SizeType>(Lane_GetIndex(), Lane_GetCount(), elementCount);
}

void Lane_Sync()
{
    ThreadContext_WaitOnLaneBarrierAndBroadcastData(nullptr, 0, 0);
}

void Lane_SyncAndBroadcastData(void* data, SizeType dataSize, SizeType sourceLaneIndex)
{
    ThreadContext_WaitOnLaneBarrierAndBroadcastData(data, dataSize, sourceLaneIndex);
}
