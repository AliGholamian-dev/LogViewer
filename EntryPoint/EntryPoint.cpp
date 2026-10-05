global CondVar g_asyncTickStartVondVar { };
global Mutex g_asyncTickStartMutex { };
global Mutex g_asyncTickStopMutex { };
global Bool8 g_asyncLoopAgain { false };
global Bool8 g_asyncExit { false };

#if !defined(NEED_ASYNC)
    #define NEED_ASYNC 0
#endif

internal void EntryPoint_Async_RequestUpdate(void)
{
    const Bool8 prevValue { Atomic_EvalAndAssign<Bool8>(&g_asyncLoopAgain, true) };
    Unused(prevValue);
}

internal void EntryPoint_Async_Enter(void *params)
{
    LaneContext laneContext { *static_cast<LaneContext*>(params) };
    Lane_SetContext(laneContext);
    // ThreadNameF("async_thread_%I64u", lane_idx()); TODO:
    Bool8 exit { false };
    while (!exit)
    {
        if (Lane_GetIndex() == 0)
        {
            if (!Atomic_Eval<Bool8>(&g_asyncLoopAgain))
            {
                MutexScope(g_asyncTickStartMutex)
                {
                    const MilliSeconds waitTime { .value = 1000 };
                    CondVar_Wait(g_asyncTickStartVondVar, g_asyncTickStartMutex, waitTime);
                }
            }
            const Bool8 asyncLoopAgainPrevValue { Atomic_EvalAndAssign<Bool8>(&g_asyncLoopAgain, false) };
            Unused(asyncLoopAgainPrevValue);
        }
        Lane_Sync();

        EntryPoint_Async_Update();

        Lane_Sync();
        
        if (Lane_GetIndex() == 0)
        {
            exit = Atomic_Eval<Bool8>(&g_asyncExit);
        }
        Lane_SyncAndBroadcastData(&exit, sizeof(exit), 0);
    }
}

internal void EntryPoint_CallMainThreadEntryPoint(void)
{
    Thread_SetName(String8_CreateFromLiteral("main_thread"));

    TempArena scratchArena{ ThreadContext_BeginScratchArena(nullptr, 0) };

    g_asyncTickStartVondVar = CondVar_Allocate();
    g_asyncTickStartMutex = Mutex_Allocate();
    g_asyncTickStopMutex = Mutex_Allocate();

    #if NEED_ASYNC
        Thread *asyncThreads { nullptr };
        UInt64 asyncThreadCount { 0 };
        UInt64 laneBroadcastValue { 0 };
        {
            local_persist constexpr UInt64 mainThreadCount { 1 };
            asyncThreadCount = SystemInfo_Get()->logicalProcessorCount;
            const UInt64 clampedMainThreadCount { MinOf<UInt64>(asyncThreadCount, mainThreadCount) };
            asyncThreadCount -= clampedMainThreadCount;
            asyncThreadCount = MaxOf<UInt64>(1, asyncThreadCount);
            Barrier barrier { Barrier_Allocate(asyncThreadCount) };
            LaneContext *laneContext = Arena_PushArrayAndZero<LaneContext>(scratchArena.arena, asyncThreadCount);
            asyncThreads = Arena_PushArrayAndZero<Thread>(scratchArena.arena, asyncThreadCount);
            for(SizeType i {0}; i < asyncThreadCount; ++i)
            {
                laneContext[i].laneIndex = i;
                laneContext[i].laneCount = asyncThreadCount;
                laneContext[i].barrier = barrier;
                laneContext[i].broadcastMemory = &laneBroadcastValue;
                asyncThreads[i] = Thread_Launch(EntryPoint_Async_Enter, &laneContext[i]);
            }
        }
    #endif

    EntryPoint_Main_InitApplication();
    EntryPoint_Main_RunApplication();

    #if NEED_ASYNC
        Atomic_EvalAndAssign<Bool8>(&g_asyncExit, true);
        Atomic_EvalAndAssign<Bool8>(&g_asyncLoopAgain, true);
        CondVar_Broadcast(g_asyncTickStartVondVar);
        for(SizeType i {0}; i < asyncThreadCount; ++i)
        {
            const MilliSeconds waitTime { .value = GetHighestNumericLimitOf<MilliSeconds::RepresentationType>() };
            Thread_Join(asyncThreads[i], waitTime);
        }
    #endif

    ThreadContext_EndScratchArena(scratchArena);
}
