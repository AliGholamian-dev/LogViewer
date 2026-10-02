#ifndef BASE_THREAD_CONTEXT_HPP
#define BASE_THREAD_CONTEXT_HPP

struct ThreadContext
{
    Arena *scratchArenas[2];
};

internal ThreadContext *ThreadContext_Allocate(void);
internal void ThreadContext_Release(ThreadContext *threadContext);
internal void ThreadContext_Select(ThreadContext *threadContext);
internal ThreadContext *ThreadContext_GetSelected(void);

internal Arena *ThreadContext_GetScratchArena(Arena **conflicts, SizeType count);
internal TempArena ThreadContext_BeginScratchArena(Arena **conflicts, SizeType count);
internal void ThreadContext_EndScratchArena(TempArena scratchArena);

#endif // BASE_THREAD_CONTEXT_HPP
