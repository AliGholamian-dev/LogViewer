#ifndef BASE_THREAD_CONTEXT_HPP
#define BASE_THREAD_CONTEXT_HPP

struct LaneContext
{
    SizeType index;
    SizeType count;
    Barrier barrier;
    UInt64 *broadcastMemory;
};

struct ThreadContext
{
    Arena *scratchArena;
    LaneContext laneContext;
};

internal ThreadContext *ThreadContext_Allocate(const ArenaParams* scratchArenaParams);
internal void ThreadContext_Release(ThreadContext *threadContext);
internal void ThreadContext_Select(ThreadContext *threadContext);
internal ThreadContext *ThreadContext_GetSelected(void);

internal Arena *ThreadContext_GetScratchArena(Arena **conflicts, SizeType count);
internal TempArena ThreadContext_BeginScratchArena(Arena **conflicts, SizeType count);
internal void ThreadContext_EndScratchArena(TempArena scratchArena);
internal LaneContext ThreadContext_SetLaneContext(LaneContext laneContext);
internal void ThreadContext_WaitOnLaneBarrierAndBroadcastData(void *broadcastData, SizeType broadcastSize, SizeType broadcastSourceLaneIndex);

#define ScratchArenaScope(scratchArenaName, conflicts, count) for (TempArena scratchArenaName = ThreadContext_BeginScratchArena(conflicts, count), *Glue(_ScratchArenaOnce_, __LINE__) = &scratchArenaName; Glue(_ScratchArenaOnce_, __LINE__) != nullptr; ThreadContext_EndScratchArena(scratchArenaName), Glue(_ScratchArenaOnce_, __LINE__) = nullptr)

LaneContext Lane_SetContext(LaneContext laneContext);
SizeType Lane_GetIndex(void);
SizeType Lane_GetCount(void);
Range1<SizeType> Lane_GetRange(const SizeType elementCount);
void Lane_Sync();
void Lane_SyncAndBroadcastData(void* data, SizeType dataSize, SizeType sourceLaneIndex);

#endif // BASE_THREAD_CONTEXT_HPP
