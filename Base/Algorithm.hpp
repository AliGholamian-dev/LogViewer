#ifndef BASE_ALGORITHM_HPP
#define BASE_ALGORITHM_HPP

global constexpr UInt32 bitmask1  { 0x00000001ul };
global constexpr UInt32 bitmask2  { 0x00000003ul };
global constexpr UInt32 bitmask3  { 0x00000007ul };
global constexpr UInt32 bitmask4  { 0x0000000ful };
global constexpr UInt32 bitmask5  { 0x0000001ful };
global constexpr UInt32 bitmask6  { 0x0000003ful };
global constexpr UInt32 bitmask7  { 0x0000007ful };
global constexpr UInt32 bitmask8  { 0x000000fful };
global constexpr UInt32 bitmask9  { 0x000001fful };
global constexpr UInt32 bitmask10 { 0x000003fful };
global constexpr UInt32 bitmask11 { 0x000007fful };
global constexpr UInt32 bitmask12 { 0x00000ffful };
global constexpr UInt32 bitmask13 { 0x00001ffful };
global constexpr UInt32 bitmask14 { 0x00003ffful };
global constexpr UInt32 bitmask15 { 0x00007ffful };
global constexpr UInt32 bitmask16 { 0x0000fffful };
global constexpr UInt32 bitmask17 { 0x0001fffful };
global constexpr UInt32 bitmask18 { 0x0003fffful };
global constexpr UInt32 bitmask19 { 0x0007fffful };
global constexpr UInt32 bitmask20 { 0x000ffffful };
global constexpr UInt32 bitmask21 { 0x001ffffful };
global constexpr UInt32 bitmask22 { 0x003ffffful };
global constexpr UInt32 bitmask23 { 0x007ffffful };
global constexpr UInt32 bitmask24 { 0x00fffffful };
global constexpr UInt32 bitmask25 { 0x01fffffful };
global constexpr UInt32 bitmask26 { 0x03fffffful };
global constexpr UInt32 bitmask27 { 0x07fffffful };
global constexpr UInt32 bitmask28 { 0x0ffffffful };
global constexpr UInt32 bitmask29 { 0x1ffffffful };
global constexpr UInt32 bitmask30 { 0x3ffffffful };
global constexpr UInt32 bitmask31 { 0x7ffffffful };
global constexpr UInt32 bitmask32 { 0xfffffffful };

global constexpr UInt64 bitmask33 { 0x00000001ffffffffull };
global constexpr UInt64 bitmask34 { 0x00000003ffffffffull };
global constexpr UInt64 bitmask35 { 0x00000007ffffffffull };
global constexpr UInt64 bitmask36 { 0x0000000fffffffffull };
global constexpr UInt64 bitmask37 { 0x0000001fffffffffull };
global constexpr UInt64 bitmask38 { 0x0000003fffffffffull };
global constexpr UInt64 bitmask39 { 0x0000007fffffffffull };
global constexpr UInt64 bitmask40 { 0x000000ffffffffffull };
global constexpr UInt64 bitmask41 { 0x000001ffffffffffull };
global constexpr UInt64 bitmask42 { 0x000003ffffffffffull };
global constexpr UInt64 bitmask43 { 0x000007ffffffffffull };
global constexpr UInt64 bitmask44 { 0x00000fffffffffffull };
global constexpr UInt64 bitmask45 { 0x00001fffffffffffull };
global constexpr UInt64 bitmask46 { 0x00003fffffffffffull };
global constexpr UInt64 bitmask47 { 0x00007fffffffffffull };
global constexpr UInt64 bitmask48 { 0x0000ffffffffffffull };
global constexpr UInt64 bitmask49 { 0x0001ffffffffffffull };
global constexpr UInt64 bitmask50 { 0x0003ffffffffffffull };
global constexpr UInt64 bitmask51 { 0x0007ffffffffffffull };
global constexpr UInt64 bitmask52 { 0x000fffffffffffffull };
global constexpr UInt64 bitmask53 { 0x001fffffffffffffull };
global constexpr UInt64 bitmask54 { 0x003fffffffffffffull };
global constexpr UInt64 bitmask55 { 0x007fffffffffffffull };
global constexpr UInt64 bitmask56 { 0x00ffffffffffffffull };
global constexpr UInt64 bitmask57 { 0x01ffffffffffffffull };
global constexpr UInt64 bitmask58 { 0x03ffffffffffffffull };
global constexpr UInt64 bitmask59 { 0x07ffffffffffffffull };
global constexpr UInt64 bitmask60 { 0x0fffffffffffffffull };
global constexpr UInt64 bitmask61 { 0x1fffffffffffffffull };
global constexpr UInt64 bitmask62 { 0x3fffffffffffffffull };
global constexpr UInt64 bitmask63 { 0x7fffffffffffffffull };
global constexpr UInt64 bitmask64 { 0xffffffffffffffffull };

