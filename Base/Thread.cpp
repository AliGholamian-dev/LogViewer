internal void Thread_AcquireThreadContexAndCallEntryPoint(ThreadEntryPointFunctionType *entryPointFunction, void *params, const ArenaParams* scratchArenaParams)
{
    Assert(entryPointFunction != nullptr, "Null entry point function");
    ThreadContext *threadContext { ThreadContext_Allocate(scratchArenaParams) };
    ThreadContext_Select(threadContext);
    entryPointFunction(params);
    ThreadContext_Release(threadContext);
}

internal void RWMutex_TakeRead(RWMutex rwMutex)
{
    RWMutex_TakeReadOrWrite(rwMutex, false);
}

internal void RWMutex_TakeWrite(RWMutex rwMutex)
{
    RWMutex_TakeReadOrWrite(rwMutex, true);
}

internal void RWMutex_DropRead(RWMutex rwMutex)
{
    RWMutex_DropReadOrWrite(rwMutex, false);
}

internal void RWMutex_DropWrite(RWMutex rwMutex)
{
    RWMutex_DropReadOrWrite(rwMutex, true);
}

internal Bool8 CondVar_WaitRWReadFor(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds)
{
    return CondVar_WaitRWFor(condVar, rwMutex, false, waitTimeInMilliSeconds);
}
internal Bool8 CondVar_WaitRWWriteFor(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds)
{
    return CondVar_WaitRWFor(condVar, rwMutex, true, waitTimeInMilliSeconds);
}

internal void Semaphore_Drop(Semaphore semaphore)
{
    Semaphore_DropBy(semaphore, 1);
}
