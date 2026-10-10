global CondVar g_asyncTickStartCondVar { };
global Mutex g_asyncTickStartMutex { };
global Mutex g_asyncTickStopMutex { };
global Bool32 g_asyncLoopAgain { false };
global Bool32 g_asyncExit { false };

#if !defined(NEED_ASYNC)
    #define NEED_ASYNC 0
#endif

internal void EntryPoint_Async_RequestUpdate(void)
{
    Atomic_EvaluateAndAssign<Bool32>(&g_asyncLoopAgain, true);
}

internal void EntryPoint_Async_Enter(void *params)
{
    LaneContext laneContext { *static_cast<LaneContext*>(params) };
    Lane_SetContext(laneContext);
    ScratchArenaScope(scratchArena, nullptr, 0)
    {
        Thread_SetName(String8_CreateFromFormattedString(scratchArena.arena, "async_thread_%I64u", Lane_GetIndex()));
    }
    for (Bool32 exit { false }; !exit;)
    {
        /// Wait for update request
        if (Lane_CheckIsMainLaneInLaneGroup())
        {
            if (!Atomic_Evaluate<Bool32>(&g_asyncLoopAgain))
            {
                MutexScope(g_asyncTickStartMutex)
                {
                    const MilliSeconds waitTime { .value = 1000 };
                    CondVar_WaitFor(g_asyncTickStartCondVar, g_asyncTickStartMutex, waitTime);
                }
            }
            Atomic_EvaluateAndAssign<Bool32>(&g_asyncLoopAgain, false);
        }
        Lane_Sync();

        /// Run uodate
        EntryPoint_Async_Update();
        Lane_Sync();
        
        /// Check exit condition
        if (Lane_CheckIsMainLaneInLaneGroup())
        {
            exit = Atomic_Evaluate<Bool32>(&g_asyncExit);
        }
        Lane_SyncAndBroadcastData(&exit, sizeof(exit), Lane_GetMainLaneIndex());
    }
}

internal void EntryPoint_CallMainThreadEntryPoint(void)
{
    Thread_SetName(String8_CreateFromLiteral("main_thread"));
    ScratchArenaScope(scratchArena, nullptr, 0)
    {
        g_asyncTickStartCondVar = CondVar_Create();
        g_asyncTickStartMutex = Mutex_Create();
        g_asyncTickStopMutex = Mutex_Create();

        #if NEED_ASYNC
            Thread *asyncThreads { nullptr };
            SizeType asyncThreadCount { 0 };
            UInt64 laneBroadcastValue { 0 };
            {
                local_persist constexpr SizeType mainThreadCount { 1 };
                asyncThreadCount = SystemInfo_Get()->logicalProcessorCount;
                const SizeType clampedMainThreadCount { MinOf<SizeType>(asyncThreadCount, mainThreadCount) };
                asyncThreadCount -= clampedMainThreadCount;
                asyncThreadCount = MaxOf<SizeType>(1, asyncThreadCount);
                Barrier barrier { Barrier_Create(asyncThreadCount) };
                LaneContext *laneContext = Arena_PushArrayAndZero<LaneContext>(scratchArena.arena, asyncThreadCount);
                asyncThreads = Arena_PushArrayAndZero<Thread>(scratchArena.arena, asyncThreadCount);
                for(SizeType i {0}; i < asyncThreadCount; ++i)
                {
                    laneContext[i].index = i;
                    laneContext[i].count = asyncThreadCount;
                    laneContext[i].barrier = barrier;
                    laneContext[i].broadcastMemory = &laneBroadcastValue;
                    asyncThreads[i] = Thread_Launch(EntryPoint_Async_Enter, &laneContext[i], Arena_CreateDefaultArenaParams());
                }
            }
        #endif

        EntryPoint_Main_InitApplication();
        EntryPoint_Main_RunApplication();

        Atomic_EvaluateAndAssign<Bool32>(&g_asyncExit, true);
        Atomic_EvaluateAndAssign<Bool32>(&g_asyncLoopAgain, true);
        CondVar_NotifyAll(g_asyncTickStartCondVar);
        #if NEED_ASYNC
            for(SizeType i {0}; i < asyncThreadCount; ++i)
            {
                const MilliSeconds waitTime { .value = GetHighestNumericLimitOf<MilliSeconds::RepresentationType>() };
                Thread_Join(asyncThreads[i], waitTime);
            }
        #endif

    }
}
