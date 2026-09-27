internal String8 String8_Create(UInt8* str, const SizeType length) 
{
    return String8
    {
        .str = str,
        .length = length
    };
}

internal String8 String8_Copy(Arena* arena, const String8 stringToCopyFrom)
{
    String8 copiedString
    {
        .str = Arena_PushArray<UInt8>(arena, stringToCopyFrom.length + 1),
        .length = stringToCopyFrom.length
    };
    Memory_Copy(copiedString.str, stringToCopyFrom.str, stringToCopyFrom.length);
    copiedString.str[stringToCopyFrom.length] = 0;
    return copiedString;
}
