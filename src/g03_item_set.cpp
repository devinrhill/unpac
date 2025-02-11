#include <giga/bytestream.h>
#include <giga/endianness.h>
#include <iostream>
#include <cstdio>
#include "g03_item_set.h"

namespace unpac {
namespace g03 {

void Item::openBytestream(giga::Bytestream& bytestream) {
    type = bytestream.readScalar<Type>();

    bytestream.seek(0xc, giga::Bytestream::SeekMode::Cur);

    x = bytestream.readScalar<float>();
    y = bytestream.readScalar<float>();
    z = bytestream.readScalar<float>();
    doCollision = bytestream.readScalar<float>();

    bytestream.seek(0x10, giga::Bytestream::SeekMode::Cur);
}

const char* Item::getTypeName() {
    switch(type) {
        case Type::PacDot:
            return "Pac-Dot";

        case Type::PowerPellet:
            return "Power Pellet";

        case Type::KnightPowerUp:
            return "Knight Power-Up";

        case Type::WingPowerUp:
            return "Wing Power-Up";

        case Type::NormalPowerUp:
            return "Pac-Man Power-Up";

        case Type::WholeApple:
            return "Whole Apple";

        case Type::HalfApple:
            return "Half Apple";

        case Type::RedGrapes:
            return "Red Grapes";

        case Type::WhiteGrapes:
            return "White Grapes";

        case Type::Bananas:
            return "Bananas";

        case Type::PeeledBanana:
            return "Peeled Banana";

        case Type::BigCheeseburger:
            return "Big Cheeseburger";

        case Type::Cheeseburger:
            return "Cheeseburger";

        case Type::VanillaMilkshake:
            return "Vanilla Milkshake";

        case Type::ChocolateMilkshake:
            return "Chocolate Milkshake";

        case Type::WholeChicken:
            return "Whole Chicken";

        case Type::ChickenDrumstick:
            return "Chicken Drumstick";

        case Type::Pizza:
            return "Pizza";

        case Type::LifeUpFlag:
            return "Life-Up Flag";
    }

    return "Unknown item type";
}

void ItemSet::openFile(const std::string& filename) {
    giga::Bytestream bytestream;
    bytestream.openFile(filename);

    this->openBytestream(bytestream);
}

void ItemSet::openBytestream(giga::Bytestream& bytestream) {
    bytestream.setEndianness(giga::Endianness::Big);

    std::size_t itemCount = bytestream.getSize() / 0x30;
    this->resize(itemCount);

    for(Item& item: *this) {
        item.openBytestream(bytestream);
    }
}

} // namespace g03
} // namespace unpac
