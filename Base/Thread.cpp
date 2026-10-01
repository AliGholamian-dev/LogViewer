internal void Thread_CallThreadEntryPoint(ThreadEntryPointFunctionType *entryPointFunction, void *params)
{
    Assert(entryPointFunction != nullptr, "Null entry point function");

    ThreadContext *threadContext { ThreadContext_Allocate() };
    ThreadContext_Select(threadContext);
    entryPointFunction(params);
    ThreadContext_Release(threadContext);
}
