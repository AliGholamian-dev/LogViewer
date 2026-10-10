enum class Win32EntityKind
{
    Free,
    Thread,
    Mutex,
    RWMutex,
    CondVar,
    Barrier
};

struct Win32Entity
{
    Win32EntityKind kind;
    union
    {
        Win32Entity* next;
        struct
        {
            ThreadEntryPointFunctionType *entryPointFunction;
            void *params;
            ArenaParams scratchArenaParams;
            HANDLE handle;
            DWORD id;
        } thread;
        CRITICAL_SECTION mutex;
        SRWLOCK rwMutex;
        CONDITION_VARIABLE condVar;
        SYNCHRONIZATION_BARRIER synchBarrier;
    };
};

struct Win32PlatformState
{
    SystemInfo systemInfo;
    SInt64 microsecondResolution;
    CRITICAL_SECTION entityMutex;
    Arena *entityArena;
    Win32Entity* firstFreeEntity;
};

global Win32PlatformState g_win32PlatformState {};

template<>
Bool8 Atomic_Evaluate<Bool8>(const volatile Bool8* value)
{
    return static_cast<Bool8>(__iso_volatile_load8(reinterpret_cast<const volatile char*>(value)));
}

template<>
Bool8 Atomic_EvaluateAndAssign<Bool8>(volatile Bool8* value, Bool8 newValue)
{
    return static_cast<Bool8>(_InterlockedExchange8(reinterpret_cast<volatile char*>(value), static_cast<char>(newValue)));
}

template<>
Bool32 Atomic_Evaluate<Bool32>(const volatile Bool32* value)
{
    return static_cast<Bool32>(__iso_volatile_load32(reinterpret_cast<const volatile int*>(value)));
}

template<>
Bool32 Atomic_EvaluateAndAssign<Bool32>(volatile Bool32* value, Bool32 newValue)
{
    return static_cast<Bool32>(_InterlockedExchange(reinterpret_cast<volatile long*>(value), static_cast<long>(newValue)));
}

internal SystemInfo *SystemInfo_Get(void)
{
    return &g_win32PlatformState.systemInfo;
}

internal void *Memory_Reserve(const SizeType sizeInBytes)
{
    return VirtualAlloc(0, sizeInBytes, MEM_RESERVE, PAGE_READWRITE);
}

internal void *Memory_ReserveLarge(const SizeType sizeInBytes)
{
    return VirtualAlloc(0, sizeInBytes, MEM_RESERVE|MEM_COMMIT|MEM_LARGE_PAGES, PAGE_READWRITE);
}

internal Bool8 Memory_Commit(void *ptr, const SizeType sizeInBytes)
{
    return (VirtualAlloc(ptr, sizeInBytes, MEM_COMMIT, PAGE_READWRITE) != nullptr);
}

internal Bool8 Memory_CommitLarge(void *ptr, const SizeType sizeInBytes)
{
    Unused(ptr, sizeInBytes);
    return true;
}

internal void Memory_Decommit(void *ptr, const SizeType sizeInBytes)
{
    VirtualFree(ptr, sizeInBytes, MEM_DECOMMIT);
}

internal void Memory_Release(void *ptr, const SizeType sizeInBytes)
{
    Unused(sizeInBytes);
    VirtualFree(ptr, 0, MEM_RELEASE);
}

internal void Memory_Set(void *ptr, const UInt8 value, const SizeType sizeInBytes)
{
    memset(ptr, value, sizeInBytes);
}

internal void Memory_Zero(void *ptr, const SizeType sizeInBytes)
{
    Memory_Set(ptr, 0, sizeInBytes);
}

internal void Memory_Copy(void *destination, const void *source, const SizeType sizeInBytes)
{
    memcpy(destination, source, sizeInBytes);
}

template<>
TimestampClock::TimePointType GetNowTime<TimestampClock>()
{
    SInt64 result { 0 };
    LARGE_INTEGER largeIntCounter;
    if(QueryPerformanceCounter(&largeIntCounter))
    {
        result = (largeIntCounter.QuadPart * 1000000) / g_win32PlatformState.microsecondResolution;
    }
    return TimestampClock::TimePointType
    { 
        .since = MicroSeconds
        { 
            .value = result 
        }
    };
}

