#include "universe.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include "finally.hpp"
#include "serialization.hpp"

namespace battlemints {

universe universe::instance;
boost::optional<std::string> universe::name;

std::string universe::filename(std::string const &nm)
{
    return data_filename(nm + ".bu");
}

void universe::set_default_name()
{
    FILE *f = fopen(data_filename("last_universe").c_str(), "rb");
    if (!f)
        name.reset();
    else {
        finally<FILE*> close_f(f, fclose);
        char universe_name[256];
        memset(universe_name, 0, 256);
        fgets(universe_name, 256, f);
        name = std::string(universe_name);
    }
}

void universe::set_name(std::string const &n)
{
    name = n;
    FILE *f = fopen(data_filename("last_universe").c_str(), "wb");
    if (f) {
        finally<FILE*> close_f(f, fclose);
        fputs(n.c_str(), f);
    }
}

void universe::clear_name()
{
    unlink(data_filename("last_universe").c_str());
    name.reset();
}

void universe::set_default()
{
    magic = MAGIC;
    version = VERSION;
    saved = false;
    achieved_goals.reset();
    flipped_color_switches.reset();
    current_map = board_name::make(1, 0);
    current_checkpoint = 0;
}

void universe::load(std::string const &nm)
{
    FILE *f = fopen(filename(nm).c_str(), "rb");
    if (!f) {
        set_default();
        return;
    }

    finally<FILE*> close_f(f, fclose);

    safe_fread(this, sizeof(universe), 1, f);
    if (magic != MAGIC) {
        std::cerr << "invalid universe magic in " << nm << "\n";
        set_default();
    } else if (version != VERSION) {
        std::cerr << "invalid universe version in " << nm << "\n";
        set_default();
    }
}

void universe::save(std::string const &nm)
{
    saved = true;

    FILE *f = fopen(filename(nm).c_str(), "wb");
    if (!f) {
        std::cerr << "failed to write universe " << nm << "\n";
        return;
    }

    finally<FILE*> close_f(f, fclose);

    safe_fwrite(this, sizeof(universe), 1, f);
}

}
