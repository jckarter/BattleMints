#ifndef __MINI_HPP__
#define __MINI_HPP__

#include "enemy.hpp"
#include "drawing.hpp"
#include "sphere_face.hpp"
#include <boost/ptr_container/ptr_vector.hpp>
#include <cstdlib>

namespace battlemints {

struct mini : enemy {
    static const float ACCEL;
    static const float RADIUS;

    static boost::ptr_vector<sphere_texture> textures;
    static sphere_face *face;

    sphere_texture &texture;

    mini(vec2 ct)
        : enemy(0.5, ct, RADIUS, 1.0, ACCEL, 0.75),
          texture(textures[rand() % textures.size()]) { }

    virtual char const * kind() const { return "mini"; }

    virtual void draw()
    {
        _push_translate();
        texture.draw();
        glColor4f(0.0, 0.0, 0.0, 1.0);
        face->draw_for_course(velocity, cur_accel);
        glPopMatrix();
    }

    static thing *from_json(Json::Value const &v) { return enemy::from_json<mini>(v); }
    static void global_start();
    static void global_finish();
};

}

#endif