internal Win32Entity *Win32_AllocateEntity(Win32EntityKind entityKind)
{
    Win32Entity *win32Entity { nullptr };
    EnterCriticalSection(&g_win32PlatformState.entityMutex);
    {
        win32Entity = g_win32PlatformState.firstFreeEntity;
        if(win32Entity != nullptr)
        {
            Assert(win32Entity->kind == Win32EntityKind::Free, "Free Win32Entity does not have Free kind as tag");
            SLL_StackPop(g_win32PlatformState.firstFreeEntity);
        }
        else
        {
            win32Entity = Arena_PushType<Win32Entity>(g_win32PlatformState.entityArena);
        }
        Assert(win32Entity != nullptr, "Could not allocate Win32Entity");
        Memory_Zero(win32Entity, sizeof(*win32Entity));
        win32Entity->kind = entityKind;
    }
    LeaveCriticalSection(&g_win32PlatformState.entityMutex);
    return win32Entity;
}

internal void Win32_ReleaseEntity(Win32Entity *entity)
{
    Assert(entity != nullptr, "Null Win32Entity");
    EnterCriticalSection(&g_win32PlatformState.entityMutex);
    {
        entity->kind = Win32EntityKind::Free;
        SLL_StackPush(g_win32PlatformState.firstFreeEntity, entity);
    }
    LeaveCriticalSection(&g_win32PlatformState.entityMutex);
}

internal DWORD WINAPI Win32_ThreadEntryPoint(void *win32Params) noexcept
{
    Assert(win32Params != nullptr, "Null param in Win32_ThreadEntryPoint, can not get Win32Entity");
    Win32Entity *entity { static_cast<Win32Entity*>(win32Params) };
    Thread_AcquireThreadContexAndCallEntryPoint(entity->thread.entryPointFunction, entity->thread.params, &entity->thread.scratchArenaParams);
    return 0;
}

internal Thread Thread_Launch(ThreadEntryPointFunctionType *entryPointFunction, void *params, const ArenaParams scratchArenaParams)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Thread) };
    entity->thread.entryPointFunction = entryPointFunction;
    entity->thread.params = params;
    entity->thread.scratchArenaParams = scratchArenaParams;
    entity->thread.handle = CreateThread(nullptr, 0, Win32_ThreadEntryPoint, entity, 0, &entity->thread.id);
    return Thread
    {
        .impl = entity
    };
}

internal Bool8 Thread_Join(Thread thread, const MilliSeconds waitTimeInMilliSeconds)
{
    Assert(waitTimeInMilliSeconds.value >= 0, "Negative wait time");
    Win32Entity *entity { static_cast<Win32Entity*>(thread.impl) };
    Assert(entity != nullptr, "Null Win32Entity of thread kind");
    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::RepresentationType>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::RepresentationType, DWORD>(waitTimeInMilliSeconds.value);
    }
    DWORD waitResult { WaitForSingleObject(entity->thread.handle, waitTime) };
    CloseHandle(entity->thread.handle);
    Win32_ReleaseEntity(entity);
    return (waitResult == WAIT_OBJECT_0);
}

internal void Thread_Detach(Thread thread)
{
    Win32Entity *entity { static_cast<Win32Entity*>(thread.impl) };
    Assert(entity != nullptr, "Null Win32Entity of thread kind");
    CloseHandle(entity->thread.handle);
    Win32_ReleaseEntity(entity);
}

internal UInt32 Thread_GetID(void)
{
  return GetCurrentThreadId(); 
}

internal void Thread_SetName(const String8 name)
{
    ScratchArenaScope(scratchArena, nullptr, 0)
    {
        const String16 name16{ String16_CreateFromString8(scratchArena.arena, name) };
        HRESULT hr { SetThreadDescription(GetCurrentThread(), reinterpret_cast<PCWSTR>(name16.str)) };
        Unused(hr);
    }
}

internal Mutex Mutex_Create(void)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Mutex) };
    InitializeCriticalSection(&entity->mutex);
    return Mutex
    {
        .impl = entity
    };
}

