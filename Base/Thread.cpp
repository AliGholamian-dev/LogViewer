internal void Thread_CallThreadEntryPoint(ThreadEntryPointFunctionType *entryPointFunction, void *params)
{
    Assert(entryPointFunction != nullptr, "Null entry point function");

    ThreadContext *threadContext { ThreadContext_Allocate() };
    ThreadContext_Select(threadContext);
    entryPointFunction(params);
    ThreadContext_Release(threadContext);
}

internal void RWMutex_TakeRead(RWMutex rwMutex)
{
    RWMutex_Take(rwMutex, false);
}

internal void RWMutex_TakeWrite(RWMutex rwMutex)
{
    RWMutex_Take(rwMutex, true);
}

internal void RWMutex_DropRead(RWMutex rwMutex)
{
    RWMutex_Drop(rwMutex, false);
}

internal void RWMutex_DropWrite(RWMutex rwMutex)
{
    RWMutex_Drop(rwMutex, true);
}

internal Bool8 CondVar_Wait_RW_Read(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds)
{
    return CondVar_Wait_RW(condVar, rwMutex, false, waitTimeInMilliSeconds);
}
internal Bool8 CondVar_Wait_RW_Write(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds)
{
    return CondVar_Wait_RW(condVar, rwMutex, true, waitTimeInMilliSeconds);
}

internal void Semaphore_Drop(Semaphore semaphore)
{
    Semaphore_DropCount(semaphore, 1);
}
