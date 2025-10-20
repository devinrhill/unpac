#include "giga/bytestream.hpp"
#include "giga/types.hpp"
#include <format>
#include <iostream>
#include <string>
#include "brres.h"

namespace unpac {

void Brres::Resource::openHeaderFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.openFile(filename);

    this->openHeaderBytestream(bytestream);
}

void Brres::Resource::openHeaderBytestream(giga::Bytestream& bytestream) {
    bytestream.setEndianness(giga::Endianness::Big);

    bytestream.read(reinterpret_cast<std::uint8_t*>(magic), 4);
    size = bytestream.readScalar<std::uint32_t>();
    version = bytestream.readScalar<std::uint32_t>();
    outerBrresOffset = bytestream.readScalar<std::int32_t>();
}

void Brres::Tex0::openFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.openFile(filename);

    this->openBytestream(bytestream);
}

void Brres::Tex0::openTex0HeaderBytestream(giga::Bytestream& bytestream) {
    bytestream.setEndianness(giga::Endianness::Big);

    _usesPalette = bytestream.readScalar<std::uint32_t>();
    std::cout << bytestream.getPos() << std::endl;
    _width = bytestream.readScalar<std::uint16_t>();
    _height = bytestream.readScalar<std::uint16_t>();
    _format = bytestream.readScalar<Format>();
    _mipmapCount = bytestream.readScalar<std::uint32_t>();
    float minMipmapCount = bytestream.readScalar<float>();
    float maxMipmapCount = bytestream.readScalar<float>();
}

void Brres::Tex0::openBytestream(giga::Bytestream& bytestream) {
    bytestream.setEndianness(giga::Endianness::Big);

    this->openHeaderBytestream(bytestream);

    if(!std::equal(magic, magic + 4, "TEX0")) {
        return;
    }

    std::uint32_t imageDataOffset = this->resolveOffsets(bytestream);

    std::int32_t nameDataOff = bytestream.readScalar<std::uint32_t>();

    this->openTex0HeaderBytestream(bytestream);

    bytestream.seek(imageDataOffset);

    std::cout << std::format("res: {}x{}, raw size: {}, pixels: {}", _width, _height, _pixelBuf.size(), _pixelBuf.size() / 0x4) << std::endl;

    this->parseImageData(bytestream);
}

std::uint32_t Brres::Tex0::resolveOffsets(giga::Bytestream& bytestream) {
    std::uint32_t offsetCount;
    switch(version) {
        case 1: {
            offsetCount = 1;
            break;
        } case 2: {
            offsetCount = 2;
            break;
        } case 3: {
            offsetCount = 1;
            break;
        } default: {
            offsetCount = 0;
        }
    }

    std::vector<std::uint32_t> offsets(offsetCount);
    for(std::uint32_t i = 0; i < offsetCount; i++) {
        offsets[i] = bytestream.readScalar<std::uint32_t>();
    }

    std::uint32_t imageStartOffset;
    switch(version) {
        case 1: {
            imageStartOffset = std::abs(outerBrresOffset) + offsets[0];

            break;
        } case 2: {
            imageStartOffset = std::abs(outerBrresOffset) + offsets[1];

            break;
        } case 3: {
            imageStartOffset = std::abs(outerBrresOffset) + offsets[0];

            break;
        }
    }

    return imageStartOffset;
}

void Brres::Tex0::parseImageData(giga::Bytestream& bytestream) {
    std::uint8_t tmpPixel[0x4];

    std::uint8_t tmpCodedBlock[0x40];
    std::uint8_t tmpBlock[0x40];

    giga::Bytestream test;

    // info shits
    std::uint32_t blockCount;
    std::uint32_t Bpp;

    giga::Bytestream blocks;

    std::uint32_t j, k, m, n;
    switch(_format) {
/*
        case Format::I4: {
            for(std::uint32_t j = 0; j < (_width * _height) / 0x2; j++) {
                tmpPixelBuf = bytestream.readScalar<std::uint8_t>();

                tmpPixel[0] = ((tmpPixelBuf & 0x0f) << 0x4) * 0x11;
                tmpPixel[1] = ((tmpPixelBuf & 0x0f) << 0x4) * 0x11;
                tmpPixel[2] = ((tmpPixelBuf & 0x0f) << 0x4) * 0x11;
                tmpPixel[3] = (tmpPixelBuf & 0xf0) * 0x11;

                for(std::uint32_t k = 0; k < 0x4; k++) {
                    _pixelBuf.push_back(tmpPixel[k]);
                }
            }

            break;
        } */
        case Format::RGBA8: {
            blocks = this->resolveBlocks();

            Bpp = 4;
            blockCount = (_width * _height * Bpp) / 0x40;

            blocks.writePadding(0x0, blockCount * 0x40);
            blocks.seek(0x0);

            // for each block
            for(j = 0; j < blockCount; j++) {
                n = 0;

                // read block data
                bytestream.read(tmpCodedBlock, 0x40);

                // for each 16 pixels
                for(k = 0; k < 0x10; k++) {
                    tmpPixel[0] = tmpCodedBlock[(k * 2) + 0x1];
                    tmpPixel[1] = tmpCodedBlock[(k * 2) + 0x20];
                    tmpPixel[2] = tmpCodedBlock[(k * 2) + 0x21];
                    tmpPixel[3] = tmpCodedBlock[(k * 2)];

                    // std::cout << std::format("{:02x}{:02x}{:02x}{:02x}", tmpPixel[0], tmpPixel[1], tmpPixel[2], tmpPixel[3]) << std::endl;

                    for(m = 0; m < 0x4; m++) {
                        tmpBlock[(n * 0x4) + m] = tmpPixel[m];
                    }
                    n++;

                }
                blocks.write(tmpBlock, 0x40);
            }

            _pixelBuf.resize(_width * _height * Bpp, 0);

            giga::u32 bpr = _width / 4;
            giga::u32 bpc = _height / 4;

            for(giga::u32 by = 0; by < bpc; ++by) {
                for(giga::u32 bx = 0; bx < bpr; ++bx) {
                    giga::u32 bidx = (by * bpr) + bx;
                    giga::u32 bs = bidx * (4 * 4 * Bpp);

                    for(giga::u32 y = 0; y < 4; ++y) {
                        for(giga::u32 x = 0; x < 4; ++x) {
                            giga::u32 pidx = (y * 4 + x) * Bpp;
                            giga::u32 ox = bx * 4 + x;
                            giga::u32 oy = by * 4 + y;
                            giga::u32 oidx = (oy * _width + ox) * Bpp;

                            for(giga::u32 c = 0; c < Bpp; ++c) {
                                _pixelBuf[oidx + c] = blocks.getBuf()[bs + pidx + c];
                            }
                        }
                    }
                }
            }

            break;
        } default: {
            return;
        }
    }
}

