#ifndef BASE_PLATFORM_HPP
#define BASE_PLATFORM_HPP

internal void *Memory_Reserve(const SizeType sizeInBytes);
internal void *Memory_ReserveLarge(const SizeType sizeInBytes);
internal Bool8 Memory_Commit(void *ptr, const SizeType sizeInBytes);
internal Bool8 Memory_CommitLarge(void *ptr, const SizeType sizeInBytes);
internal void Memory_Decommit(void *ptr, const SizeType sizeInBytes);
internal void Memory_Release(void *ptr, const SizeType sizeInBytes);
internal void Memory_Set(void *ptr, const UInt8 value, const SizeType sizeInBytes);
internal void Memory_Zero(void *ptr, const SizeType sizeInBytes);
internal void Memory_Copy(void *destination, const void *source, const SizeType sizeInBytes);

#endif // BASE_PLATFORM_HPP
