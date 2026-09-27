internal int EntryPoint_MainThread(void)
{
    /// TODO: Introduce async thread, which does some work then calls aysnc tick, aysnc tick will be implemented by each app based on its sub systems and ...
    /// TODO: maybe run main alos with multiple threads, not needed now (One should be exactly this thread)
    EntryPoint_EnterApplicationMain();

    /// TODO: Wait for all threads (main and async to join)
    return 0;
}

internal void EntryPoint_SupplementThread(ThreadEntryPointFunctionType *entryPointFunction, void *params)
{
    ThreadContext *threadContext { ThreadContext_Allocate() };
    ThreadContext_Select(threadContext);
    entryPointFunction(params);
    ThreadContext_Release(threadContext);
}