internal void Mutex_Destory(Mutex mutex)
{
    Win32Entity *entity { static_cast<Win32Entity*>(mutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of mutex kind");
    DeleteCriticalSection(&entity->mutex);
    Win32_ReleaseEntity(entity);
}

internal void Mutex_Take(Mutex mutex)
{
    Win32Entity *entity { static_cast<Win32Entity*>(mutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of mutex kind");
    EnterCriticalSection(&entity->mutex);
}

internal void Mutex_Drop(Mutex mutex)
{
    Win32Entity *entity { static_cast<Win32Entity*>(mutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of mutex kind");
    LeaveCriticalSection(&entity->mutex);
}

internal RWMutex RWMutex_Create(void)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::RWMutex) };
    InitializeSRWLock(&entity->rwMutex);
    return RWMutex
    {
        .impl = entity
    };
}

internal void RWMutex_Destory(RWMutex rwMutex)
{
    Win32Entity *entity { static_cast<Win32Entity*>(rwMutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of rw mutex kind");
    Win32_ReleaseEntity(entity);
}

internal void RWMutex_TakeReadOrWrite(RWMutex rwMutex, const Bool8 isWriteMode)
{
    Win32Entity *entity{ static_cast<Win32Entity *>(rwMutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of rw mutex kind");
    if (isWriteMode)
    {
        AcquireSRWLockExclusive(&entity->rwMutex);
    }
    else
    {
        AcquireSRWLockShared(&entity->rwMutex);
    }
}

internal void RWMutex_DropReadOrWrite(RWMutex rwMutex, const Bool8 isWritemode)
{
    Win32Entity *entity{ static_cast<Win32Entity *>(rwMutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of rw mutex kind");
    if (isWritemode)
    {
        ReleaseSRWLockExclusive(&entity->rwMutex);
    }
    else
    {
        ReleaseSRWLockShared(&entity->rwMutex);
    }
}

internal CondVar CondVar_Create(void)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::CondVar) };
    InitializeConditionVariable(&entity->condVar);
    return CondVar
    {
        .impl = entity  
    };
}

internal void CondVar_Destory(CondVar condVar)
{
    Win32Entity *entity { static_cast<Win32Entity*>(condVar.impl) };
    Assert(entity != nullptr, "Null Win32Entity of cond var kind");
    Win32_ReleaseEntity(entity);
}

internal Bool8 CondVar_WaitFor(CondVar condVar, Mutex mutex, const MilliSeconds waitTimeInMilliSeconds)
{
    Assert(waitTimeInMilliSeconds.value >= 0, "Negative wait time");
    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::RepresentationType>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::RepresentationType, DWORD>(waitTimeInMilliSeconds.value);
    }
    Win32Entity *condVarEntity { static_cast<Win32Entity*>(condVar.impl) };
    Win32Entity *mutexEntity { static_cast<Win32Entity*>(mutex.impl) };
    Assert(condVarEntity != nullptr, "Null Win32Entity of cond var kind");
    Assert(mutexEntity != nullptr, "Null Win32Entity of mutex kind");
    return SleepConditionVariableCS(&condVarEntity->condVar, &mutexEntity->mutex, waitTime);
}

internal Bool8 CondVar_WaitRWFor(CondVar condVar, RWMutex rwMutex, const Bool8 isWriteMode, const MilliSeconds waitTimeInMilliSeconds)
{
    Assert(waitTimeInMilliSeconds.value >= 0, "Negative wait time");
    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::RepresentationType>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::RepresentationType, DWORD>(waitTimeInMilliSeconds.value);
    }
    Win32Entity *condVarEntity { static_cast<Win32Entity*>(condVar.impl) };
    Win32Entity *rwMutexEntity { static_cast<Win32Entity*>(rwMutex.impl) };
    Assert(condVarEntity != nullptr, "Null Win32Entity of cond var kind");
    Assert(rwMutexEntity != nullptr, "Null Win32Entity of rw mutex kind");
    return SleepConditionVariableSRW(&condVarEntity->condVar, &rwMutexEntity->rwMutex, waitTime, isWriteMode ? 0 : CONDITION_VARIABLE_LOCKMODE_SHARED);;
}

internal void CondVar_NotifyOne(CondVar condVar)
{
    Win32Entity *entity { static_cast<Win32Entity*>(condVar.impl) };
    Assert(entity != nullptr, "Null Win32Entity of cond var kind");
    WakeConditionVariable(&entity->condVar);
}

internal void CondVar_NotifyAll(CondVar condVar)
{
    Win32Entity *entity { static_cast<Win32Entity*>(condVar.impl) };
    Assert(entity != nullptr, "Null Win32Entity of cond var kind");
    WakeAllConditionVariable(&entity->condVar);
}

internal Semaphore Semaphore_Create(const SizeType initialCount, const SizeType maxCount, const String8 name)
{
    HANDLE handle {};
    ScratchArenaScope(scratchArena, nullptr, 0)
    {
        const String16 name16{ String16_CreateFromString8(scratchArena.arena, name) };
        handle = CreateSemaphore(0, SafeCast<SizeType, LONG>(initialCount), SafeCast<SizeType, LONG>(maxCount), reinterpret_cast<PCWSTR>(name16.str));

    }
    return Semaphore  
    {
        .impl = handle
    };
}

internal void Semaphore_Destory(Semaphore semaphore)
{
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    CloseHandle(handle);
}

internal Semaphore Semaphore_Open(const String8 name)
{
    HANDLE handle {};
    ScratchArenaScope(scratchArena, nullptr, 0)
    {
        const String16 name16{ String16_CreateFromString8(scratchArena.arena, name) };
        handle = OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, reinterpret_cast<PCWSTR>(name16.str));
    }
    return Semaphore 
    {
        .impl = handle
    };;
}

internal void Semaphore_Close(Semaphore semaphore)
{
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    CloseHandle(handle);
}

internal Bool8 Semaphore_Take(Semaphore semaphore, const MilliSeconds waitTimeInMilliSeconds)
{
    Assert(waitTimeInMilliSeconds.value >= 0, "Negative wait time");
    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::RepresentationType>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::RepresentationType, DWORD>(waitTimeInMilliSeconds.value);
    }
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    DWORD waitResult { WaitForSingleObject(handle, waitTime) };
    return (waitResult == WAIT_OBJECT_0);
}

internal void Semaphore_DropBy(Semaphore semaphore, const SizeType dropCount)
{
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    ReleaseSemaphore(handle, SafeCast<SizeType, LONG>(dropCount), 0);
}

internal Barrier Barrier_Create(const SizeType count)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Barrier) };
    BOOL initWasGood { InitializeSynchronizationBarrier(&entity->synchBarrier, SafeCast<SizeType, LONG>(count), -1) };
    Unused(initWasGood);
    return Barrier
    {
        .impl = entity
    };
}

