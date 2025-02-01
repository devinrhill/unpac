#ifndef UNPAC_BRSAR_H
#define UNPAC_BRSAR_H

#include <cstdint>
#include <string>
#include <giga/bytestream.h>

namespace unpac {

class Brsar {
public:
    void openFile(const std::string& filename);

private:
    void readSymb(giga::Bytestream& bytestream, std::uint32_t symbOffset);
    void readInfo(giga::Bytestream& bytestream, std::uint32_t infoOffset);
    void readFile(giga::Bytestream& bytestream, std::uint32_t fileOffset);
};

} // namespace unpac

#endif // UNPAC_BRSAR_H
