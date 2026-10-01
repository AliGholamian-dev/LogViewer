global constexpr SizeType g_arenaHeaderSize { sizeof(Arena) };

internal Arena *Arena_Allocate(const ArenaParams* params)
{
    Assert(params != nullptr, "Null arena paramters");

    SizeType reserveSizeInBytes { params->reserveSizeInBytes };
    SizeType commitSizeInBytes { params->commitSizeInBytes };

    void *base = params->optionalBackingBuffer;
    if (base == nullptr)
    {
        if (Flag_CheckBitIsSet(params->configFlags, ArenaConfigs::LargePages))
        {
            reserveSizeInBytes = AlignUpPow2<SizeType>(reserveSizeInBytes, SystemInfo_Get()->largePageSize);
            commitSizeInBytes = AlignUpPow2<SizeType>(commitSizeInBytes, SystemInfo_Get()->largePageSize);
        }
        else
        {
            reserveSizeInBytes = AlignUpPow2<SizeType>(reserveSizeInBytes, SystemInfo_Get()->pageSize);
            commitSizeInBytes = AlignUpPow2<SizeType>(commitSizeInBytes, SystemInfo_Get()->pageSize);
        }

        if (Flag_CheckBitIsSet(params->configFlags, ArenaConfigs::LargePages))
        {
            base = Memory_ReserveLarge(reserveSizeInBytes);
            Memory_CommitLarge(base, commitSizeInBytes);
        }
        else
        {
            base = Memory_Reserve(reserveSizeInBytes);
            Memory_Commit(base, commitSizeInBytes);
        }
    }

    /// TODO: Turn into platform based popup show or shell report
    Assert(base != nullptr, "Could not reserve memory for arena");
    
    Arena *arena = reinterpret_cast<Arena*>(base);
    arena->reserveSizeInBytes = params->reserveSizeInBytes;
    arena->commitSizeInBytes = params->commitSizeInBytes;
    arena->bytesReserved = reserveSizeInBytes;
    arena->bytesCommitted = commitSizeInBytes;
    arena->position = g_arenaHeaderSize;
    arena->configFlags = params->configFlags;

    return arena;
}

internal void Arena_Release(Arena *arena)
{
    Assert(arena != nullptr, "Null arena");
    Memory_Release(arena, arena->reserveSizeInBytes);
}

internal SizeType Arena_GetPosition(const Arena *arena)
{
    Assert(arena != nullptr, "Null arena");
    return arena->position;
}

internal void *Arena_Push(Arena *arena, const SizeType sizeInBytes, const SizeType alignmentInBytes, const Bool8 memoryShouldBeZeroed)
{
    Assert(arena != nullptr, "Null arena");

    const SizeType prePushPosition { AlignUpPow2<SizeType>(arena->position, alignmentInBytes) };
    const SizeType postPushPosition { prePushPosition + sizeInBytes };

    SizeType size_to_zero { 0 };
    if(memoryShouldBeZeroed)
    {
        size_to_zero = MinOf<SizeType>(arena->bytesCommitted, postPushPosition) - prePushPosition;
    }

    if(postPushPosition > arena->bytesCommitted)
    {
        SizeType postPushCommitedSizeInBytesAligned { postPushPosition + arena->commitSizeInBytes - 1 };
        postPushCommitedSizeInBytesAligned -= postPushCommitedSizeInBytesAligned % arena->commitSizeInBytes;
        const SizeType postPushCommitedSizeInBytesClamped { ClampTop<SizeType>(postPushCommitedSizeInBytesAligned, arena->bytesReserved) };
        const SizeType commitSizeInBytes { postPushCommitedSizeInBytesClamped - arena->bytesCommitted };
        UInt8 *commitPtr = reinterpret_cast<UInt8*>(arena) + arena->bytesCommitted;
        if(Flag_CheckBitIsSet(arena->configFlags, ArenaConfigs::LargePages))
        {
            Memory_CommitLarge(commitPtr, commitSizeInBytes);
        }
        else
        {
            Memory_Commit(commitPtr, commitSizeInBytes);
        }
        arena->bytesCommitted = postPushCommitedSizeInBytesClamped;
    }

    void *result { nullptr };
    if(arena->bytesCommitted >= postPushPosition)
    {
        result = reinterpret_cast<UInt8*>(arena) + prePushPosition;
        arena->position = postPushPosition;
        Memory_Zero(result, size_to_zero);
    }

    Assert(result != nullptr, "Could not push to arena");

    return result;
}

internal void Arena_PopToPosition(Arena *arena, const SizeType positionToPopTo)
{
    Assert(arena != nullptr, "Null arena");

    arena->position = ClampBottom(positionToPopTo, g_arenaHeaderSize);
}

internal void Arena_PopByAmount(Arena *arena, const SizeType amountToPop)
{
    Assert(arena != nullptr, "Null arena");

    const SizeType currentPosition { Arena_GetPosition(arena) };
    const SizeType maximumAmountPossibleToPop { MinOf<SizeType>(amountToPop, currentPosition) };
    const SizeType newPosition { currentPosition - maximumAmountPossibleToPop };
    Arena_PopToPosition(arena, newPosition);
}

internal void Arena_Clear(Arena *arena)
{
    Assert(arena != nullptr, "Null arena");

    Arena_PopToPosition(arena, 0);
}
