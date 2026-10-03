#ifndef BASE_THREAD_HPP
#define BASE_THREAD_HPP

using ThreadEntryPointFunctionType = void(void *params);

struct Thread 
{
    void* impl;
};

struct Mutex
{
    void* impl;
};

struct RWMutex
{
    void* impl;
};

struct CondVar
{
    void* impl;
};

struct Semaphore
{
    void* impl;
};

struct Barrier
{
    void* impl;
};

internal Thread Thread_Launch(ThreadEntryPointFunctionType *entryPointFunction, void *params);
internal Bool8 Thread_Join(Thread thread, const MilliSeconds waitTimeInMilliSeconds);
internal void Thread_Detach(Thread thread);
internal void Thread_CallThreadEntryPoint(ThreadEntryPointFunctionType *entryPointFunction, void *params);

internal UInt32 Thread_GetID(void);
internal void Thread_SetName(const String8 name);

internal Mutex Mutex_Allocate(void);
internal void Mutex_Release(Mutex mutex);
internal void Mutex_Take(Mutex mutex);
internal void Mutex_Drop(Mutex mutex);

internal RWMutex RWMutex_Allocate(void);
internal void RWMutex_Release(RWMutex rwMutex);
internal void RWMutex_Take(RWMutex rwMutex, const Bool8 isWriteMode);
internal void RWMutex_Drop(RWMutex rwMutex, const Bool8 isWritemode);
internal void RWMutex_TakeRead(RWMutex rwMutex);
internal void RWMutex_TakeWrite(RWMutex rwMutex);
internal void RWMutex_DropRead(RWMutex rwMutex);
internal void RWMutex_DropWrite(RWMutex rwMutex);

internal CondVar CondVar_Allocate(void);
internal void CondVar_Release(CondVar condVar);
internal Bool8 CondVar_Wait(CondVar condVar, Mutex mutex, const MilliSeconds waitTimeInMilliSeconds);
internal Bool8 CondVar_Wait_RW(CondVar condVar, RWMutex rwMutex, const Bool8 isWriteMode, const MilliSeconds waitTimeInMilliSeconds);
internal Bool8 CondVar_Wait_RW_Read(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds);
internal Bool8 CondVar_Wait_RW_Write(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds);
internal void CondVar_Signal(CondVar condVar);
internal void CondVar_Broadcast(CondVar condVar);

internal Semaphore Semaphore_Allocate(const UInt32 initialCount, const UInt32 maxCount, const String8 name);
internal Semaphore Semaphore_Open(const String8 name);
internal void Semaphore_Release(Semaphore semaphore);
internal void Semaphore_Close(Semaphore semaphore);
internal Bool8 Semaphore_Take(Semaphore semaphore, const MilliSeconds waitTimeInMilliSeconds);
internal void Semaphore_Drop(Semaphore semaphore);
internal void Semaphore_DropCount(Semaphore semaphore, const UInt32 dropCount);

internal Barrier Barrier_Allocate(UInt64 count);
internal void Barrier_Release(Barrier barrier);
internal void Barrier_Wait(Barrier barrier);

#define MutexScope(mutex) DeferLoop(Mutex_Take(mutex), Mutex_Drop(mutex))
#define RWMutexScope(rwMutex, isWriteMmode) DeferLoop(RWMutex_Take((rwMutex), (isWriteMmode)), RWMutex_Drop((rwMutex), (isWriteMmode)))
#define RWMutexScopeRead(rwMutex) DeferLoop(RWMutex_TakeRead(rwMutex), RWMutex_DropRead(rwMutex))
#define RWMutexScopeWrite(rwMutex) DeferLoop(RWMutex_TakeWrite(rwMutex), RWMutex_DropWrite(rwMutex))
#define RWMutexScopeRWPromote(rwMutex) DeferLoop((RWMutex_DropRead(rwMutex), RWMutex_TakeWrite(rwMutex)), (RWMutex_DropWrite(rwMutex), RWMutex_TakeRead(rwMutex)))

#endif // BASE_THREAD_HPP
