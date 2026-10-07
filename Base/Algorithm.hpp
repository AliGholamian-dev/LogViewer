#ifndef BASE_ALGORITHM_HPP
#define BASE_ALGORITHM_HPP

template <typename T>
concept UnsignedInteger = std::unsigned_integral<T>;

template <typename T, SizeType N>
concept ValidBitIndex = UnsignedInteger<T> && (N >= 1) && (N <= sizeof(T) * 8);

template <typename T, SizeType N>
requires ValidBitIndex<T, N>
internal consteval T Bit()
{
    return T{ 1 } << (N - 1);
}

template <typename T, SizeType N>
requires ValidBitIndex<T, N>
internal consteval T BitMask()
{
    if constexpr (N == sizeof(T) * 8)
    {
        return ~T{ 0 };
    }
    else
    {
        return (T{ 1 } << N) - T{ 1 };
    }
}

template<typename T>
internal consteval T GetHighestNumericLimitOf()
{
    return std::numeric_limits<T>::max();
}

template<typename T>
internal consteval T GetLowestNumericLimitOf()
{
    return std::numeric_limits<T>::lowest();
}

template<typename T>
internal constexpr T MinOf(const T leftValue, const T rightValue)
{
    return (leftValue < rightValue) ? leftValue : rightValue;
}

template<typename T>
internal constexpr T MaxOf(const T leftValue, const T rightValue)
{
    return (leftValue > rightValue) ? leftValue : rightValue;
}

template<typename T>
internal constexpr T ClampTop(const T value, const T maxValue)
{
    return MinOf(value, maxValue);
}

template<typename T>
internal constexpr T ClampBottom(const T value, const T minValue)
{
    return MaxOf(value, minValue);
}

template<typename T>
internal constexpr T ClampRange(const T value, const T minValue, const T maxValue)
{
    return ClampBottom(ClampTop(value, maxValue), minValue);
}

template<typename T>
internal constexpr T AlignUpPow2(const T value, const T alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

template<typename T>
internal constexpr T AlignDownPow2(const T value, const T alignment)
{
    return value & ~(alignment - 1);
}

template<typename T>
internal constexpr T AlignPaddingPow2(const T value, const T alignment)
{
    return (0 - value) & (alignment - 1);
}

internal constexpr SizeType KB(SizeType value)
{
    return value << 10;
}

internal constexpr SizeType MB(SizeType value)
{
    return value << 20;
}

internal constexpr SizeType GB(SizeType value)
{
    return value << 30;
}

template<typename FromType, typename ToType>
internal constexpr ToType SafeCast(const FromType fromValue)
{
    if constexpr (std::is_integral_v<FromType> && std::is_integral_v<ToType>)
    {
        if constexpr (std::is_signed_v<FromType> && std::is_unsigned_v<ToType>)
        {
            // Signed -> unsigned.
            using UnsignedFrom = std::make_unsigned_t<FromType>;
            Assert(fromValue >= 0, "Cast failed, out of range (negative value -> unsigned)");
            Assert(static_cast<UnsignedFrom>(fromValue) <= GetHighestNumericLimitOf<ToType>(), "Cast failed, out of range (value > Highest Numeric Limit)");
        }
        else if constexpr (std::is_unsigned_v<FromType> && std::is_signed_v<ToType>)
        {
            // Unsigned -> signed.
            using UnsignedTo = std::make_unsigned_t<ToType>;
            Assert(fromValue <= static_cast<UnsignedTo>(GetHighestNumericLimitOf<ToType>()), "Cast failed, out of range (value > Highest Numeric Limit)");
        }
        else
        {
            // Same signedness.
            Assert(fromValue >= GetLowestNumericLimitOf<ToType>(), "Cast failed, out of range (value < Lowest Numeric Limit)");
            Assert(fromValue <= GetHighestNumericLimitOf<ToType>(), "Cast failed, out of range (value > Highest Numeric Limit)");
        }
    }
    else if constexpr (std::is_floating_point_v<FromType> && std::is_integral_v<ToType>)
    {
        // Assert(std::isfinite(fromValue), "Cast failed, floating point value is not finite");
        Assert(fromValue >= GetLowestNumericLimitOf<ToType>(), "Cast failed, out of range (value < Lowest Numeric Limit)");
        Assert(fromValue <= GetHighestNumericLimitOf<ToType>(), "Cast failed, out of range (value > Highest Numeric Limit)");
    }
    else if constexpr (std::is_integral_v<FromType> && std::is_floating_point_v<ToType>)
    {
        Assert(static_cast<Float64>(fromValue) >= GetLowestNumericLimitOf<ToType>(), "Cast failed, out of range (value < Lowest Numeric Limit)");
        Assert(static_cast<Float64>(fromValue) <= GetHighestNumericLimitOf<ToType>(), "Cast failed, out of range (value > Highest Numeric Limit)");
    }
    else if constexpr (std::is_floating_point_v<FromType> && std::is_floating_point_v<ToType>)
    {
        // Assert(std::isfinite(fromValue), "Cast failed, floating point value is not finite");
        Assert(fromValue >= GetLowestNumericLimitOf<ToType>(), "Cast failed, out of range (value < Lowest Numeric Limit)");
        Assert(fromValue <= GetHighestNumericLimitOf<ToType>(), "Cast failed, out of range (value > Highest Numeric Limit)");
    }
    return static_cast<ToType>(fromValue);
}

template <typename T, SizeType N>
internal consteval SizeType FixedSizeArrayCount(const T (&)[N]) 
{
    return N;
}

#endif // BASE_ALGORITHM_HPP
