#ifndef __EXPLOSION_HPP__
#define __EXPLOSION_HPP__

#include "thing.hpp"

namespace battlemints {

struct explosion : thing {
    explosion(vec2 ce);

    static void explode(thing *th);

    virtual rect visibility_box();
    virtual void draw();

    virtual bool does_collisions() const { return false; }

    virtual void tick();

    virtual char const * kind() const { return "explosion"; }

    struct particle {
        vec2 center, velocity;
        float size;
    };

    unsigned age;
    std::vector<particle> particles;

private:
    static vec2 _random_velocity();
    static float _random_size();
};

}

#endif
