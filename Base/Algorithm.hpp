#ifndef BASE_ALGORITHM_HPP
#define BASE_ALGORITHM_HPP

template<typename T>
consteval T GetHighestNumericLimitOf()
{
    return std::numeric_limits<T>::max();
}

template<typename T>
consteval T GetLowestNumericLimitOf()
{
    return std::numeric_limits<T>::lowest();
}

template<typename T>
constexpr T MinOf(const T leftValue, const T rightValue)
{
    return (leftValue < rightValue) ? leftValue : rightValue;
}

template<typename T>
constexpr T MaxOf(const T leftValue, const T rightValue)
{
    return (leftValue > rightValue) ? leftValue : rightValue;
}

template<typename T>
constexpr T ClampTop(const T value, const T maxValue)
{
    return MinOf(value, maxValue);
}

template<typename T>
constexpr T ClampBottom(const T value, const T minValue)
{
    return MaxOf(value, minValue);
}

template<typename T>
constexpr T ClampRange(const T value, const T minValue, const T maxValue)
{
    return ClampBottom(ClampTop(value, maxValue), minValue);
}

template<typename T>
constexpr T AlignUpPow2(const T value, const T alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

template<typename T>
constexpr T AlignDownPow2(const T value, const T alignment)
{
    return value & ~(alignment - 1);
}

template<typename T>
constexpr T AlignPaddingPow2(const T value, const T alignment)
{
    return (0 - value) & (alignment - 1);
}

constexpr SizeType KB(SizeType value)
{
    return value << 10;
}

constexpr SizeType MB(SizeType value)
{
    return value << 20;
}

constexpr SizeType GB(SizeType value)
{
    return value << 30;
}

template<typename FromType, typename ToType>
constexpr ToType SafeCast(const FromType fromValue)
{
    Assert(fromValue >= GetLowestNumericLimitOf<ToType>(), "Cast failed, out of range (value < Lowest Numeric Limit)");
    Assert(fromValue <= GetHighestNumericLimitOf<ToType>(), "Cast failed, out of range (value > Highest Numeric Limit)");
    return static_cast<ToType>(fromValue);
}

#endif // BASE_ALGORITHM_HPP
