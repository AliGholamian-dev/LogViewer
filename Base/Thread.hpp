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

internal Thread Thread_Launch(ThreadEntryPointFunctionType *entryPointFunction, void *params, const ArenaParams scratchArenaParams);
internal Bool8 Thread_Join(Thread thread, const MilliSeconds waitTimeInMilliSeconds);
internal void Thread_Detach(Thread thread);
internal void Thread_AcquireThreadContexAndCallEntryPoint(ThreadEntryPointFunctionType *entryPointFunction, void *params, const ArenaParams* scratchArenaParams);
internal UInt32 Thread_GetID(void);
internal void Thread_SetName(const String8 name);

internal Mutex Mutex_Create(void);
internal void Mutex_Destory(Mutex mutex);
internal void Mutex_Take(Mutex mutex);
internal void Mutex_Drop(Mutex mutex);

internal RWMutex RWMutex_Create(void);
internal void RWMutex_Destory(RWMutex rwMutex);
internal void RWMutex_TakeReadOrWrite(RWMutex rwMutex, const Bool8 isWriteMode);
internal void RWMutex_DropReadOrWrite(RWMutex rwMutex, const Bool8 isWritemode);
internal void RWMutex_TakeRead(RWMutex rwMutex);
internal void RWMutex_TakeWrite(RWMutex rwMutex);
internal void RWMutex_DropRead(RWMutex rwMutex);
internal void RWMutex_DropWrite(RWMutex rwMutex);

internal CondVar CondVar_Create(void);
internal void CondVar_Destory(CondVar condVar);
internal Bool8 CondVar_WaitFor(CondVar condVar, Mutex mutex, const MilliSeconds waitTimeInMilliSeconds);
internal Bool8 CondVar_WaitRWFor(CondVar condVar, RWMutex rwMutex, const Bool8 isWriteMode, const MilliSeconds waitTimeInMilliSeconds);
internal Bool8 CondVar_WaitRWReadFor(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds);
internal Bool8 CondVar_WaitRWWriteFor(CondVar condVar, RWMutex rwMutex, const MilliSeconds waitTimeInMilliSeconds);
internal void CondVar_NotifyOne(CondVar condVar);
internal void CondVar_NotifyAll(CondVar condVar);

internal Semaphore Semaphore_Create(const UInt32 initialCount, const UInt32 maxCount, const String8 name);
internal void Semaphore_Destory(Semaphore semaphore);
internal Semaphore Semaphore_Open(const String8 name);
internal void Semaphore_Close(Semaphore semaphore);
internal Bool8 Semaphore_Take(Semaphore semaphore, const MilliSeconds waitTimeInMilliSeconds);
internal void Semaphore_DropOne(Semaphore semaphore);
internal void Semaphore_DropBy(Semaphore semaphore, const UInt32 dropCount);

internal Barrier Barrier_Create(const UInt32 count);
internal void Barrier_Destory(Barrier barrier);
internal void Barrier_Wait(Barrier barrier);

#define MutexScope(mutex) DeferLoop(Mutex_Take(mutex), Mutex_Drop(mutex))
#define RWMutexScope(rwMutex, isWriteMode) DeferLoop(RWMutex_TakeReadOrWrite((rwMutex), (isWriteMode)), RWMutex_DropReadOrWrite((rwMutex), (isWriteMode)))
#define RWMutexScopeRead(rwMutex) DeferLoop(RWMutex_TakeRead(rwMutex), RWMutex_DropRead(rwMutex))
#define RWMutexScopeWrite(rwMutex) DeferLoop(RWMutex_TakeWrite(rwMutex), RWMutex_DropWrite(rwMutex))
#define RWMutexScopeRWPromote(rwMutex) DeferLoop((RWMutex_DropRead(rwMutex), RWMutex_TakeWrite(rwMutex)), (RWMutex_DropWrite(rwMutex), RWMutex_TakeRead(rwMutex)))

#endif // BASE_THREAD_HPP
