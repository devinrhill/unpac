#include "giga/bytestream.hpp"
#include <string>
#include "u8_arc.h"

namespace unpac {

void U8arc::openFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.setEndianness(giga::endian::Endianness::Big);
    bytestream.openFile(filename);

    char magic[4];
    bytestream.read(reinterpret_cast<std::uint8_t*>(magic), 4);
    if(!std::equal(magic, magic + 4, "\x55\xAA\x38\x2d")) { // U.8-
        return;
    }

    std::uint32_t firstNodeOffset = bytestream.readScalar<std::uint32_t>();
    std::uint32_t tablesSize = bytestream.readScalar<std::uint32_t>();
    std::uint32_t dataOffset = bytestream.readScalar<std::uint32_t>();

    bytestream.seek(firstNodeOffset);

}

} // namespace unpac
