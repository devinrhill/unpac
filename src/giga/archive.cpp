#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include "archive.hpp"

namespace giga {

void Archive::inject(const std::string& filename) {
    struct stat st;
    if(!stat(filename.c_str(), &st)) {
        if(st.st_mode & S_IFREG) {
            std::filesystem::path path = std::filesystem::path(filename).relative_path();

            giga::Bytestream bytestream;
            bytestream.openFile(filename);
            bytestream.setFilename(path.filename());
            // std::cout << std::format("{}\n{}\n\n", path.string(), bytestream.getFilename());

            this->push_back(bytestream);
        }
    }
}

void Archive::injectDirectory(const std::string& directoryName) {
    for(const auto& directoryEntry: std::filesystem::recursive_directory_iterator(directoryName)) {
        inject(directoryEntry.path());
    }
}

void Archive::extract(const std::string& filename, const std::string& directoryName) {
    std::filesystem::create_directories(directoryName);

    std::string tmpPath;
    for(Bytestream& bytestream: *this) {
        if(bytestream.getFilename() == filename) {
            std::filesystem::path rel = std::filesystem::path(filename).parent_path();
            std::filesystem::path fullDir = directoryName / rel;

            std::filesystem::create_directories(fullDir);

            tmpPath = (std::filesystem::path(directoryName) / std::filesystem::path(filename)).string();

            bytestream.finalizeFile(tmpPath);

            break;
        }
    }
}

void Archive::extractAll(const std::string& directoryName) {
    std::filesystem::create_directory(directoryName);

    std::filesystem::path tmpPath;
    for(Bytestream& bytestream: *this) {
        tmpPath = std::filesystem::path(directoryName) / std::filesystem::path(bytestream.getFilename());

        std::filesystem::create_directories(tmpPath.parent_path());
        
        bytestream.finalizeFile(tmpPath.string());
    }
}

const std::string& Archive::getFilename() const noexcept {
    return _filename;
}

giga::Bytestream& Archive::getMember(const std::string& filename) {
    for(giga::Bytestream& bytestream: *this) {
        if(bytestream.getFilename() == filename) {
            return bytestream;
        }
    }

    throw std::runtime_error(std::format("Couldn't find member '{}'", filename));
}

void Archive::setFilename(const std::string& filename) noexcept {
    _filename = filename;
}

} // namespace giga
