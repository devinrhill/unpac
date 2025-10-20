#include "giga/bytestream.hpp"
#include <string>
#include "brsar.h"

namespace unpac {

void Brsar::openFile(const std::string& filename) {
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

    std::uint8_t versionMax, versionMin;
    versionMax = bytestream.readScalar<std::uint8_t>();
    versionMin = bytestream.readScalar<std::uint8_t>();

    bytestream.seek(0xc);

    std::uint16_t headerSize = bytestream.readScalar<std::uint16_t>();
    std::uint16_t sectionCount = bytestream.readScalar<std::uint32_t>();

    std::uint32_t symbOffset = bytestream.readScalar<std::uint32_t>();
    std::uint32_t symbSize = bytestream.readScalar<std::uint32_t>();
    std::uint32_t infoOffset = bytestream.readScalar<std::uint32_t>();
    std::uint32_t infoSize = bytestream.readScalar<std::uint32_t>();
    std::uint32_t fileOffset = bytestream.readScalar<std::uint32_t>();
    std::uint32_t fileSize = bytestream.readScalar<std::uint32_t>();

    readSymb(bytestream, symbOffset);
    readInfo(bytestream, infoOffset);
    readFile(bytestream, fileOffset);
}

void readSymb(giga::Bytestream& bytestream, std::uint32_t symbOffset) {

}

void readInfo(giga::Bytestream& bytestream, std::uint32_t infoOffset) {

}

void readFile(giga::Bytestream& bytestream, std::uint32_t fileOffset) {

}

} // namespace unpac
