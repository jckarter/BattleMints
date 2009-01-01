#ifndef __DRAMATIS_PERSONAE_HPP__
#define __DRAMATIS_PERSONAE_HPP__

#include "game.hpp"
#include "geom.hpp"
#include "thing.hpp"
#include "board.hpp"
#include "particles.hpp"
#include "serialization.hpp"
#include "drawing.hpp"
#include "transition.hpp"

namespace battlemints {

struct camera : thing {
    static const float LEAD_FACTOR, FOLLOW_FACTOR;

    thing *target;

    camera() : thing(0.0f, ZERO_VEC2, 0.0f), target(NULL) { }

    virtual bool does_draws() const { return false; }
    virtual bool does_collisions() const { return false; }

    virtual void tick();

    void cut_to_target(thing *new_target) { target = new_target; center = new_target->center; }
};

struct player : sphere {
    static const float ACCEL_SCALE, RADIUS, MASS, SPRING;
    static const vec4 COLOR;
    static sphere_texture *texture;
    static sphere_face *face;

    player(vec2 center) : sphere(MASS, center, RADIUS, SPRING) { }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    virtual void draw();

    void die() { board::current()->particles_thing()->explode(this); board::restart_with<death_transition>(); }

    static thing *from_json(Json::Value const &v);
    static void global_start();
    static void global_finish();

private:
    vec2 _cur_accel();
};

struct enemy : sphere {
    float accel;
    float responsiveness;

    thing *target;
    vec2 cur_accel;

    enemy(float m, vec2 ct, float r, float sp, float a, float re)
        : sphere(m, ct, r, sp), accel(a), responsiveness(re),
          target(NULL), cur_accel(ZERO_VEC2) {}

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    void die() { board::current()->particles_thing()->explode(this); }

    virtual char const * kind() const { return "enemy"; }

protected:
    template<typename Enemy>
    static thing *from_json(Json::Value const &v)
    {
        vec2 center = vec2_from_json(v["center"]);
        return new Enemy(center);
    }
};

struct mini : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, RESPONSIVENESS;

    static boost::ptr_vector<sphere_texture> textures;
    static sphere_face *face;

    sphere_texture &texture;

    mini(vec2 ct)
        : enemy(MASS, ct, RADIUS, SPRING, ACCEL, RESPONSIVENESS),
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

struct mega : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, RESPONSIVENESS;
    static const vec4 COLOR;

    static sphere_texture *texture;

    mega(vec2 ct)
        : enemy(MASS, ct, RADIUS, SPRING, ACCEL, RESPONSIVENESS) { }

    virtual char const * kind() const { return "mega"; }

    virtual void draw() { _push_translate(); texture->draw(); glPopMatrix(); }
    virtual void wall_damage() { }

    static thing *from_json(Json::Value const &v) { return enemy::from_json<mega>(v); }
    static void global_start();
    static void global_finish();
};

}

#endif
