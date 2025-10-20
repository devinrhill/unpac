#ifndef GIGA_ARCHIVE_H
#define GIGA_ARCHIVE_H

#include <string>
#include <vector>
#include "bytestream.hpp"

namespace giga {

class Archive: public std::vector<giga::Bytestream> {
public:
    // input
    void inject(const std::string& filename);
    void injectDirectory(const std::string& directoryName = ".");

    // output
    void extract(const std::string& filename, const std::string& directoryName = ".");
    void extractAll(const std::string& directoryName = ".");

    const std::string& getFilename() const noexcept;
    giga::Bytestream& getMember(const std::string& filename);
    void setFilename(const std::string& filename) noexcept;

private:
    std::string _filename = "";
};

} // namespace giga

#endif // GIGA_ARCHIVE_H
