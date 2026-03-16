#pragma once
#include <vector>
#include <string>

class LevelCatalog {
public:
    // 3D vector. Levels vector that constains each level that is a vector of strings
    // 'inline static' alows to define in the header
    inline static const std::vector<std::vector<std::string>> Levels = {

        // LEVEL 0
        {
            "xxxxxxxxxxxxxxx",
            "x1233333333221x",
            "x111111A111111x",
            "x2212222221222x",
            "x3312333332133x",
            "x33xxxxxxxxx33x",
            "x1112222222111x",
            "x111A11A11A111x",
            "xxxxxxx xxxxxxx"
        },

        // LEVEL 1
        {
            "              ",
            "              ",
            "              ",
            "      111     ",
            "      232     ",
            "      111     ",
            "              ",
            "              ",
            "              "
        },

        // LEVEL 2
        {
            "   11    11   ",
            " 111111111111 ",
            " 113311113311 ",
            " 111111111111 ",
            "   11    11   "
        },

        // LEVEL 3
        {
            "  111  xxx 111  ",
            " 11111BABC11111 ",
            " 11111111111111 ",
            "  111111111111  ",
            "    11111111    ",
            "      1111      ",
            "       11       "
        },

        // LEVEL 4
        {
            "       33       ",
            "      3223      ",
            "     321123     ",
            "    32111123    ",
            "   3333333333   "
        },

        // LEVEL 5
        {
            " 2   2   2   2 ",
            "   1   1   1   ",
            " 2   2   2   2 ",
            "   1   1   1   ",
            " 2   2   2   2 "
        },

        // LEVEL 6
        {
            " 33333333333333 ",
            " 3            3 ",
            " 3  11111111  3 ",
            " 3  11111111  3 ",
            " 3            3 ",
            " 333333  333333 " 
        }
    };
};