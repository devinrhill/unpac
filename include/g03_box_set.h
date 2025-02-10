#ifndef UNPAC_G03_BOX_SET_H
#define UNPAC_G03_BOX_SET_H

#include <vector>

namespace unpac {

class Box {
public:
    enum class Type {
        BigWooden = 0,
        SmallWooden = 1,
        BigIron = 2,
        SmallIron = 3
    };

    enum class ItemType {
        NoItem = 0,
        PacDot = 1,
        PacDots3 = 2,
        PacDots6 = 3,
        PacDots9 = 4,
        PowerPellet = 5,
        KnightPowerUp = 6,
        WingPowerUp = 7,
        NormalPowerUp = 8,
        WholeApple = 9,
        HalfApple = 10,
        RedGrapes = 11,
        WhiteGrapes = 12,
        Bananas = 13,
        PeeledBanana = 14,
        BigCheeseburger = 15,
        Cheeseburger = 16,
        VanillaMilkshake = 17,
        ChocolateMilkshake = 18,
        WholeChicken = 19,
        ChickenDrumstick = 20,
        Pizza = 21,
        LifeUpFlag = 22
    };
};

class BoxSet: public std::vector<Box> {
public:
};

} // namespace unpac

#endif // UNPAC_G03_BOX_SET_H
