enum class Win32EntityKind
{
    Free,
    Thread
};

struct Win32Entity
{
    Win32EntityKind kind;
    union
    {
        struct
        {
            Win32Entity* nextFree;
        } freeData;
        struct
        {
            ThreadEntryPointFunctionType *entryPointFunction;
            void *params;
            HANDLE handle;
            DWORD id;
        } thread;
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
            g_win32PlatformState.firstFreeEntity = g_win32PlatformState.firstFreeEntity->freeData.nextFree;
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
    entity->freeData.nextFree = g_win32PlatformState.firstFreeEntity;
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
    entity->thread.handle = CreateThread(NULL, 0, Win32_ThreadEntryPoint, entity, 0, &entity->thread.id);
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

internal void Win32_InitPlatform(void)
{    
    Bool8 largePagesAllowed { false };
    {
        HANDLE token {};
        if(OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token))
        {
            LUID luid {};
            if(LookupPrivilegeValue(0, SE_LOCK_MEMORY_NAME, &luid))
            {
                TOKEN_PRIVILEGES priv {};
                priv.PrivilegeCount           = 1;
                priv.Privileges[0].Luid       = luid;
                priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                largePagesAllowed = !!AdjustTokenPrivileges(token, 0, &priv, sizeof(priv), NULL, NULL);
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
        .largePagesAllowed = largePagesAllowed && GetLargePageMinimum() > 0
    };

    InitializeCriticalSection(&g_win32PlatformState.entityMutex);
    {
        const ArenaParams entityArenaParams
        {
            .reserveSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : MB(1),
            .commitSizeInBytes = SystemInfo_Get()->largePagesAllowed ? SystemInfo_Get()->largePageSize : MB(64),
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
