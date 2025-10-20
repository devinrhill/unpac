#ifndef UNPAC_U8_ARC
#define UNPAC_U8_ARC

#include <string>
#include "giga/archive.hpp"

namespace unpac {

class U8arc: public giga::Archive {
public:
    void openFile(const std::string& filename);
};

} // namespace unpac

#endif // UNPAC_U8_ARC