internal void Barrier_Destory(Barrier barrier)
{
    Win32Entity *entity { static_cast<Win32Entity*>(barrier.impl) };
    Assert(entity != nullptr, "Null Win32Entity of barrier kind");
    DeleteSynchronizationBarrier(&entity->synchBarrier);
    Win32_ReleaseEntity(entity);
}

internal void Barrier_Wait(Barrier barrier)
{
    Win32Entity *entity { static_cast<Win32Entity*>(barrier.impl) };
    Assert(entity != nullptr, "Null Win32Entity of barrier kind");
    EnterSynchronizationBarrier(&entity->synchBarrier, 0);
}

internal void Win32_InitPlatform(void)
{  
    /// Sysyem Info
    {
        Bool8 m_largePagesAllowed { false };
        {
            HANDLE token {};
            if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
            {
                LUID luid {};
                SetLastError(ERROR_SUCCESS);
                if(LookupPrivilegeValue(0, SE_LOCK_MEMORY_NAME, &luid))
                {
                    TOKEN_PRIVILEGES priv {};
                    priv.PrivilegeCount           = 1;
                    priv.Privileges[0].Luid       = luid;
                    priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                    if (AdjustTokenPrivileges(token, FALSE, &priv, sizeof(priv), nullptr, nullptr))
                    { 
                        m_largePagesAllowed = (GetLastError() != ERROR_NOT_ALL_ASSIGNED);
                    }
                }
                CloseHandle(token);
            }
        }
        SYSTEM_INFO win32Systeminfo {};
        GetSystemInfo(&win32Systeminfo);
        {
            g_win32PlatformState.microsecondResolution  = 1;
            LARGE_INTEGER largeIntResolution;
            if(QueryPerformanceFrequency(&largeIntResolution))
            {
                g_win32PlatformState.microsecondResolution = largeIntResolution.QuadPart;
            }
        }
        {
            g_win32PlatformState.systemInfo = SystemInfo
            {
                .logicalProcessorCount = SafeCast<DWORD, SizeType>(win32Systeminfo.dwNumberOfProcessors),
                .pageSize = SafeCast<DWORD, SizeType>(win32Systeminfo.dwPageSize),
                .largePageSize = GetLargePageMinimum(),
                .allocationGranularity = SafeCast<DWORD, SizeType>(win32Systeminfo.dwAllocationGranularity),
                .largePagesAllowed = m_largePagesAllowed && GetLargePageMinimum() > 0
            };
        }
    }


    /// Main thread context
    {
        const ArenaParams mainThreadScratchArenaParams { Arena_CreateDefaultArenaParams() };
        ThreadContext *threadContext { ThreadContext_Allocate(&mainThreadScratchArenaParams) };
        ThreadContext_Select(threadContext);
    }


    /// Win32 entity 
    {
        InitializeCriticalSection(&g_win32PlatformState.entityMutex);
        {
            const ArenaParams entityArenaParams { Arena_CreateDefaultArenaParams() };
            g_win32PlatformState.entityArena = Arena_Allocate(&entityArenaParams);
        }
        g_win32PlatformState.firstFreeEntity = nullptr;
    }
}

internal void Win32_DeInitPlatform(void)
{
    NoOp();
}
