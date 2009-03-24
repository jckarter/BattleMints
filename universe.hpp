#ifndef __UNIVERSE_HPP__
#define __UNIVERSE_HPP__

#include "game.hpp"
#include <bitset>

namespace battlemints {

struct universe {
    static universe instance;

    static void set_default();
    static void load(std::string const &name);
    static void save(std::string const &name);

    std::bitset<NUM_GOALS> achieved_goals;
    std::bitset<NUM_COLOR_SWITCHES> flipped_color_switches;
    std::string current_map;
    int current_checkpoint;
};

}

#endif
