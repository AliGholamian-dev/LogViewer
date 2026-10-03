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
        Win32Entity* nextFree;
        struct
        {
            ThreadEntryPointFunctionType *entryPointFunction;
            void *params;
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
    CRITICAL_SECTION entityMutex;
    Arena *entityArena;
    Win32Entity* firstFreeEntity;
};

global Win32PlatformState g_win32PlatformState {};

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

internal Win32Entity *Win32_AllocateEntity(Win32EntityKind entityKind)
{
    Win32Entity *win32Entity { nullptr };
    EnterCriticalSection(&g_win32PlatformState.entityMutex);
    {
        win32Entity = g_win32PlatformState.firstFreeEntity;
        if(win32Entity != nullptr)
        {
            Assert(win32Entity->kind == Win32EntityKind::Free, "Free Win32Entity does not have Free kind as tag");
            g_win32PlatformState.firstFreeEntity = g_win32PlatformState.firstFreeEntity->nextFree;
        }
        else
        {
            win32Entity = Arena_PushType<Win32Entity>(g_win32PlatformState.entityArena);
        }
        Memory_Zero(win32Entity, sizeof(*win32Entity));
    }
    LeaveCriticalSection(&g_win32PlatformState.entityMutex);

    Assert(win32Entity != nullptr, "Could not allocate Win32Entity");
    win32Entity->kind = entityKind;
    return win32Entity;
}

internal void Win32_ReleaseEntity(Win32Entity *entity)
{
    Assert(entity != nullptr, "Null Win32Entity");
    entity->kind = Win32EntityKind::Free;
    EnterCriticalSection(&g_win32PlatformState.entityMutex);
    entity->nextFree = g_win32PlatformState.firstFreeEntity;
    g_win32PlatformState.firstFreeEntity = entity;
    LeaveCriticalSection(&g_win32PlatformState.entityMutex);
}

internal DWORD WINAPI Win32_ThreadEntryPoint(void *win32Params) noexcept
{
    Assert(win32Params != nullptr, "Null param in Win32_ThreadEntryPoint, can not get Win32Entity");
    Win32Entity *entity { static_cast<Win32Entity*>(win32Params) };
    Thread_CallThreadEntryPoint(entity->thread.entryPointFunction, entity->thread.params);
    return 0;
}

internal Thread Thread_Launch(ThreadEntryPointFunctionType *entryPointFunction, void *params)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Thread) };
    entity->thread.entryPointFunction = entryPointFunction;
    entity->thread.params = params;
    entity->thread.handle = CreateThread(nullptr, 0, Win32_ThreadEntryPoint, entity, 0, &entity->thread.id);
    return Thread
    {
        .impl = entity
    };
}

internal Bool8 Thread_Join(Thread thread, const MilliSeconds waitTimeInMilliSeconds)
{
    Win32Entity *entity { static_cast<Win32Entity*>(thread.impl) };
    Assert(entity != nullptr, "Null Win32Entity of thread kind");

    DWORD waitResult { WAIT_OBJECT_0 };
    if(entity != nullptr)
    {
        DWORD waitTime { 0 };
        if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::Representation>())
        {
            waitTime = INFINITE;
        }
        else if (waitTimeInMilliSeconds.value > 0)
        {
            waitTime = SafeCast<MilliSeconds::Representation, DWORD>(waitTimeInMilliSeconds.value);
        }
        waitResult = WaitForSingleObject(entity->thread.handle, waitTime);
        CloseHandle(entity->thread.handle);
        Win32_ReleaseEntity(entity);
    }
    return (waitResult == WAIT_OBJECT_0);
}

internal void Thread_Detach(Thread thread)
{
    Win32Entity *entity { static_cast<Win32Entity*>(thread.impl) };
    Assert(entity != nullptr, "Null Win32Entity of thread kind");

    if(entity != nullptr)
    {
        CloseHandle(entity->thread.handle);
        Win32_ReleaseEntity(entity);
    }
}

internal UInt32 Thread_GetID(void)
{
  return GetCurrentThreadId(); 
}

