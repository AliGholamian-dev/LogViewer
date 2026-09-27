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
    HINSTANCE hInstance;
    SystemInfo systemInfo;
    CRITICAL_SECTION entityMutex;
    Arena *entityArena;
    Win32Entity* firstFreeEntity;
};

global Win32PlatformState g_win32PlatformState {};

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
    std::memset(ptr, value, sizeInBytes);
}

internal void Memory_Zero(void *ptr, const SizeType sizeInBytes)
{
    Memory_Set(ptr, 0, sizeInBytes);
}

internal void Memory_Copy(void *destination, const void *source, const SizeType sizeInBytes)
{
    std::memcpy(destination, source, sizeInBytes);
}

internal SystemInfo *SystemInfo_Get(void)
{
    return &g_win32PlatformState.systemInfo;
}

internal Win32Entity *Win32_AllocateEntity(Win32EntityKind entityKind)
{
    Win32Entity *win32Entity { nullptr };
    EnterCriticalSection(&g_win32PlatformState.entityMutex);
    {
        win32Entity = g_win32PlatformState.firstFreeEntity;
        if(win32Entity != nullptr)
        {
            // assert(win32Entity->kind == Win32EntityKind::Free);
            g_win32PlatformState.firstFreeEntity = g_win32PlatformState.firstFreeEntity->freeData.nextFree;
        }
        else
        {
            win32Entity = Arena_PushType<Win32Entity>(g_win32PlatformState.entityArena);
        }
        Memory_Zero(win32Entity, sizeof(*win32Entity));
    }
    LeaveCriticalSection(&g_win32PlatformState.entityMutex);
    win32Entity->kind = entityKind;
    return win32Entity;
}

internal void Win32_ReleaseEntity(Win32Entity *entity)
{
    entity->kind = Win32EntityKind::Free;
    EnterCriticalSection(&g_win32PlatformState.entityMutex);
    entity->freeData.nextFree = g_win32PlatformState.firstFreeEntity;
    g_win32PlatformState.firstFreeEntity = entity;
    LeaveCriticalSection(&g_win32PlatformState.entityMutex);
}

internal DWORD Win32_ThreadEntryPoint(void *win32Params)
{
    Win32Entity *entity { static_cast<Win32Entity*>(win32Params) };
    EntryPoint_SupplementThread(entity->thread.entryPointFunction, entity->thread.params);
    return 0;
}

internal Thread Thread_Launch(ThreadEntryPointFunctionType *entryPointFunction, void *params)
{
    Win32Entity *entity { Win32_AllocateEntity(Win32EntityKind::Thread) };
    entity->thread.entryPointFunction = entryPointFunction;
    entity->thread.params = params;
    entity->thread.handle = CreateThread(0, 0, Win32_ThreadEntryPoint, entity, 0, &entity->thread.id);
    return Thread
    {
        .impl = entity
    };
}

internal Bool8 Thread_Join(Thread thread, const MilliSeconds waitTimeInMilliSeconds)
{
    Win32Entity *entity { static_cast<Win32Entity*>(thread.impl) };
    DWORD waitResult { WAIT_OBJECT_0 };
    if(entity != nullptr)
    {
        DWORD waitTime { 0 };
        if(waitTimeInMilliSeconds.value == GetHighestNumericLimit<MilliSeconds::Representation>())
        {
            waitTime = INFINITE;
        }
        else if (waitTimeInMilliSeconds.value > 0)
        {
            waitTime = static_cast<DWORD>(waitTimeInMilliSeconds.value);
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
    if(entity != nullptr)
    {
        CloseHandle(entity->thread.handle);
        Win32_ReleaseEntity(entity);
    }
}

internal void Win32_InitPlatform(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{    
    Unused(hPrevInstance, pCmdLine, nCmdShow);

    g_win32PlatformState.hInstance = hInstance;

    SYSTEM_INFO win32Systeminfo {};
    GetSystemInfo(&win32Systeminfo);
    g_win32PlatformState.systemInfo = SystemInfo
    {
        .numberOfLogicalProcessors = static_cast<UInt64>(win32Systeminfo.dwNumberOfProcessors),
        .pageSize = static_cast<SizeType>(win32Systeminfo.dwPageSize),
        .largePageSize = GetLargePageMinimum(),
        .allocationGranularity = static_cast<SizeType>(win32Systeminfo.dwAllocationGranularity)
    };

    InitializeCriticalSection(&g_win32PlatformState.entityMutex);
    {
        const ArenaParams entityArenaParams
        {
            .reserveSizeInBytes = MB(64),
            .commitSizeInBytes = KB(64),
            .optionalBackingBuffer = nullptr,
            .configFlags = Flag_NoFlags<FlagType<ArenaConfigs>>()
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

internal HINSTANCE Win32_GetModule(void)
{
    return g_win32PlatformState.hInstance;
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Win32_InitPlatform(hInstance, hPrevInstance, pCmdLine, nCmdShow);
    /// TODO: Maybe pass parsed command line arguments
    int runResult { EntryPoint_MainThread() };
    Win32_DeInitPlatform();
    return runResult;
}
