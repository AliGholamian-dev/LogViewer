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
    FlagType<ArenaConfigs> configFlags;
    SizeType bytesReserved;
    SizeType bytesCommitted;
    SizeType position;
};

internal Arena *Arena_Allocate(const ArenaParams* params);
internal void Arena_Release(Arena *arena);
internal SizeType Arena_GetPosition(const Arena *arena);
internal void *Arena_Push(Arena *arena, const SizeType sizeInBytes, const SizeType alignmentInBytes, const Bool8 memoryShouldBeZeroed);
internal void Arena_PopToPosition(Arena *arena, const SizeType positionToPopTo);
internal void Arena_PopByAmount(Arena *arena, const SizeType amountToPop);
internal void Arena_Clear(Arena *arena);

////////////////////////////////
// Arena Array Helpers

template <typename T>
T* Arena_PushArrayAligned(Arena *arena, const SizeType arrayCapacity, const SizeType alignmentInBytes)
{
    return static_cast<T*>(Arena_Push(arena, sizeof(T) * arrayCapacity, alignmentInBytes, false));
}

template <typename T>
T* Arena_PushArrayAlignedAndZero(Arena *arena, const SizeType arrayCapacity, const SizeType alignmentInBytes)
{
    return static_cast<T*>(Arena_Push(arena, sizeof(T) * arrayCapacity, alignmentInBytes, true));
}

template <typename T>
T* Arena_PushArray(Arena *arena, const SizeType arrayCapacity)
{
    return Arena_PushArrayAligned<T>(arena, arrayCapacity, MaxOf<SizeType>(alignof(T), 8));
}

template <typename T>
T* Arena_PushArrayAndZero(Arena *arena, const SizeType arrayCapacity)
{
    return Arena_PushArrayAlignedAndZero<T>(arena, arrayCapacity, MaxOf<SizeType>(alignof(T), 8));
}

////////////////////////////////
// Arena Type Helpers

template <typename T>
T* Arena_PushTypeAligned(Arena *arena, const SizeType alignmentInBytes)
{
    return Arena_PushArrayAligned<T>(arena, 1, alignmentInBytes);
}

template <typename T>
T* Arena_PushTypeAlignedAndZero(Arena *arena, const SizeType alignmentInBytes)
{
    return Arena_PushArrayAlignedAndZero<T>(arena, 1, alignmentInBytes);
}

template <typename T>
T* Arena_PushType(Arena *arena)
{
    return Arena_PushTypeAligned<T>(arena, MaxOf<SizeType>(alignof(T), 8));
}

template <typename T>
T* Arena_PushTypeAndZero(Arena *arena)
{
    return Arena_PushTypeAlignedAndZero<T>(arena, MaxOf<SizeType>(alignof(T), 8));
}

#endif // BASE_ARENA_HPP
