#pragma once
#include <vector>
#include <string>

class LevelCatalog {
public:
    // 3D vector. Levels vector that constains each level (which is a vector of strings)
    // 'inline static' allows definition directly in the header file
    inline static const std::vector<std::vector<std::string>> Levels = {

        // LEVEL 1: The Warm-up
        {
            "               ",
            "               ",
            "    2222222    ",
            "   211111112   ",
            "  21122222112  ",
            "   211111112   ",
            "    2222222    ",
            "               ",
            "               "
        },

        // LEVEL 2: Space Invader
        {
            "    1     1    ",
            "     1   1     ",
            "    1111111    ",
            "   223222322   ",
            "  33333333333  ",
            "  3 3333333 3  ",
            "  3 3     3 3  ",
            "     22 22     ",
            "               "
        },

        // LEVEL 3: The Twin Fortresses
        {
            "X             X",
            "X33333X X33333X",
            "X32223X X32223X",
            "X32123X X32123X",
            "X32223X X32223X",
            "X33333X X33333X",
            "XXXXXXX XXXXXXX",
            "               ",
            "               "
        },

        // LEVEL 4: The Funnel
        {
            "XXXXXXXXXXXXXXX",
            "X1111111111111X",
            "X1222222222221X",
            "X12B3A3C3A3B21X",
            "XXXXXX   XXXXXX",
            "     X A X     ",
            "     X 2 X     ",
            "     X 1 X     ",
            "     X   X     "
        },

        // LEVEL 5: The Explosive Pyramid
        {
            "               ",
            "               ",
            "       C       ",
            "      C3C      ",
            "     C232C     ",
            "    C11111C    ",
            "   CCCCCCCCC   ",
            "  CCCCCCCCCCC  ",
            "               "
        }
    };
};