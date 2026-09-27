#ifndef BASE_FLAGS_HPP
#define BASE_FLAGS_HPP

template <typename EnumType>
concept FlagEnum = std::is_enum_v<EnumType>;

template <FlagEnum T>
using FlagType = std::underlying_type_t<T>;

template <FlagEnum EnumType>
constexpr FlagType<EnumType> Flag_ConvertEnumToValue(const EnumType enumValue)
{
    return static_cast<FlagType<EnumType>>(enumValue);
}

template <FlagEnum EnumType>
constexpr FlagType<EnumType> Flag_SetBit(const FlagType<EnumType> flags, const EnumType enumValue)
{
    return (flags | Flag_ConvertEnumToValue(enumValue));
}

template <FlagEnum EnumType>
constexpr FlagType<EnumType> Flag_ResetBit(const FlagType<EnumType> flags, const EnumType enumValue)
{
    return (flags & (~Flag_ConvertEnumToValue(enumValue)));
}

template <typename T>
constexpr T Flag_ClearAllBits(const T flags)
{
    Unused(flags);
    return static_cast<T>(0);
}

template <typename T>
consteval T Flag_NoFlags()
{
    return static_cast<T>(0);
}

template <FlagEnum EnumType>
constexpr Bool8 Flag_CheckBitIsSet(const FlagType<EnumType> flags, const EnumType enumValue)
{
    const FlagType<EnumType> mask { Flag_ConvertEnumToValue(enumValue) };
    return (flags & mask) == mask;
}

#endif // BASE_FLAGS_HPP
