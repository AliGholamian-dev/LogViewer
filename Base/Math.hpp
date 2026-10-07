#ifndef BASE_MATH_HPP
#define BASE_MATH_HPP

template <typename T>
struct Vec2
{
    T x;
    T y;
};

template <typename T>
struct Position2D
{
    T x;
    T y;
};

template <typename T>
struct Size2D
{
    T width;
    T height;
};

template <typename T>
struct Range1
{
    T min;
    T max;
};

template <UnsignedInteger T>
internal constexpr Range1<T> GetSubdivisionIndexRange(T subdivisionIndex, T divisionCount, T elementCount)
{
    const T elementCountPerDivision { elementCount / divisionCount };
    const T leftoverElementCount { elementCount - (elementCountPerDivision * divisionCount) };
    const T leftoverElementCountBeforeThisDivision { MinOf<T>(subdivisionIndex, leftoverElementCount) };
    const T divisionBaseIndex { subdivisionIndex * elementCountPerDivision + leftoverElementCountBeforeThisDivision };
    const T clampedDivisionBaseIndex { MinOf<T>(divisionBaseIndex, elementCount) };
    const T divisionOnePastLastIndex { clampedDivisionBaseIndex + elementCountPerDivision + ((subdivisionIndex < leftoverElementCount) ? 1 : 0) };
    const T clampedDivisionOnePastLastIndex { MinOf<T>(divisionOnePastLastIndex, elementCount) };
    return Range1<T>
    {
        .min = clampedDivisionBaseIndex,
        .max = clampedDivisionOnePastLastIndex
    };
}

template <typename T>
internal constexpr T Abs(T value)
{
    return value < T{ 0 } ? -value : value;
}

#endif // BASE_MATH_HPP