internal void Thread_SetName(const String8 name)
{
    TempArena scratchArena{ ThreadContext_BeginScratchArena(nullptr, 0) };

    {
        const String16 name16{ String16_CreateFromString8(scratchArena.arena, name) };
        HRESULT hr { SetThreadDescription(GetCurrentThread(), (PCWSTR)name16.str) };
        Unused(hr);
    }

    {
        String8 nameCopy { String8_Copy(scratchArena.arena, name) };
        #pragma pack(push, 8)
                struct THREADNAME_INFO
                {
                    UInt32 dwType;     // Must be 0x1000.
                    char *szName;      // Pointer to name (in user addr space).
                    UInt32 dwThreadID; // Thread ID (-1=caller thread).
                    UInt32 dwFlags;    // Reserved for future use, must be zero.
                };
        #pragma pack(pop)
        THREADNAME_INFO info {};
        info.dwType = 0x1000;
        info.szName = (char *)nameCopy.str;
        info.dwThreadID = Thread_GetID();
        info.dwFlags = 0;
        #pragma warning(push)
        #pragma warning(disable : 6320 6322)
                __try
                {
                    RaiseException(0x406D1388, 0, sizeof(info) / sizeof(void *), (const ULONG_PTR *)&info);
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                }
        #pragma warning(pop)
    }

    ThreadContext_EndScratchArena(scratchArena);
}

internal Mutex Mutex_Allocate(void)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Mutex) };
    InitializeCriticalSection(&entity->mutex);
    return Mutex
    {
        .impl = entity
    };
}

internal void Mutex_Release(Mutex mutex)
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

internal RWMutex RWMutex_Allocate(void)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::RWMutex) };
    InitializeSRWLock(&entity->rwMutex);
    return RWMutex
    {
        .impl = entity
    };
}

internal void RWMutex_Release(RWMutex rwMutex)
{
    Win32Entity *entity { static_cast<Win32Entity*>(rwMutex.impl) };
    Assert(entity != nullptr, "Null Win32Entity of rw mutex kind");
    Win32_ReleaseEntity(entity);
}

internal void RWMutex_Take(RWMutex rwMutex, const Bool8 isWriteMode)
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

internal void RWMutex_Drop(RWMutex rwMutex, const Bool8 isWritemode)
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

internal CondVar CondVar_Allocate(void)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::CondVar) };
    InitializeConditionVariable(&entity->condVar);
    return CondVar
    {
        .impl = entity  
    };
}

internal void CondVar_Release(CondVar condVar)
{
    Win32Entity *entity { static_cast<Win32Entity*>(condVar.impl) };
    Assert(entity != nullptr, "Null Win32Entity of cond var kind");
    Win32_ReleaseEntity(entity);
}

internal Bool8 CondVar_Wait(CondVar condVar, Mutex mutex, const MilliSeconds waitTimeInMilliSeconds)
{
    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::Representation>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::Representation, DWORD>(waitTimeInMilliSeconds.value);
    }
    Bool8 result { false };
    if(waitTime > 0)
    {
        Win32Entity *condVarEntity { static_cast<Win32Entity*>(condVar.impl) };
        Win32Entity *mutexEntity { static_cast<Win32Entity*>(mutex.impl) };
        Assert(condVarEntity != nullptr, "Null Win32Entity of cond var kind");
        Assert(mutexEntity != nullptr, "Null Win32Entity of mutex kind");
        result = SleepConditionVariableCS(&condVarEntity->condVar, &mutexEntity->mutex, waitTime);
    }
    return result;
}

internal Bool8 CondVar_Wait_RW(CondVar condVar, RWMutex rwMutex, const Bool8 isWriteMode, const MilliSeconds waitTimeInMilliSeconds)
{
    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::Representation>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::Representation, DWORD>(waitTimeInMilliSeconds.value);
    }
    Bool8 result { false };
    if(waitTime > 0)
    {
        Win32Entity *condVarEntity { static_cast<Win32Entity*>(condVar.impl) };
        Win32Entity *rwMutexEntity { static_cast<Win32Entity*>(rwMutex.impl) };
        Assert(condVarEntity != nullptr, "Null Win32Entity of cond var kind");
        Assert(rwMutexEntity != nullptr, "Null Win32Entity of rw mutex kind");
        result = SleepConditionVariableSRW(&condVarEntity->condVar, &rwMutexEntity->rwMutex, waitTime, isWriteMode ? 0 : CONDITION_VARIABLE_LOCKMODE_SHARED);
    }
    return result;
}

internal void CondVar_Signal(CondVar condVar)
{
    Win32Entity *entity { static_cast<Win32Entity*>(condVar.impl) };
    Assert(entity != nullptr, "Null Win32Entity of cond var kind");
    WakeConditionVariable(&entity->condVar);
}

