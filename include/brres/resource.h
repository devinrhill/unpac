#ifndef UNPAC_BRRES_RESOURCE_H
#define UNPAC_BRRES_RESOURCE_H

#include <giga/archive.h>

namespace unpac {
namespace brres {

class Resource: public giga::Archive {
public:
    void openBytestream(giga::Bytestream& bytestream);
    void openFile(const std::string& filename);
};

} // namespace brres
} // namespace unpac

#endif // UNPAC_BRRES_RESOURCE_H
