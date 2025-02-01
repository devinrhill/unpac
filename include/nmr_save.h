#ifndef UNPAC_NMR_SAVE_H
#define UNPAC_NMR_SAVE_H

#include <string>

namespace unpac {

class NmrSave {
public:
    void openFile(const std::string& filename);
};

} // namespace unpac

#endif // UNPAC_NMR_SAVE_H
