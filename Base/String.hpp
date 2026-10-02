#ifndef BASE_STRING_HPP
#define BASE_STRING_HPP

struct String8
{
    UInt8* str;
    SizeType length;
};

internal String8 String8_Create(UInt8* str, const SizeType length);
internal String8 String8_Copy(Arena* arena, const String8 stringToCopyFrom);

template <SizeType N>
constexpr const String8 String8_CreateFromLiteral(const char (&literal)[N])
{
    char* nonConstLiteral { const_cast<char*>(literal) };
    return String8_Create(static_cast<UInt8*>(nonConstLiteral), N - 1);
}

struct String16
{
    UInt16* str;
    SizeType length;
};

internal String16 String16_Create(UInt16* str, const SizeType length);
internal String16 String16_CreateFromString8(Arena *arena, String8 string8);

struct UnicodeDecode
{
    UInt32 inc;
    UInt32 codepoint;
};

internal UnicodeDecode UTF8_Decode(UInt8 *str, SizeType max);
internal UnicodeDecode UTF16_Decode(UInt16 *str, SizeType max);
internal UInt32 UTF8_Encode(UInt8 *str, UInt32 codepoint);
internal UInt32 UTF16_Encode(UInt16 *str, UInt32 codepoint);

#endif // BASE_STRING_HPP