global constexpr UInt32 bit1  { (1ul <<  0) };
global constexpr UInt32 bit2  { (1ul <<  1) };
global constexpr UInt32 bit3  { (1ul <<  2) };
global constexpr UInt32 bit4  { (1ul <<  3) };
global constexpr UInt32 bit5  { (1ul <<  4) };
global constexpr UInt32 bit6  { (1ul <<  5) };
global constexpr UInt32 bit7  { (1ul <<  6) };
global constexpr UInt32 bit8  { (1ul <<  7) };
global constexpr UInt32 bit9  { (1ul <<  8) };
global constexpr UInt32 bit10 { (1ul <<  9) };
global constexpr UInt32 bit11 { (1ul << 10) };
global constexpr UInt32 bit12 { (1ul << 11) };
global constexpr UInt32 bit13 { (1ul << 12) };
global constexpr UInt32 bit14 { (1ul << 13) };
global constexpr UInt32 bit15 { (1ul << 14) };
global constexpr UInt32 bit16 { (1ul << 15) };
global constexpr UInt32 bit17 { (1ul << 16) };
global constexpr UInt32 bit18 { (1ul << 17) };
global constexpr UInt32 bit19 { (1ul << 18) };
global constexpr UInt32 bit20 { (1ul << 19) };
global constexpr UInt32 bit21 { (1ul << 20) };
global constexpr UInt32 bit22 { (1ul << 21) };
global constexpr UInt32 bit23 { (1ul << 22) };
global constexpr UInt32 bit24 { (1ul << 23) };
global constexpr UInt32 bit25 { (1ul << 24) };
global constexpr UInt32 bit26 { (1ul << 25) };
global constexpr UInt32 bit27 { (1ul << 26) };
global constexpr UInt32 bit28 { (1ul << 27) };
global constexpr UInt32 bit29 { (1ul << 28) };
global constexpr UInt32 bit30 { (1ul << 29) };
global constexpr UInt32 bit31 { (1ul << 30) };
global constexpr UInt32 bit32 { (1ul << 31) };

global constexpr UInt64 bit33 { (1ull << 32) };
global constexpr UInt64 bit34 { (1ull << 33) };
global constexpr UInt64 bit35 { (1ull << 34) };
global constexpr UInt64 bit36 { (1ull << 35) };
global constexpr UInt64 bit37 { (1ull << 36) };
global constexpr UInt64 bit38 { (1ull << 37) };
global constexpr UInt64 bit39 { (1ull << 38) };
global constexpr UInt64 bit40 { (1ull << 39) };
global constexpr UInt64 bit41 { (1ull << 40) };
global constexpr UInt64 bit42 { (1ull << 41) };
global constexpr UInt64 bit43 { (1ull << 42) };
global constexpr UInt64 bit44 { (1ull << 43) };
global constexpr UInt64 bit45 { (1ull << 44) };
global constexpr UInt64 bit46 { (1ull << 45) };
global constexpr UInt64 bit47 { (1ull << 46) };
global constexpr UInt64 bit48 { (1ull << 47) };
global constexpr UInt64 bit49 { (1ull << 48) };
global constexpr UInt64 bit50 { (1ull << 49) };
global constexpr UInt64 bit51 { (1ull << 50) };
global constexpr UInt64 bit52 { (1ull << 51) };
global constexpr UInt64 bit53 { (1ull << 52) };
global constexpr UInt64 bit54 { (1ull << 53) };
global constexpr UInt64 bit55 { (1ull << 54) };
global constexpr UInt64 bit56 { (1ull << 55) };
global constexpr UInt64 bit57 { (1ull << 56) };
global constexpr UInt64 bit58 { (1ull << 57) };
global constexpr UInt64 bit59 { (1ull << 58) };
global constexpr UInt64 bit60 { (1ull << 59) };
global constexpr UInt64 bit61 { (1ull << 60) };
global constexpr UInt64 bit62 { (1ull << 61) };
global constexpr UInt64 bit63 { (1ull << 62) };
global constexpr UInt64 bit64 { (1ull << 63) };

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

template <typename T, SizeType N>
constexpr std::size_t ArrayCount(const T (&)[N]) noexcept {
    return N;
}

#endif // BASE_ALGORITHM_HPP
