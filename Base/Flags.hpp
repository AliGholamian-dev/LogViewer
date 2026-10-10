#ifndef BASE_FLAGS_HPP
#define BASE_FLAGS_HPP

template <typename EnumType>
concept FlagEnum = std::is_enum_v<EnumType>;

template <FlagEnum T>
using FlagType = std::underlying_type_t<T>;

template <FlagEnum EnumType>
internal constexpr FlagType<EnumType> Flag_ConvertEnumToValue(const EnumType enumValue)
{
    return static_cast<FlagType<EnumType>>(enumValue);
}

template <FlagEnum EnumType, typename... EnumTypes>
requires (std::same_as<EnumType, EnumTypes> && ...)
internal constexpr FlagType<EnumType> Flag_SetBits(const FlagType<EnumType> flags, const EnumType firstEnumValue, const EnumTypes... restEnumValues)
{
    return static_cast<FlagType<EnumType>>(flags | (Flag_ConvertEnumToValue(firstEnumValue) | ... | Flag_ConvertEnumToValue(restEnumValues)));
}

template <FlagEnum EnumType, typename... EnumTypes>
requires (std::same_as<EnumType, EnumTypes> && ...)
internal constexpr FlagType<EnumType> Flag_ResetBits(const FlagType<EnumType> flags, const EnumType firstEnumValue, const EnumTypes... restEnumValues)
{
    return static_cast<FlagType<EnumType>>(flags & ~(Flag_ConvertEnumToValue(firstEnumValue) | ... | Flag_ConvertEnumToValue(restEnumValues)));
}

template <FlagEnum EnumType>
internal constexpr FlagType<EnumType> Flag_ClearAllBits(const FlagType<EnumType> flags)
{
    Unused(flags);
    return static_cast<FlagType<EnumType>>(0);
}

template <FlagEnum EnumType>
internal consteval FlagType<EnumType> Flag_NoFlags()
{
    return static_cast<FlagType<EnumType>>(0);
}

template <FlagEnum EnumType, typename... EnumTypes>
requires (std::same_as<EnumType, EnumTypes> && ...)
internal constexpr FlagType<EnumType> Flag_FromBits(const EnumType firstEnumValue, const EnumTypes... restEnumValues)
{
    return static_cast<FlagType<EnumType>>((Flag_ConvertEnumToValue(firstEnumValue) | ... | Flag_ConvertEnumToValue(restEnumValues)));
}

template <FlagEnum EnumType, typename... EnumTypes>
requires (std::same_as<EnumType, EnumTypes> && ...)
internal constexpr Bool8 Flag_CheckBitsAreSet(const FlagType<EnumType> flags, const EnumType firstEnumValue, const EnumTypes... restEnumValues)
{
    const FlagType<EnumType> mask { (Flag_ConvertEnumToValue(firstEnumValue) | ... | Flag_ConvertEnumToValue(restEnumValues)) };

    return (flags & mask) == mask;
}

#endif // BASE_FLAGS_HPP
