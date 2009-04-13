#ifndef __UNIVERSE_HPP__
#define __UNIVERSE_HPP__

#include "game.hpp"
#include "board.hpp"
#include <bitset>
#include <string>
#include <boost/optional.hpp>

namespace battlemints {

struct universe {
    static const unsigned MAGIC = 0xBA77135A;
    static const int VERSION = 2;

    static universe instance;
    static boost::optional<std::string> name;

    static void set_default_name();
    static void set_name(std::string const &n);
    static void clear_name();

    static std::string filename(std::string const &nm);

    void set_default();
    void load(std::string const &nm);
    void save(std::string const &nm);

    unsigned magic;
    int version;
    int saved;
    std::bitset<NUM_GOALS> achieved_goals;
    std::bitset<NUM_COLOR_SWITCHES> flipped_color_switches;
    board_name current_map;
    int current_checkpoint;
};

}

#endif
