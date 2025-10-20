#include "brres/resource.h"

namespace unpac {
namespace brres {

void Resource::openBytestream(giga::Bytestream& bytestream) {
    char magic[4];
    bytestream.read(magic, 4);
}

void Resource::openFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.openFile(filename);

    this->openBytestream(bytestream);
}

} // namespace brres
} // namespace unpac
