#ifndef UNPAC_G03_ITEM_SET_H
#define UNPAC_G03_ITEM_SET_H

#include "giga/bytestream.hpp"

namespace unpac {
namespace g03 {

class Item {
public:
    enum class Type: std::uint32_t {
        PacDot = 0,
        PowerPellet = 1,
        KnightPowerUp = 2,
        WingPowerUp = 3,
        NormalPowerUp = 4,
        WholeApple = 5,
        HalfApple = 6,
        RedGrapes = 7,
        WhiteGrapes = 8,
        Bananas = 9,
        PeeledBanana = 10,
        BigCheeseburger = 11,
        Cheeseburger = 12,
        VanillaMilkshake = 13,
        ChocolateMilkshake = 14,
        WholeChicken = 15,
        ChickenDrumstick = 16,
        Pizza = 17,
        LifeUpFlag = 18
    };

    void openBytestream(giga::Bytestream& bytestream);

    const char* getTypeName();

    Type type = Type::PacDot;
    float x = 0;
    float y = 0;
    float z = 0;
    float doCollision = 1;
};

class ItemSet: public std::vector<Item> {
public:
    void openFile(const std::string& filename);
    void openBytestream(giga::Bytestream& bytestream);
};

} // namespace g03
} // namespace unpac

#endif // UNPAC_G03_ITEM_SET_H
