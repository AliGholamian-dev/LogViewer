C_LINKAGE thread_static ThreadContext *t_threadLocalThreadContext { };

internal ThreadContext *ThreadContext_Allocate(void)
{
    const ArenaParams threadContextScratchAren0aParams
    {
        .reserveSizeInBytes = MB(1),
        .commitSizeInBytes = KB(64),
        .optionalBackingBuffer = nullptr,
        .configFlags = Flag_NoFlags<FlagType<ArenaConfigs>>()
    };
    const ArenaParams threadContextScratchAren1aParams
    {
        .reserveSizeInBytes = MB(1),
        .commitSizeInBytes = KB(64),
        .optionalBackingBuffer = nullptr,
        .configFlags = Flag_NoFlags<FlagType<ArenaConfigs>>()
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
