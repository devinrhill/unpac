#ifndef UNPAC_ARCV_H
#define UNPAC_ARCV_H

#include <string>
#include <giga/archive.h>
#include <giga/bytestream.h>

namespace unpac {

class Arcv: public giga::Archive {
public:
    enum class Version {
        NMBC = 0, // Namco Museum Battle Collection (vol. 1 & 2)
        NMR = 1   // Namco Museum Remix & Megamix
    };

    void openFile(const std::string& filename, Version version = Version::NMR);
    void finalizeFile(const std::string& filename, bool isCompressed = true, Version version = Version::NMR);
private:

    // NMBC
    void readNmbcUncompArcv(giga::Bytestream& bytestream);

    // NMR
    void readNmrUncompArcv(giga::Bytestream& bytestream);
    void readNmrCompArcv(giga::Bytestream& bytestream);
    void writeNmrUncompArcv(giga::Bytestream& bytestream);
    void writeNmrCompArcv(giga::Bytestream& bytestream);
};

} // namespace unpac

#endif // UNPAC_ARCV_H
