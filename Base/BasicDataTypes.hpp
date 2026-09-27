#ifndef BASE_BASIC_DATA_TYPES_HPP
#define BASE_BASIC_DATA_TYPES_HPP

#include <cstddef>
#include <cstdint>

using UInt8    = std::uint8_t;
using UInt16   = std::uint16_t;
using UInt32   = std::uint32_t;
using UInt64   = std::uint64_t;
using Bool8    = bool;
using Bool16   = UInt16;
using Bool32   = UInt32;
using Bool64   = UInt64;
using SInt8    = std::int8_t;
using SInt16   = std::int16_t;
using SInt32   = std::int32_t;
using SInt64   = std::int64_t;
using Float32  = float;
using Float64  = double;
using SizeType = std::size_t;
using Char     = char;

static_assert(sizeof(UInt8)    == 1, "[BasicDataTypes]: UInt8 should be 1 byte");
static_assert(sizeof(UInt16)   == 2, "[BasicDataTypes]: UInt16 should be 2 bytes");
static_assert(sizeof(UInt32)   == 4, "[BasicDataTypes]: UInt32 should be 4 bytes");
static_assert(sizeof(UInt64)   == 8, "[BasicDataTypes]: UInt64 should be 8 bytes");
static_assert(sizeof(Bool8)    == 1, "[BasicDataTypes]: Bool8 should be 1 byte");
static_assert(sizeof(Bool16)   == 2, "[BasicDataTypes]: Bool16 should be 2 bytes");
static_assert(sizeof(Bool32)   == 4, "[BasicDataTypes]: Bool32 should be 4 bytes");
static_assert(sizeof(Bool64)   == 8, "[BasicDataTypes]: Bool64 should be 8 bytes");
static_assert(sizeof(SInt8)    == 1, "[BasicDataTypes]: SInt8 should be 1 byte");
static_assert(sizeof(SInt16)   == 2, "[BasicDataTypes]: SInt16 should be 2 bytes");
static_assert(sizeof(SInt32)   == 4, "[BasicDataTypes]: SInt32 should be 4 bytes");
static_assert(sizeof(SInt64)   == 8, "[BasicDataTypes]: SInt64 should be 8 bytes");
static_assert(sizeof(Float32)  == 4, "[BasicDataTypes]: Float32 should be 4 bytes");
static_assert(sizeof(Float64)  == 8, "[BasicDataTypes]: Float64 should be 8 bytes");

#endif // BASE_BASIC_DATA_TYPES_HPP