internal void CondVar_Broadcast(CondVar condVar)
{
    Win32Entity *entity { static_cast<Win32Entity*>(condVar.impl) };
    Assert(entity != nullptr, "Null Win32Entity of cond var kind");
    WakeAllConditionVariable(&entity->condVar);
}

internal Semaphore Semaphore_Allocate(const UInt32 initialCount, const UInt32 maxCount, const String8 name)
{
    TempArena scratchArena{ ThreadContext_BeginScratchArena(nullptr, 0) };
    const String16 name16{ String16_CreateFromString8(scratchArena.arena, name) };
    HANDLE handle { CreateSemaphore(0, SafeCast<UInt32, LONG>(initialCount), SafeCast<UInt32, LONG>(maxCount), (LPCWSTR)name16.str) };
    Semaphore semaphore 
    {
        .impl = handle
    };
    ThreadContext_EndScratchArena(scratchArena);
    return semaphore;
}

internal Semaphore Semaphore_Open(const String8 name)
{
    TempArena scratchArena{ ThreadContext_BeginScratchArena(nullptr, 0) };
    const String16 name16{ String16_CreateFromString8(scratchArena.arena, name) };
    HANDLE handle { OpenSemaphore(SEMAPHORE_ALL_ACCESS, 0, (LPCWSTR)name16.str) };
    Semaphore semaphore 
    {
        .impl = handle
    };
    ThreadContext_EndScratchArena(scratchArena);
    return semaphore;
}

internal void Semaphore_Release(Semaphore semaphore)
{
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    CloseHandle(handle);
}

internal void Semaphore_Close(Semaphore semaphore)
{
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    CloseHandle(handle);
}

internal Bool8 Semaphore_Take(Semaphore semaphore, const MilliSeconds waitTimeInMilliSeconds)
{

    DWORD waitTime { 0 };
    if(waitTimeInMilliSeconds.value == GetHighestNumericLimitOf<MilliSeconds::Representation>())
    {
        waitTime = INFINITE;
    }
    else if (waitTimeInMilliSeconds.value > 0)
    {
        waitTime = SafeCast<MilliSeconds::Representation, DWORD>(waitTimeInMilliSeconds.value);
    }
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    DWORD waitResult { WaitForSingleObject(handle, waitTime) };
    return (waitResult == WAIT_OBJECT_0);
}

internal void Semaphore_DropCount(Semaphore semaphore, const UInt32 dropCount)
{
    HANDLE handle { static_cast<HANDLE>(semaphore.impl) };
    ReleaseSemaphore(handle, SafeCast<UInt32, LONG>(dropCount), 0);
}

internal Barrier Barrier_Allocate(UInt64 count)
{
    Barrier result {0};
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Barrier) };
    BOOL initWasGood { InitializeSynchronizationBarrier(&entity->synchBarrier, SafeCast<UInt64, LONG>(count), -1) };
    Unused(initWasGood);
    return Barrier
    {
        .impl = entity
    };
}

internal void Barrier_Release(Barrier barrier)
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
    g_win32PlatformState.systemInfo = SystemInfo
    {
        .logicalProcessorCount = SafeCast<DWORD, UInt64>(win32Systeminfo.dwNumberOfProcessors),
        .pageSize = SafeCast<DWORD, SizeType>(win32Systeminfo.dwPageSize),
        .largePageSize = GetLargePageMinimum(),
        .allocationGranularity = SafeCast<DWORD, SizeType>(win32Systeminfo.dwAllocationGranularity),
        .largePagesAllowed = m_largePagesAllowed && GetLargePageMinimum() > 0
    };

    InitializeCriticalSection(&g_win32PlatformState.entityMutex);
    {
        const ArenaParams entityArenaParams
        {
            .reserveSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : MB(1),
            .commitSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : KB(64),
            .optionalBackingBuffer = nullptr,
            .configFlags = SystemInfo_Get()->largePagesAllowed ? Flag_ConvertEnumToValue<ArenaConfigs>(ArenaConfigs::LargePages) : Flag_NoFlags<ArenaConfigs>()
        };
        g_win32PlatformState.entityArena = Arena_Allocate(&entityArenaParams);
    }
    g_win32PlatformState.firstFreeEntity = nullptr;

    ThreadContext *threadContext { ThreadContext_Allocate() };
    ThreadContext_Select(threadContext);
}

internal void Win32_DeInitPlatform(void)
{
    /// Note: Reserved, do not cleanup resources that are claimed by OS automatically upon exit
    NoOp();
}
