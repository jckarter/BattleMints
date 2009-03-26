#ifndef __UNIVERSE_HPP__
#define __UNIVERSE_HPP__

#include "game.hpp"
#include <bitset>
#include <string>

namespace battlemints {

struct universe {
    static const unsigned MAGIC = 0xBA77135A;
    static const int MAP_NAME_SIZE = 64, VERSION = 1;

    static universe instance;

    static void set_default();
    static void load(std::string const &name);
    static void save(std::string const &name);

    unsigned magic;
    int version;
    std::bitset<NUM_GOALS> achieved_goals;
    std::bitset<NUM_COLOR_SWITCHES> flipped_color_switches;
    char current_map[MAP_NAME_SIZE];
    int current_checkpoint;

    void set_current_map(std::string const &s);
};

}

#endif
