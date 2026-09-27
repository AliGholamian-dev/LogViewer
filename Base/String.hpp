#ifndef BASE_STRING_HPP
#define BASE_STRING_HPP

struct String8
{
    UInt8* str;
    SizeType length;
};

internal String8 String8_Create(UInt8* str, const SizeType length);

template <SizeType N>
constexpr const String8 String8_CreateFromLiteral(const Char (&literal)[N])
{
    Char* nonConstLiteral { const_cast<Char*>(literal) };
    return String8_Create(static_cast<UInt8*>(nonConstLiteral), N - 1);
}

internal String8 String8_Copy(Arena* arena, const String8 stringToCopyFrom);

#endif // BASE_STRING_HPP
