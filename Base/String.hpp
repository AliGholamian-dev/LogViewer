#ifndef BASE_STRING_HPP
#define BASE_STRING_HPP

struct String8
{
    UInt8* str;
    SizeType length;
};

internal String8 String8_Create(UInt8* str, const SizeType length)
{
    return String8
    {
        .str = str,
        .length = length
    };
}

template <SizeType N>
constexpr const String8 String8_CreateFromLiteral(const char (&literal)[N])
{
    char* nonConstLiteral { const_cast<char*>(literal) };
    return String8_Create(static_cast<UInt8*>(nonConstLiteral), N - 1);
}

internal String8 String8_Copy(Arena* arena, const String8 stringToCopyFrom)
{
    Assert(arena != nullptr, "Null arena for string copy");

    String8 copiedString
    {
        .str = Arena_PushArray<UInt8>(arena, stringToCopyFrom.length + 1),
        .length = stringToCopyFrom.length
    };
    Memory_Copy(copiedString.str, stringToCopyFrom.str, stringToCopyFrom.length);
    copiedString.str[stringToCopyFrom.length] = 0;
    return copiedString;
}

#endif // BASE_STRING_HPP
