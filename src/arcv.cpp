#include "arcv.h"
#include <algorithm>
#include <cstring>
#include "giga/bytestream.hpp"
#include "giga/endianness.hpp"
#include "giga/lzss.hpp"
#include <string>

namespace unpac {

void Arcv::openFile(const std::string &filename, Version version) {
  giga::Bytestream bytestream;
  bytestream.setEndianness(giga::Endianness::Little);
  bytestream.openFile(filename);

  char magic[4];
  bytestream.read(reinterpret_cast<std::uint8_t *>(magic), 4);
  switch (version) {
  case Version::NMBC:
    if (!std::equal(magic, magic + 4, "ARCV")) {
      readNmbcUncompArcv(bytestream);
    }

    break;
  case Version::NMR:
    if (std::equal(magic, magic + 4, "SSZL")) {
      readNmrCompArcv(bytestream);
      readNmrUncompArcv(bytestream);
    } else if (std::equal(magic, magic + 4, "VCRA")) {
      readNmrUncompArcv(bytestream);
    } else {
      return;
    }

    break;
  }

  // Set our filename at the end so we know no errors occured
  this->setFilename(filename);
}

// TODO: finish
void Arcv::readNmbcUncompArcv(giga::Bytestream &bytestream) {
  (void)bytestream;

  /*
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
  */
}

void Arcv::readNmrUncompArcv(giga::Bytestream &bytestream) {
  std::uint32_t memberCount = bytestream.readScalar<std::uint32_t>();
  if (memberCount < 1) { // Empty archive
    return;
  }

  std::uint32_t size = bytestream.readScalar<std::uint32_t>();
  if (size < 0x40) { // Empty archive
    return;
  }

  bytestream.seek(0x40);

  // Temporary current member metadata variables
  std::uint32_t tmpMemberOffset, tmpMemberSize;
  char tmpMemberName[0x38];
  std::size_t tmpSeekbackPos;

  this->resize(memberCount);
  for (std::uint32_t i = 0; i < memberCount; i++) {
    // Read current member metadata
    tmpMemberOffset = bytestream.readScalar<std::uint32_t>();
    tmpMemberSize = bytestream.readScalar<std::uint32_t>();

    // Read current member filename
    bytestream.read(reinterpret_cast<std::uint8_t *>(tmpMemberName), 0x38);

    // Save our position at the next member metadata
    tmpSeekbackPos = bytestream.getPos();

    // Set current member filename
    (*this)[i].setFilename(tmpMemberName);

    // Trick to resize current internal member buffer
    (*this)[i].writePadding(0x0, tmpMemberSize);

    // Read member data to current member
    bytestream.read((*this)[i].getBuf(), tmpMemberSize, tmpMemberOffset);
    (*this)[i].seek(
        0x0); // Make sure we don't run out of characters to read for future use

    // Go back to the next member metadata
    bytestream.seek(tmpSeekbackPos);
  }

  bytestream.seek(0x0);
}

void Arcv::readNmrCompArcv(giga::Bytestream &bytestream) {
  // Go to the start of useful LZS data
  bytestream.seek(0x8);

  // Get compressed and uncompressed file sizes
  std::uint32_t compSize = bytestream.readScalar<std::uint32_t>();
  std::uint32_t uncompSize = bytestream.readScalar<std::uint32_t>();

  // Read our compressed buffer
  std::vector<std::uint8_t> compBuf(compSize);
  bytestream.read(compBuf.data(), compSize);

  // Decompress the compressed buffer
  std::vector<std::uint8_t> uncompBuf(uncompSize);
  giga::lzss::decompress(compBuf.data(), compSize, uncompBuf.data(), uncompSize,
                         giga::lzss::Config{12, 4});

  // Write our uncompressed buffer to the output bytestream
  bytestream.reset();
  bytestream.write(uncompBuf.data(), uncompSize);

  // Go back to where we left off at openFile
  // so we can read the uncompressed buffer instead
  bytestream.seek(0x4);
}

void Arcv::finalizeFile(const std::string &filename, bool isCompressed,
                        Version version) {
  this->setFilename(filename);

  giga::Bytestream bytestream;
  bytestream.setEndianness(giga::Endianness::Little);

  switch (version) {
  case Version::NMBC:
    break;

  case Version::NMR:
    if (!isCompressed) {
      writeNmrUncompArcv(bytestream);
    } else {
      writeNmrUncompArcv(bytestream);
      writeNmrCompArcv(bytestream);
    }

    break;
  }

  bytestream.finalizeFile(filename);
}

void Arcv::writeNmrUncompArcv(giga::Bytestream &bytestream) {
  // Write ARCV header
  bytestream.write(reinterpret_cast<const std::uint8_t *>("VCRA"), 0x4);
  bytestream.writeScalar<std::uint32_t>(this->size()); // Member count
  bytestream.write(reinterpret_cast<const std::uint8_t *>("TSIZ"),
                   0x4); // Temporary file size
  bytestream.writeScalar<std::uint32_t>(
      0x0); // Unused; presumably the compressed file size if this were like a
            // NMBC .bin
  bytestream.write(reinterpret_cast<const std::uint8_t *>("\5\2\7 "),
                   0x4);              // This weird number found in every ARCV
  bytestream.writePadding(0x0, 0x2c); // Write the rest of the header padding

  // Write ARCV member metadata
  for (std::size_t i = 0; i < this->size(); i++) {
    bytestream.write(reinterpret_cast<const std::uint8_t *>("TOFF"),
                     0x4); // Temporary member offset
    bytestream.writeScalar<std::uint32_t>((*this)[i].getSize());
    bytestream.writeString(
        (*this)[i].getFilename(), 0x00,
        0x38); // Member filenames are kept at 0x38 characters
  }

  // Write ARCV member data
  std::size_t tmpMemberOffset;
  for (std::size_t i = 0; i < this->size(); i++) {
    tmpMemberOffset = bytestream.getPos();

    bytestream.write((*this)[i].getBuf(), (*this)[i].getSize());

    // Write the padding to make member data aligned to 0x40
    // due to a restriction in the Wii filesystem
    while ((bytestream.getPos() % 0x40)) {
      bytestream.writeScalar<std::uint8_t>(0xa3);
    }

    bytestream.writeScalar<std::uint32_t>(
        tmpMemberOffset, sizeof(std::uint32_t), 0x40 + (i * 0x40));
  }

  // Write ARCV size at the end
  bytestream.writeScalar<std::uint32_t>(bytestream.getSize(),
                                        sizeof(std::uint32_t), 0x8);

  bytestream.seek(0x0);
}

void Arcv::writeNmrCompArcv(giga::Bytestream &bytestream) {
  std::vector<std::uint8_t> uncompBuf(bytestream.getSize());
  bytestream.seek(0x0);
  bytestream.read(uncompBuf.data(), uncompBuf.size());

  bytestream.reset();

  std::vector<std::uint8_t> compBuf(uncompBuf.size());

  std::size_t newCompBufSize =
      giga::lzss::compress(uncompBuf.data(), uncompBuf.size(), compBuf.data(),
                           compBuf.size(), giga::lzss::Config{12, 4});
  compBuf.resize(newCompBufSize);

  bytestream.write(reinterpret_cast<const std::uint8_t *>("SSZL"), 4);
  bytestream.writeScalar<std::uint32_t>(0);
  bytestream.writeScalar<std::uint32_t>(newCompBufSize);
  bytestream.writeScalar<std::uint32_t>(uncompBuf.size());
  bytestream.write(compBuf.data(), newCompBufSize);

  bytestream.seek(0x0);
}

} // namespace unpac