giga::Bytestream Brres::Tex0::resolveBlocks(giga::Bytestream& bytestream, Format format) {
    switch(format) {
        case Format::RGBA8: {
            giga::Bytestream blocks;
            std::uint8_t tmpPixel[0x4];

            std::uint8_t tmpCodedBlock[0x40];
            std::uint8_t tmpBlock[0x40];


            giga::uint Bpp = 4;
            giga::uint blockCount = (_width * _height * Bpp) / 0x40;

            blocks.writePadding(0x0, blockCount * 0x40);
            blocks.seek(0x0);

            giga::uint n, m;
            // for each block
            for(giga::uint j = 0; j < blockCount; j++) {
                n = 0;

                // read block data
                bytestream.read(tmpCodedBlock, 0x40);

                // for each 16 pixels
                for(giga::uint k = 0; k < 0x10; k++) {
                    tmpPixel[0] = tmpCodedBlock[(k * 2) + 0x1];
                    tmpPixel[1] = tmpCodedBlock[(k * 2) + 0x20];
                    tmpPixel[2] = tmpCodedBlock[(k * 2) + 0x21];
                    tmpPixel[3] = tmpCodedBlock[(k * 2)];

                    // std::cout << std::format("{:02x}{:02x}{:02x}{:02x}", tmpPixel[0], tmpPixel[1], tmpPixel[2], tmpPixel[3]) << std::endl;

                    for(m = 0; m < 0x4; m++) {
                        tmpBlock[(n * 0x4) + m] = tmpPixel[m];
                    }
                    n++;

                }
                blocks.write(tmpBlock, 0x40);
            }

            _pixelBuf.resize(_width * _height * Bpp, 0);


            break;
        }
    }
}

bool Brres::Tex0::getPaletteUsage() const noexcept {
    return _usesPalette;
}

std::uint16_t Brres::Tex0::getWidth() const noexcept {
    return _width;
}

std::uint16_t Brres::Tex0::getHeight() const noexcept {
    return _height;
}

Brres::Tex0::Format Brres::Tex0::getFormat() const noexcept {
    return _format;
}

const char* Brres::Tex0::getFormatName() const noexcept {
    switch(_format) {
        case Format::I4: {
            return "I4";
        } case Format::I8: {
            return "I8";
        } case Format::IA4: {
            return "IA4";
        } case Format::IA8: {
            return "IA8";
        } case Format::RGB565: {
            return "RGB565";
        } case Format::RGB5A3: {
            return "RGB5A3";
        } case Format::RGBA8: {
            return "RGBA8";
        } case Format::C4: {
            return "C4";
        } case Format::C8: {
            return "C8";
        } case Format::C14X2: {
            return "C14X2";
        } case Format::CMPR: {
            return "CMPR";
        } default: {
            return "Unknown format";
        }
    }
}

std::uint32_t Brres::Tex0::getMipmapCount() const noexcept {
    return _mipmapCount;
}

std::uint8_t* Brres::Tex0::getPixelBuf() const {
    return const_cast<std::uint8_t*>(_pixelBuf.data());
}

void Brres::openBytestream(giga::Bytestream& bytestream) {
    char magic[4];
    bytestream.read(reinterpret_cast<std::uint8_t*>(magic), 4);

    std::uint8_t bom[2];
    bytestream.read(bom, 2);
    if(std::equal(bom, bom + 2, "\xfe\xff")) {
        bytestream.setEndianness(giga::Endianness::Big);
    } else if(std::equal(bom, bom + 2, "\xff\xfe")) {
        bytestream.setEndianness(giga::Endianness::Little);
    } else {
        return;
    }

    bytestream.seek(0x8);

    // std::uint32_t fileSize = bytestream.readScalar<std::uint32_t>();
}

void Brres::openFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.openFile(filename);

    this->openBytestream(bytestream);
}

} // namespace unpac
