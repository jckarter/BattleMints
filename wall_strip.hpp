#ifndef __WALL_STRIP_HPP__
#define __WALL_STRIP_HPP__

#include "thing.hpp"
#include "serialization.hpp"
#include <vector>

namespace battlemints {

struct wall_strip : thing {
    std::vector<vec2> vertices;
    rect bounding_box; // order dependent init
    bool closed;

    template<typename InputIterator>
    wall_strip(InputIterator start, InputIterator fin, bool cl)
        : thing(INFINITYF, ZERO_VEC2), vertices(start, fin), bounding_box(_find_bbox()), closed(cl) 
        { }

    virtual bool does_collisions() const { return false; }

    virtual void draw();

    virtual void awaken() { _generate_collision_things(); }

    virtual rect visibility_box() { return bounding_box; }

    virtual char const * kind() const { return "wall_strip"; }

    static thing *from_json(Json::Value const &v);

private:
    rect _find_bbox();
    void _generate_collision_things();
};

}

#endif
