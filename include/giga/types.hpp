#ifndef GIGA_TYPES_H
#define GIGA_TYPES_H

#include <cstdint>

namespace giga {

// Unsigned fixed-width integer types
typedef std::uint8_t u8;
typedef std::uint16_t u16;
typedef std::uint32_t u32;
typedef std::uint64_t u64;

// Signed fixed-width integer types
typedef std::int8_t s8;
typedef std::int16_t s16;
typedef std::int32_t s32;
typedef std::int64_t s64;

// Fixed-width floating point types
typedef float f32;
typedef double f64;

// Short-name unsigned integer types
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

// Short-name signed integer types
typedef signed char schar;
typedef signed short sshort;
typedef signed int sint;
typedef signed long slong;
typedef signed long long slonglong;

} // namespace giga

#endif // GIGA_TYPES_H
