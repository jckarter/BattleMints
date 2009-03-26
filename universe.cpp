#include "universe.hpp"
#include <cstdio>
#include <cstring>
#include <iostream>
#include "finally.hpp"
#include "serialization.hpp"

namespace battlemints {

universe universe::instance;

void universe::set_default()
{
    instance.magic = MAGIC;
    instance.version = VERSION;
    instance.achieved_goals.reset();
    instance.flipped_color_switches.reset();
    instance.set_current_map("hub");
    instance.current_checkpoint = 0;
}

void universe::load(std::string const &name)
{
    FILE *f = fopen(name.c_str(), "rb");
    finally<FILE*> close_f(f, fclose);

    safe_fread(&instance, sizeof(universe), 1, f);
    if (instance.magic != MAGIC) {
        std::cerr << "invalid universe magic in " << name << "\n";
        set_default();
    } else if (instance.version != VERSION) {
        std::cerr << "invalid universe version in " << name << "\n";
        set_default();
    }
}

void universe::save(std::string const &name)
{
    FILE *f = fopen(name.c_str(), "wb");
    finally<FILE*> close_f(f, fclose);

    safe_fwrite(&instance, sizeof(universe), 1, f);
}

void universe::set_current_map(std::string const &s)
{
    if (s.size() >= MAP_NAME_SIZE-1) {
        std::cerr << "current_map name " << s << " too long\n";
        strncpy(instance.current_map, s.c_str(), 63);
        instance.current_map[63] = '\0';
    } else {
        strcpy(instance.current_map, s.c_str());
        memset(instance.current_map + s.size(), '\0', MAP_NAME_SIZE - s.size());
    }
}

}
