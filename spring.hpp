#ifndef __SPRING_HPP__
#define __SPRING_HPP__

#include "sphere.hpp"

namespace battlemints {

struct spring : sphere {
    spring(vec2 c) : sphere(20000.0, c, 0.25, make_vec4(0.4, 0.25, 0.0, 1.0))
        { }

    virtual void collide(thing &t) { t.collide_spring(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_spring(s, *this); }
    virtual void collide_spring(spring &s) { collide_sphere_sphere(*this, s); }
    
};

}

#endif
