#include <algorithm>
#include <cstring>
#include <giga/bytestream.h>
#include <giga/endianness.h>
#include <giga/lzss.h>
#include <string>
#include "arcv.h"

namespace unpac {

void Arcv::openFile(const std::string& filename, Version version) {
    giga::Bytestream bytestream;
    bytestream.setEndianness(giga::endian::Endianness::Little);
    bytestream.openFile(filename);

    char magic[4];
    bytestream.read(reinterpret_cast<std::uint8_t*>(magic), 4);
    switch(version) {
        case Version::NMBC:
            if(!std::equal(magic, magic + 4, "ARCV")) {
                readNmbcUncompArcv(bytestream);
            }

            break;
        case Version::NMR:
            if(std::equal(magic, magic + 4, "SSZL")) {
                readNmrCompArcv(bytestream);
                readNmrUncompArcv(bytestream);
            } else if(std::equal(magic, magic + 4, "VCRA")) {
                readNmrUncompArcv(bytestream);
            } else {
                return;
            }

            break;
    }
}

void Arcv::readNmbcUncompArcv(giga::Bytestream& bytestream) {
    std::uint32_t memberCount = bytestream.readScalar<std::uint32_t>();
    std::uint32_t uncompFileSize = bytestream.readScalar<std::uint32_t>();
    std::uint32_t compFileSize = bytestream.readScalar<std::uint32_t>();
    std::uint32_t checksum = bytestream.readScalar<std::uint32_t>();

    bytestream.seek(0x40);

    std::uint32_t tmpMemberOffset;
    std::uint32_t tmpMemberSize;
    char tmpMemberName[0x30];
    std::uint32_t tmpUnknown;
    for(std::uint32_t i = 0; i < memberCount; i++) {
        tmpMemberOffset = bytestream.readScalar<std::uint32_t>();
        tmpMemberSize = bytestream.readScalar<std::uint32_t>();
        bytestream.read(reinterpret_cast<std::uint8_t*>(tmpMemberName), 0x30);
        tmpUnknown = bytestream.readScalar<std::uint32_t>();
        bytestream.seek(0x4, giga::Bytestream::SeekMode::Cur);

        if(tmpMemberSize > 0) {

        }
    }
}

void Arcv::readNmrUncompArcv(giga::Bytestream& bytestream) {
    std::uint32_t memberCount = bytestream.readScalar<std::uint32_t>();
    if(memberCount < 1) { // Empty archive
        return;
    }

    std::uint32_t size = bytestream.readScalar<std::uint32_t>();
    if(size < 0x40) { // Empty archive
        return;
    }
    bytestream.seek(0x10); // Skip
    std::uint32_t badChecksum = bytestream.readScalar<std::uint32_t>(); // Not sure

    bytestream.seek(0x40);

    std::uint32_t tmpMemberOffset, tmpMemberSize;
    char tmpMemberName[0x38];
    giga::Bytestream tmpMemberBytestream;
    std::size_t tmpSeekbackPos;
    std::vector<std::uint8_t> tmpMemberBuf;
    for(std::uint32_t i = 0; i < memberCount; i++) {
        // Reset member metadata variables
        tmpMemberOffset = 0;
        tmpMemberSize = 0;
        std::memset(tmpMemberName, 0, 0x38);

        // Read member metadata
        tmpMemberOffset = bytestream.readScalar<std::uint32_t>();
        tmpMemberSize = bytestream.readScalar<std::uint32_t>();
        bytestream.read(reinterpret_cast<std::uint8_t*>(tmpMemberName), 0x38);

        // Save our position at the next member metadata
        tmpSeekbackPos = bytestream.getPos();

        // Set member filename
        tmpMemberBytestream.setFilename(tmpMemberName);

        // Read member data
        tmpMemberBuf.resize(tmpMemberSize);
        bytestream.read(tmpMemberBuf.data(), tmpMemberSize, tmpMemberOffset);

        tmpMemberBytestream.write(tmpMemberBuf.data(), tmpMemberSize);

        // Go back to the next member metadata
        bytestream.seek(tmpSeekbackPos);

        this->push_back(tmpMemberBytestream);

        tmpMemberBytestream.reset();
        tmpMemberBuf.clear();
    }
}

void Arcv::readNmrCompArcv(giga::Bytestream& bytestream) {
    bytestream.seek(0x8);

    std::uint32_t compSize = bytestream.readScalar<std::uint32_t>();
    std::uint32_t uncompSize = bytestream.readScalar<std::uint32_t>();

    std::vector<std::uint8_t> compBuf(compSize);
    bytestream.read(compBuf.data(), compSize);

    std::vector<std::uint8_t> uncompBuf(uncompSize);
    giga::lzss::decompress(compBuf.data(), compSize, uncompBuf.data(), uncompSize, giga::lzss::Config{12, 4});

    bytestream.reset();

    bytestream.write(uncompBuf.data(), uncompSize);

    bytestream.seek(0x4);
}

void Arcv::finalizeFile(const std::string& filename, bool isCompressed, Version version) {
    giga::Bytestream bytestream;
    bytestream.setEndianness(giga::endian::Endianness::Little);

    switch(version) {
        case Version::NMBC:
            break;
        
        case Version::NMR:
            if(!isCompressed) {
                writeNmrUncompArcv(bytestream);
            } else {
                writeNmrUncompArcv(bytestream);
                writeNmrCompArcv(bytestream);
            }

            break;
    }

    bytestream.finalizeFile(filename);
}

void Arcv::writeNmrUncompArcv(giga::Bytestream& bytestream) {
    bytestream.write(reinterpret_cast<const std::uint8_t*>("VCRA"), 4);
    bytestream.writeScalar<std::uint32_t>(this->size());
    bytestream.write(reinterpret_cast<const std::uint8_t*>("TSIZ"), 4); // Temporary size
    bytestream.writeScalar<std::uint32_t>(0);
    bytestream.write(reinterpret_cast<const std::uint8_t*>("\5\2\7 "), 4);
    bytestream.writePadding(0, 0x2c);

    for(std::size_t i = 0; i < this->size(); i++) {
        bytestream.write(reinterpret_cast<const std::uint8_t*>("TOFF"), 4); // Temporary member offset
        bytestream.writeScalar<std::uint32_t>((*this)[i].getSize());
        bytestream.writeString((*this)[i].getFilename(), 0, 0x38);
    }

    std::size_t tmpMemberOffset;
    std::size_t tmpSeekbackPos;
    for(std::size_t i = 0; i < this->size(); i++) {
        tmpMemberOffset = 0;
        tmpSeekbackPos = 0;

        tmpMemberOffset = bytestream.getPos();

        bytestream.write((*this)[i].getBuf(), (*this)[i].getSize());

        while((bytestream.getPos() % 0x40)) {
            bytestream.writeScalar<std::uint8_t>(0xa3);
        };

        tmpSeekbackPos = bytestream.getPos();

        bytestream.seek(0x40 + (i * 0x40));
        bytestream.writeScalar<std::uint32_t>(tmpMemberOffset);

        bytestream.seek(tmpSeekbackPos);
    }

    bytestream.seek(0x8);
    bytestream.writeScalar<std::uint32_t>(bytestream.getSize());
}

void Arcv::writeNmrCompArcv(giga::Bytestream& bytestream) {
    std::vector<std::uint8_t> uncompBuf(bytestream.getSize());
    bytestream.seek(0x0);
    bytestream.read(uncompBuf.data(), uncompBuf.size());

    bytestream.reset();

    std::vector<std::uint8_t> compBuf(uncompBuf.size());

    std::size_t newCompBufSize = giga::lzss::compress(uncompBuf.data(), uncompBuf.size(), compBuf.data(), compBuf.size(), giga::lzss::Config{12, 4});
    compBuf.resize(newCompBufSize);

    bytestream.write(reinterpret_cast<const std::uint8_t*>("SSZL"), 4);
    bytestream.writeScalar<std::uint32_t>(0);
    bytestream.writeScalar<std::uint32_t>(newCompBufSize);
    bytestream.writeScalar<std::uint32_t>(uncompBuf.size());
    bytestream.write(compBuf.data(), newCompBufSize);
}

} // namespace unpac
