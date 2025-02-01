#include <giga/bytestream.h>
#include <string>
#include "brres.h"

namespace unpac {

void Brres::openFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.openFile(filename);

    char magic[4];
    bytestream.read(reinterpret_cast<std::uint8_t*>(magic), 4);

    std::uint8_t bom[2];
    bytestream.read(bom, 2);
    if(std::equal(bom, bom + 2, "\xfe\xff")) {
        bytestream.setEndianness(giga::endian::Endianness::Big);
    } else if(std::equal(bom, bom + 2, "\xff\xfe")) {
        bytestream.setEndianness(giga::endian::Endianness::Little);
    } else {
        return;
    }

    bytestream.seek(0x8);

    std::uint32_t fileSize = bytestream.readScalar<std::uint32_t>();
}

} // namespace brres
