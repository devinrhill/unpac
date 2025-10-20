#ifndef GIGA_BYTESTREAM_H
#define GIGA_BYTESTREAM_H

#include <cstddef>
#include <format>
#include <stdexcept>
#include <string>
#include <vector>
#include "endianness.hpp"
#include "types.hpp"

// TODO: Add documentation

namespace giga {

class Bytestream {
public:
    enum class SeekMode {
        Beg = 1,
        Cur = 2,
        End = 3
    };

    Bytestream& operator=(Bytestream& other);

    void openFile(const std::string& filename);
	void finalizeFile(const std::string& filename);

    std::size_t seek(std::size_t pos, SeekMode mode = SeekMode::Beg);
    std::size_t resize(std::size_t newSize);

    // Input/Output
    std::size_t read(void* buf, std::size_t size, int offset = -1);
    std::size_t write(const void* buf, std::size_t size, int offset = -1);
    void writePadding(u8 padding, std::size_t paddingCount, int offset = -1);
    void writeString(const std::string& str, u8 pad = 0, std::size_t len = -1);

    // Getters
    bool isEmpty() const noexcept;
    std::string getFilename() const noexcept;
    u8* getBuf() noexcept;
    std::size_t getPos() const noexcept;
    std::size_t getSize() const noexcept;
    Endianness getEndianness() const noexcept;

    // Setters
    void setFilename(const std::string& filename) noexcept;
    void setBuf(const u8* buf, std::size_t size);
    void setEndianness(Endianness _endianness) noexcept;
    void reset() noexcept;

    // Fixed-width Input/Output
    u8 readU8();
    u16 readU16();
    u32 readU32();
    u64 readU64();
    s8 readS8();
    s16 readS16();
    s32 readS32();
    s64 readS64();
    float readFloat();
    double readDouble();

    void writeU8(u8 value);
    void writeU16(u16 value);
    void writeU32(u32 value);
    void writeU64(u64 value);
    void writeS8(s8 value);
    void writeS16(s16 value);
    void writeS32(s32 value);
    void writeS64(s64 value);
    void writeFloat(float value);
    void writeDouble(double value);
	
    // Template Input/Output
	template<typename T>
    T readScalar(int size = -1, int offset = -1) {
        static_assert(std::is_scalar<T>::value, "Type T is non-scalar");

        std::size_t tmpSize;
        if(size == -1) {
            tmpSize = sizeof(T);
        } else {
            tmpSize = size;
        }

        T num;
        try {
            this->read(reinterpret_cast<u8*>(&num), tmpSize, offset);
        } catch(const std::exception& e) {
            throw std::runtime_error(std::format("Couldn't read from buffer: {}", e.what()));
        }

        num = byteswapEndianness<T>(num, _endianness, tmpSize);

        return num;
    }

    template<typename T>
    void writeScalar(const T num, int size = -1, int offset = -1) {
        static_assert(std::is_scalar<T>::value, "Type T is non-scalar");

        std::size_t tmpSize;
        if(size == -1) {
            tmpSize = sizeof(T);
        } else {
            tmpSize = size;
        }

        // swap bytes if needed
        T tmpNum = byteswapEndianness<T>(num, _endianness, tmpSize);
        try {
            this->write(reinterpret_cast<const u8*>(&tmpNum), tmpSize, offset);
        } catch(const std::exception& e){
            throw std::runtime_error(std::format("Couldn't write to buffer: {}", e.what()));
        }
    }

private:
    std::string _filename = "";
    std::vector<u8> _buf = {};
    std::size_t _pos = 0;
    Endianness _endianness = NATIVE_ENDIANNESS;
};

} // namespace giga

#endif // GIGA_BYTESTREAM_H
