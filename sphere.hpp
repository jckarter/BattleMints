#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <CoreGraphics/CoreGraphics.h>
#include <boost/array.hpp>
#include "collision.hpp"
#include "thing.hpp"

namespace battlemints {

struct sphere : thing {
    vec4 color;
    float radius;

    virtual rect visibility_box();
    virtual rect collision_box();
    virtual void draw();

    virtual void collide(thing &t) { t.collide_sphere(*this); }
    virtual void collide_sphere(sphere &s) { collide_sphere_sphere(*this, s); }
    virtual void collide_spring(spring &s) { collide_sphere_spring(*this, s); }

    virtual float collision_time(thing const &t) const
        { return t.collision_time_sphere(*this); }
    virtual float collision_time_sphere(sphere const &s) const
        { return collision_time_sphere_sphere(*this, s); }

    sphere(float m, vec2 ct, float r, vec4 co)
        : thing(m, ct), color(co), radius(r) { _set_up_drawing(); }

    virtual ~sphere() { _tear_down_drawing(); }

    virtual char const * kind() const { return "sphere"; }
    virtual void print(std::ostream &os) const
        { thing::print(os); os << " color:" << color << " r:" << radius; }

private:
    GLuint _texture;
    boost::array<float, 8> _vertices;

    void _set_up_drawing();
    void _tear_down_drawing();

    CGContextRef _context(unsigned pixel_radius, void *data);
    void _render_sphere_texture(float border_radius, unsigned pixel_radius, void *data);
    GLuint _make_sphere_texture(float radius, unsigned pixel_radius);
};

}

#endif
