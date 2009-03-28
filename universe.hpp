#ifndef __UNIVERSE_HPP__
#define __UNIVERSE_HPP__

#include "game.hpp"
#include <bitset>
#include <string>
#include <boost/optional.hpp>

namespace battlemints {

struct universe {
    static const unsigned MAGIC = 0xBA77135A;
    static const int MAP_NAME_SIZE = 64, VERSION = 1;

    static universe instance;
    static boost::optional<std::string> name;

    static std::string filename(std::string const &nm);

    void set_default();
    void load(std::string const &nm);
    void save(std::string const &nm);

    unsigned magic;
    int version;
    int saved;
    std::bitset<NUM_GOALS> achieved_goals;
    std::bitset<NUM_COLOR_SWITCHES> flipped_color_switches;
    char current_map[MAP_NAME_SIZE];
    int current_checkpoint;

    void set_current_map(std::string const &s);
};

}

#endif
