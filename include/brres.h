#ifndef UNPAC_BRRES_H
#define UNPAC_BRRES_H

#include <giga/archive.h>

namespace unpac {

class Brres: public giga::Archive {
public:
    void openFile(const std::string& filename);
};

} // namespace unpac

#endif // UNPAC_BRRES_H
