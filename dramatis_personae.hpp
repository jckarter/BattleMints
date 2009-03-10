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
#include <boost/array.hpp>
#include <iostream>
#include <vector>

namespace battlemints {

void global_start_actors();

struct player : sphere {
    static const float ACCEL_SCALE, RADIUS, MASS, SHIELD_RADIUS, SHIELD_SPRING, SPRING, DAMP;
    static const vec4 COLOR, SHIELD_COLOR;

    static boost::array<renders_with_pair, 3> renders_with_pairs_template;

    bool shielded;
    int grace_period;

    player(vec2 center)
        : sphere(center, MASS, RADIUS, SPRING, DAMP), shielded(false), grace_period(0) { }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    virtual void wall_damage() { damage(); }
    virtual void post_damage() { damage(); }

    virtual renders_with_range renders_with() const;
    virtual vec4 sphere_color(float r) { return r == SHIELD_RADIUS ? SHIELD_COLOR : COLOR; }

    void damage();
    void die();
    void lose_shield();
    void gain_shield();

    static thing *from_json(Json::Value const &v) { return sphere::from_json<player>(v); }
};

struct powerup : sphere {
    static const float RADIUS, MASS, SPRING, DAMP;
    static const vec4 CHARGED_COLOR, DEAD_COLOR, PULSE_COLOR;
    static const int CHARGE_TIME = 300;

    static boost::array<renders_with_pair, 1> renders_with_pairs;

    std::string powerup_kind;
    unsigned charge_time;

    powerup(vec2 center, std::string const &k)
        : sphere(center, MASS, RADIUS, SPRING, DAMP), powerup_kind(k), charge_time(0) { }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float);

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual char const * kind() const { return "powerup"; }

    static thing *from_json(Json::Value const &v);
};

struct enemy : sphere {
    float accel;
    float responsiveness;

    thing *target;

    enemy(vec2 ct, float m, float r, float sp, float d, float a, float re)
        : sphere(ct, m, r, sp, d), accel(a), responsiveness(re),
          target(NULL) {}

    virtual void tick();
    virtual void on_collision(thing &o);

    virtual void wall_damage() { die(); }
    virtual void post_damage() { die(); }

    void die() { board::current()->particles.explode(this, true); }

    virtual void trigger(thing *scapegoat);

    virtual char const * kind() const { return "enemy"; }
};

struct mini : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, DAMP, RESPONSIVENESS;
    static const boost::array<vec4, 6> colors;

    static boost::array<renders_with_pair, 2> renders_with_pairs;

    vec4 color;

    mini(vec2 ct)
        : enemy(ct, MASS, RADIUS, SPRING, DAMP, ACCEL, RESPONSIVENESS),
          color(colors[rand() % colors.size()]) { }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return color; }

    virtual char const * kind() const { return "mini"; }

    static thing *from_json(Json::Value const &v) { return sphere::from_json<mini>(v); }
};

struct mega : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, DAMP, RESPONSIVENESS;
    static const vec4 COLOR;

    static boost::array<renders_with_pair, 2> renders_with_pairs;

    mega(vec2 ct)
        : enemy(ct, MASS, RADIUS, SPRING, DAMP, ACCEL, RESPONSIVENESS) { }

    virtual char const * kind() const { return "mega"; }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return COLOR; }

    virtual void wall_damage() { }
    virtual void post_damage() { }

    static thing *from_json(Json::Value const &v) { return sphere::from_json<mega>(v); }
};

struct bumper : sphere {
    static const float RADIUS, MASS, INNER_RADIUS, SPRING, DAMP;
    static const vec4 INNER_COLOR, OUTER_COLOR;

    static boost::array<renders_with_pair, 2> renders_with_pairs_template;

    bumper(vec2 ct)
        : sphere(ct, MASS, RADIUS, SPRING, DAMP) { }

    virtual char const * kind() const { return "bumper"; }

    virtual renders_with_range renders_with() const;
    virtual vec4 sphere_color(float radius);

    static thing *from_json(Json::Value const &v) { return sphere::from_json<bumper>(v); }
};

struct switch_spring : sphere {
    static const float RADIUS, MASS, SLOT_LENGTH, SLOT_WIDTH, SPRING_FACTOR, DAMP;
    static const vec4 COLOR, TRIGGERED_COLOR, SLOT_COLOR;
    static const boost::array<vec2, 4> slot_vertices;
    
    static boost::array<renders_with_pair, 2> renders_with_pairs;

    vec2 home, axis;
    float slot_matrix[16];
    bool triggered;
    thing *last_touch;

    switch_spring(vec2 ct, vec2 ax)
        : sphere(ct - ax * SLOT_LENGTH, MASS, RADIUS, 0.0f, 0.0f),
          home(ct), axis(ax), triggered(false),
          last_touch(NULL)
        { _set_matrix(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;

    virtual vec4 sphere_color(float) { return triggered ? TRIGGERED_COLOR : COLOR; }

    virtual char const * kind() const { return "switch_spring"; }
    
    virtual void tick();
    virtual void on_collision(thing &o);

    static thing *from_json(Json::Value const &v);

private:
    void _set_matrix();
};

struct spawn : thing {
    thing *larva;

    spawn(thing *l) : thing(l->center, NO_COLLISION | DOES_TICKS), larva(l) {}

    virtual ~spawn() { if (larva) delete larva; }
    virtual void trigger(thing *scapegoat)
    {
        board::current()->add_thing(larva);
        board::current()->remove_thing(this);
        larva->trigger(scapegoat);
        larva = NULL;
    }
};

}

#endif
