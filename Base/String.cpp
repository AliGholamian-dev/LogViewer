read_only global UInt8 g_utf8Class[32] =
{
  1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0,2,2,2,2,3,3,4,5,
};

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

internal String16 String16_Create(UInt16* str, const SizeType length)
{
    return String16
    {
        .str = str,
        .length = length
    };
}

internal String16 String16_CreateFromString8(Arena *arena, String8 string8)
{
    Assert(arena != nullptr, "Null arena");

    UInt16 *str { nullptr };
    SizeType length { 0 };
    if(string8.length > 0)
    {
        const SizeType string16Capacity { string8.length * 2 };
        str = Arena_PushArray<UInt16>(arena, string16Capacity + 1);
        UInt8 *ptr { string8.str };
        UInt8 *onePastLast = ptr + string8.length;
        UnicodeDecode consume;
        for(;ptr < onePastLast; ptr += consume.inc)
        {
            consume = UTF8_Decode(ptr, SafeCast<PtrDiffType, SizeType>(onePastLast - ptr));
            length += UTF16_Encode(str + length, consume.codepoint);
        }
        str[length] = 0;
        Arena_PopByAmount(arena, (string16Capacity - length)*2);
    }
    return String16_Create(str, length);
}

internal UnicodeDecode UTF8_Decode(UInt8 *str, SizeType max)
{
    UnicodeDecode result 
    { 
        .inc = 1, 
        .codepoint = GetHighestNumericLimitOf<UInt32>() 
    };
    UInt8 byte = str[0];
    UInt8 byte_class = g_utf8Class[byte >> 3];
    switch (byte_class)
    {
        case 1:
        {
            result.codepoint = byte;
        } break;
        case 2:
        {
            if (1 < max)
            {
                UInt8 cont_byte = str[1];
                if (g_utf8Class[cont_byte >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask5) << 6;
                    result.codepoint |= (cont_byte & bitmask6);
                    result.inc = 2;
                }
            }
        } break;
        case 3:
        {
            if (2 < max)
            {
                UInt8 cont_byte[2] = {str[1], str[2]};
                if (g_utf8Class[cont_byte[0] >> 3] == 0 &&
                    g_utf8Class[cont_byte[1] >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask4) << 12;
                    result.codepoint |= ((cont_byte[0] & bitmask6) << 6);
                    result.codepoint |= (cont_byte[1] & bitmask6);
                    result.inc = 3;
                }
            }
        } break;
        case 4:
        {
            if (3 < max)
            {
                UInt8 cont_byte[3] = {str[1], str[2], str[3]};
                if (g_utf8Class[cont_byte[0] >> 3] == 0 &&
                    g_utf8Class[cont_byte[1] >> 3] == 0 &&
                    g_utf8Class[cont_byte[2] >> 3] == 0)
                {
                    result.codepoint = (byte & bitmask3) << 18;
                    result.codepoint |= ((cont_byte[0] & bitmask6) << 12);
                    result.codepoint |= ((cont_byte[1] & bitmask6) << 6);
                    result.codepoint |= (cont_byte[2] & bitmask6);
                    result.inc = 4;
                }
            }
        } break;
        default:
        {
        } break;
    }
  return result;
}

internal UnicodeDecode UTF16_Decode(UInt16 *str, SizeType max)
{
    UnicodeDecode result 
    { 
        .inc = 1, 
        .codepoint = GetHighestNumericLimitOf<UInt32>() 
    };
    result.codepoint = str[0];
    result.inc = 1;
    if (max > 1 && 0xD800 <= str[0] && str[0] < 0xDC00 && 0xDC00 <= str[1] && str[1] < 0xE000)
    {
        result.codepoint = SafeCast<SInt32, UInt32>(((str[0] - 0xD800) << 10) | ((str[1] - 0xDC00) + 0x10000));
        result.inc = 2;
    }
    return result;
}

internal UInt32 UTF8_Encode(UInt8 *str, UInt32 codepoint)
{
    UInt32 inc = 0;
    if (codepoint <= 0x7F)
    {
        str[0] = SafeCast<UInt32, UInt8>(codepoint);
        inc = 1;
    }
    else if (codepoint <= 0x7FF)
    {
        str[0] = (bitmask2 << 6) | ((codepoint >> 6) & bitmask5);
        str[1] = bit8 | (codepoint & bitmask6);
        inc = 2;
    }
    else if (codepoint <= 0xFFFF)
    {
        str[0] = (bitmask3 << 5) | ((codepoint >> 12) & bitmask4);
        str[1] = bit8 | ((codepoint >> 6) & bitmask6);
        str[2] = bit8 | (codepoint & bitmask6);
        inc = 3;
    }
    else if (codepoint <= 0x10FFFF)
    {
        str[0] = (bitmask4 << 4) | ((codepoint >> 18) & bitmask3);
        str[1] = bit8 | ((codepoint >> 12) & bitmask6);
        str[2] = bit8 | ((codepoint >> 6) & bitmask6);
        str[3] = bit8 | (codepoint & bitmask6);
        inc = 4;
    }
    else
    {
        str[0] = '?';
        inc = 1;
    }
    return inc;
}

internal UInt32 UTF16_Encode(UInt16 *str, UInt32 codepoint)
{
    UInt32 inc = 1;
    if (codepoint == GetHighestNumericLimitOf<UInt32>())
    {
        str[0] = SafeCast<char, UInt16>('?');
    }
    else if (codepoint < 0x10000)
    {
        str[0] = SafeCast<UInt32, UInt16>(codepoint);
    }
    else
    {
        UInt32 v = codepoint - 0x10000;
        str[0] = SafeCast<UInt32, UInt16>(0xD800 + (v >> 10));
        str[1] = SafeCast<UInt32, UInt16>(0xDC00 + (v & bitmask10));
        inc = 2;
    }
    return inc;
}
