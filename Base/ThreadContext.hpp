#ifndef BASE_THREAD_CONTEXT_HPP
#define BASE_THREAD_CONTEXT_HPP

struct LaneContext
{
    UInt64 laneIndex;
    UInt64 laneCount;
    Barrier barrier;
    UInt64 *broadcastMemory;
};

struct ThreadContext
{
    Arena *scratchArenas[2];
    LaneContext laneContext;
};

internal ThreadContext *ThreadContext_Allocate(void);
internal void ThreadContext_Release(ThreadContext *threadContext);
internal void ThreadContext_Select(ThreadContext *threadContext);
internal ThreadContext *ThreadContext_GetSelected(void);

internal Arena *ThreadContext_GetScratchArena(Arena **conflicts, SizeType count);
internal TempArena ThreadContext_BeginScratchArena(Arena **conflicts, SizeType count);
internal void ThreadContext_EndScratchArena(TempArena scratchArena);

internal LaneContext ThreadContext_SetLaneContext(LaneContext laneContext);
internal void ThreadContext_WaitOnLaneBarrier(void *broadcastData, SizeType broadcastSize, UInt64 broadcastSourceLaneIndex);

LaneContext Lane_SetContext(LaneContext laneContext);
UInt64 Lane_GetIndex(void);
UInt64 Lane_GetCount(void);
UInt64 Lane_GetIndexFromTaskIndex(const UInt64 index);
Range1<UInt64> Lane_GetRange(const UInt64 elementCount);
void Lane_Sync();
void Lane_SyncAndBroadcastData(void* data, SizeType dataSize, UInt64 sourceLaneIndex);

#endif // BASE_THREAD_CONTEXT_HPP
