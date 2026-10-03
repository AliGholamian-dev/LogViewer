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

internal Range1<UInt64> Math_GetSubdivisionRange(UInt64 subdivisionIndex, UInt64 divisionCount, UInt64 elementCount)
{
    const UInt64 elementCountPerDivision { elementCount / divisionCount };
    const UInt64 leftoverElementCount { elementCount - (elementCountPerDivision * divisionCount) };
    const UInt64 leftoverElementCountBeforeThisDivision { MinOf<UInt64>(subdivisionIndex, leftoverElementCount) };
    const UInt64 divisionBaseIndex { subdivisionIndex * elementCountPerDivision + leftoverElementCountBeforeThisDivision };
    const UInt64 clampedDivisionBaseIndex { MinOf<UInt64>(divisionBaseIndex, elementCount) };
    const UInt64 divisionOnePastLastIndex { clampedDivisionBaseIndex + elementCountPerDivision + ((subdivisionIndex < leftoverElementCount) ? 1 : 0) };
    const UInt64 clampedDivisionOnePastLastIndex { MinOf<UInt64>(divisionOnePastLastIndex, elementCount) };
    return Range1<UInt64>
    {
        .min = clampedDivisionBaseIndex,
        .max = clampedDivisionOnePastLastIndex
    };
}

#endif // BASE_MATH_HPP
