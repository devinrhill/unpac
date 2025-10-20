#ifndef GIGA_ENDIANNESS_H
#define GIGA_ENDIANNESS_H

#include <cstddef>
#include "types.hpp"

namespace giga {

enum class Endianness {
	Unknown = 0,
	Little = 1234,
	Big = 4321
};

#if (defined(__x86_64__) || defined(__i386__))
constexpr Endianness NATIVE_ENDIANNESS = Endianness::Little;
#else
constexpr Endianness NATIVE_ENDIANNESS = Endianness::Big;
#endif

inline u16 byteswap16(u16 num) {
    return ( (((num) >> 8) & 0x00ff) | (((num) << 8) & 0xff00) );
}

inline u32 byteswap32(u32 num) {
    return ( (((num) >> 24) & 0x000000ff) | (((num) >>  8) & 0x0000ff00) |
             (((num) <<  8) & 0x00ff0000) | (((num) << 24) & 0xff000000) );
}

inline u64 byteswap64(u64 num) {
    return ( (((num) >> 56) & 0x00000000000000ff) | (((num) >> 40) & 0x000000000000ff00) |
             (((num) >> 24) & 0x0000000000ff0000) | (((num) >>  8) & 0x00000000ff000000) |
             (((num) <<  8) & 0x000000ff00000000) | (((num) << 24) & 0x0000ff0000000000) |
             (((num) << 40) & 0x00ff000000000000) | (((num) << 56) & 0xff00000000000000) );
}

const char* getEndiannessName(Endianness endianness = Endianness::Unknown);
template<typename T>
T byteswapEndianness(T num, Endianness endianness = NATIVE_ENDIANNESS, int size = -1) {
    std::size_t tmpSize;
    if(size == -1) {
        tmpSize = sizeof(T);
    } else {
        tmpSize = size;
    }

	if(endianness != NATIVE_ENDIANNESS) {
		switch(tmpSize) {
            case 1: {
                goto do_nothing;
            }
    /*
            case 2: {
                return static_cast<T>(byteswap16(num));
            } case 4: {
                return static_cast<T>(byteswap32(num));
            } case 8: {
                return static_cast<T>(byteswap64(num));
            }
    */
            default: {
                T tmpNum = num;

                for(std::size_t i = 0; i < tmpSize; i++) {
                    reinterpret_cast<char*>(&tmpNum)[i] = reinterpret_cast<char*>(&num)[tmpSize - i - 1];
                }

                return tmpNum;
            }
		}
	}

do_nothing:
	return num;
}

} // namespace giga

#endif // GIGA_ENDIANNESS_H
