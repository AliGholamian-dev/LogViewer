#ifndef BASE_ARENA_HPP
#define BASE_ARENA_HPP

enum class ArenaConfigs : UInt8
{
    LargePages = (1 << 0),
};

struct ArenaParams
{
    SizeType reserveSizeInBytes;
    SizeType commitSizeInBytes;
    void *optionalBackingBuffer;
    FlagType<ArenaConfigs> configFlags;
};

struct Arena
{
    SizeType reserveSizeInBytes;
    SizeType commitSizeInBytes;
    SizeType bytesReserved;
    SizeType bytesCommitted;
    SizeType position;
    FlagType<ArenaConfigs> configFlags;
};

struct TempArena
{
    Arena *arena;
    SizeType position;
};

/// TODO: Add a setter for the Default, so that the user can change based on options or system constraints
internal ArenaParams Arena_CreateDefaultArenaParams(void);
internal Arena *Arena_Allocate(const ArenaParams* params);
internal void Arena_Release(Arena *arena);
internal SizeType Arena_GetPosition(const Arena *arena);
internal void *Arena_Push(Arena *arena, const SizeType sizeInBytes, const SizeType alignmentInBytes, const Bool8 memoryShouldBeZeroed);
internal void Arena_PopToPosition(Arena *arena, const SizeType positionToPopTo);
internal void Arena_PopByAmount(Arena *arena, const SizeType amountToPop);
internal void Arena_Clear(Arena *arena);

internal TempArena TempArena_Begin(Arena *arena);
internal void TempArena_End(TempArena tempArena);

template <typename T>
T* Arena_PushArrayAligned(Arena *arena, const SizeType arrayCapacity, const SizeType alignmentInBytes)
{
    Assert(arena != nullptr, "Null arena");

    return static_cast<T*>(Arena_Push(arena, sizeof(T) * arrayCapacity, alignmentInBytes, false));
}

template <typename T>
T* Arena_PushArrayAlignedAndZero(Arena *arena, const SizeType arrayCapacity, const SizeType alignmentInBytes)
{
    Assert(arena != nullptr, "Null arena");

    return static_cast<T*>(Arena_Push(arena, sizeof(T) * arrayCapacity, alignmentInBytes, true));
}

template <typename T>
T* Arena_PushArray(Arena *arena, const SizeType arrayCapacity)
{
    Assert(arena != nullptr, "Null arena");

    return Arena_PushArrayAligned<T>(arena, arrayCapacity, MaxOf<SizeType>(alignof(T), 8));
}

template <typename T>
T* Arena_PushArrayAndZero(Arena *arena, const SizeType arrayCapacity)
{
    Assert(arena != nullptr, "Null arena");

    return Arena_PushArrayAlignedAndZero<T>(arena, arrayCapacity, MaxOf<SizeType>(alignof(T), 8));
}

template <typename T>
T* Arena_PushTypeAligned(Arena *arena, const SizeType alignmentInBytes)
{
    Assert(arena != nullptr, "Null arena");

    return Arena_PushArrayAligned<T>(arena, 1, alignmentInBytes);
}

template <typename T>
T* Arena_PushTypeAlignedAndZero(Arena *arena, const SizeType alignmentInBytes)
{
    Assert(arena != nullptr, "Null arena");

    return Arena_PushArrayAlignedAndZero<T>(arena, 1, alignmentInBytes);
}

template <typename T>
T* Arena_PushType(Arena *arena)
{
    Assert(arena != nullptr, "Null arena");

    return Arena_PushTypeAligned<T>(arena, MaxOf<SizeType>(alignof(T), 8));
}

template <typename T>
T* Arena_PushTypeAndZero(Arena *arena)
{
    Assert(arena != nullptr, "Null arena");

    return Arena_PushTypeAlignedAndZero<T>(arena, MaxOf<SizeType>(alignof(T), 8));
}

#define TempArenaScope(tempArenaName, referenceArena) for (TempArena tempArenaName = TempArena_Begin(referenceArena), *Glue(_TempArenaOnce_, __LINE__) = &tempArenaName; Glue(_TempArenaOnce_, __LINE__) != nullptr; TempArena_End(tempArenaName), Glue(_TempArenaOnce_, __LINE__) = nullptr)

#endif // BASE_ARENA_HPP
