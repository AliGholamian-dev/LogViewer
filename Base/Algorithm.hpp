#ifndef BASE_ALGORITHM_HPP
#define BASE_ALGORITHM_HPP

template<typename T>
consteval T GetHighestNumericLimit()
{
    return std::numeric_limits<T>::max();
}

template<typename T>
consteval T GetLowestNumericLimit()
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

constexpr UInt64 KB(UInt64 value)
{
    return value << 10;
}

constexpr UInt64 MB(UInt64 value)
{
    return value << 20;
}

constexpr UInt64 GB(UInt64 value)
{
    return value << 30;
}

#endif // BASE_ALGORITHM_HPP
