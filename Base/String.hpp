#ifndef BASE_STRING_HPP
#define BASE_STRING_HPP

struct String8
{
    UInt8* str;
    SizeType length;
};

struct String16
{
    UInt16* str;
    SizeType length;
};

struct UnicodeDecode
{
    UInt32 inc;
    UInt32 codepoint;
};

internal UnicodeDecode UTF8_Decode(UInt8 *str, SizeType max);
internal UnicodeDecode UTF16_Decode(UInt16 *str, SizeType max);
internal UInt32 UTF8_Encode(UInt8 *str, UInt32 codepoint);
internal UInt32 UTF16_Encode(UInt16 *str, UInt32 codepoint);
internal String8 String8_Create(UInt8* str, const SizeType length);
template <SizeType N>
constexpr const String8 String8_CreateFromLiteral(const char (&literal)[N])
{
    char* nonConstLiteral { const_cast<char*>(literal) };
    return String8_Create(reinterpret_cast<UInt8*>(nonConstLiteral), N - 1);
}
/// TODO: make these safe and templated instead of variadic args 
internal String8 String8_CreateFromFormattedStringAndVariadicArguments(Arena* arena, const char* format, va_list args);
internal String8 String8_CreateFromFormattedString(Arena* arena, const char* format, ...);
internal String8 String8_Copy(Arena* arena, const String8 stringToCopyFrom);

internal String16 String16_Create(UInt16* str, const SizeType length);
internal String16 String16_CreateFromString8(Arena *arena, const String8 string8);

#endif // BASE_STRING_HPP
