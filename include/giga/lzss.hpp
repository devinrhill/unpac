#ifndef LZSS_H
#define LZSS_H

#define LZSS_NDEBUG 1

#include <cstddef>
#include <cstdint>

namespace giga {
namespace lzss {

typedef struct Config {
    int EI; // Number of bits for reference offset
    int EJ; // Number of bits for reference length
} Config;

constexpr int EBADTEXT = -1;
constexpr int EBADCODE = -2;
constexpr int EBADCONFIG = -3;
constexpr int EBADALLOC = -4;

int decompress(const std::uint8_t* text, // Text buffer
               std::size_t textSize,     // Text buffer size
               std::uint8_t* code,       // Code buffer
               std::size_t codeSize,     // Code buffer size
               Config config             // Reference field lengths
);

int compress(std::uint8_t* code,   // Code buffer
             std::size_t codeSize, // Code buffer size
             std::uint8_t* text,   // Text buffer
             std::size_t textSize, // Text buffer size
             Config config         // Reference field lengths
);

} // namespace lzss
} // namespace giga

#endif // LZSS_H
