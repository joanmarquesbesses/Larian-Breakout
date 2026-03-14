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
            "          ",
            " 11111111 ",
            " 12222221 ",
            " 12333321 ",
            " 12222221 ",
            " 11111111 "
        },

        // LEVEL 1
        {
            " 111 ",
            " 232 ",
            " 111 "
        },

        // LEVEL 2
        {
            "   11    11   ",
            " 111111111111 ",
            " 113311113311 ",
            " 111111111111 ",
            "   11    11   "
        }
    };
};