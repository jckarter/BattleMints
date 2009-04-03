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
    set_current_map(NEW_GAME_MAP);
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

void universe::set_current_map(std::string const &s)
{
    if (s.size() >= MAP_NAME_SIZE-1) {
        std::cerr << "current_map name " << s << " too long\n";
        strncpy(current_map, s.c_str(), 63);
        current_map[63] = '\0';
    } else {
        strcpy(current_map, s.c_str());
        memset(current_map + s.size(), '\0', MAP_NAME_SIZE - s.size());
    }
}

}
