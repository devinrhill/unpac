#ifndef UNPAC_BRRES_H
#define UNPAC_BRRES_H

#include <giga/archive.h>
#include <giga/bytestream.h>

namespace unpac {

class Brres: public giga::Archive {
public:
    class Resource {
    public:
        void openHeaderFile(const std::string& filename);
        void openHeaderBytestream(giga::Bytestream& bytestream);

        char magic[4];
        std::uint32_t size;
        std::uint32_t version;
        std::int32_t outerBrresOffset;
        std::int32_t nameOffset;
    };

    class Tex0: private Resource {
    public:
        enum class Format: std::uint32_t {
            I4 = 0x0,
            I8 = 0x1,
            IA4 = 0x2,
            IA8 = 0x3,
            RGB565 = 0x4,
            RGB5A3 = 0x5,
            RGBA32 = 0x6,
            C4 = 0x8,
            C8 = 0x9,
            C14X2 = 0xa,
            CMPR = 0xe
        };

        void openFile(const std::string& filename);
        void openBytestream(giga::Bytestream& bytestream);

        bool getPaletteUsage() const noexcept;
        std::uint16_t getWidth() const noexcept;
        std::uint16_t getHeight() const noexcept;
        Format getFormat() const noexcept;
        const char* getFormatName() const noexcept;
        std::uint32_t getMipmapCount() const noexcept;
        std::uint8_t* getPixelBuf() const;

    private:
        void openTex0HeaderBytestream(giga::Bytestream& bytestream);
        std::uint32_t resolveOffsets(giga::Bytestream& bytestream);
        void parseImageData(giga::Bytestream& bytestream);

        bool _usesPalette = false;
        std::uint16_t _width = 0;
        std::uint16_t _height = 0;
        Format _format = Format::RGBA32;
        std::uint32_t _mipmapCount;
        std::vector<std::uint8_t> _pixelBuf;
        std::vector<std::vector<std::uint8_t>> _mipmapPixelBufs;
    };

    void openFile(const std::string& filename);
};

} // namespace unpac

#endif // UNPAC_BRRES_H
