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
    static const float ACCEL_SCALE, RADIUS, MASS, SPRING, DAMP;
    static const float SHIELD_RADIUS, SHIELD_SPRING;
    static const float INVULN_SPRING, INVULN_MASS, INVULN_DAMP;
    static const float PANIC_SPRING, PANIC_MASS, PANIC_RADIUS;
    static const vec4 COLOR, SHIELD_COLOR, INVULN_BODY_COLOR, PANIC_COLOR;

    static const int
        INVULN_PELLET_BURN = 30,
        PANIC_CHARGE = 30*60,
        PANIC_TAP_COUNT = 5,
        PANIC_TIME = 3,
        MAX_PELLET_SPILL = 20;

    static boost::array<renders_with_pair, 3> renders_with_pairs_template;
    static boost::array<renders_with_pair, 3> invuln_renders_with_pairs;

    static const boost::array<vec4, 6> invuln_colors;

    bool shielded, invuln;
    int panicked, grace_period, panic_charge;
    int pellets, pellet_burn, pellet_grace_period;

    player(vec2 center)
        : sphere(center, MASS, RADIUS, SPRING, DAMP, PLAYER),
          shielded(false), invuln(false), panicked(0),
          grace_period(0), panic_charge(0), pellets(0), pellet_burn(0), pellet_grace_period(0)
        { }

    virtual void tick();

    virtual char const * kind() const { return "player"; }

    virtual void wall_damage() { damage(); }
    virtual void post_damage() { damage(); }

    virtual renders_with_range renders_with() const;
    virtual vec4 sphere_color(float r);

    void damage();
    void die();
    void lose_shield();
    void gain_shield();
    void lose_invuln();
    void gain_invuln();
    void panic();
    void lose_pellets();

    void update_stats();

    player(FILE *bin)
        : sphere(PLAYER, bin, MASS, RADIUS, SPRING, DAMP),
          shielded(false), invuln(false), panicked(0),
          grace_period(0), panic_charge(0), pellets(0), pellet_burn(0), pellet_grace_period(0)
        { }
};

struct powerup : sphere {
    static const float RADIUS, MASS, SPRING, DAMP;
    static const vec4 CHARGED_COLOR, DEAD_COLOR;
    static const int CHARGE_TIME = 300;

    static boost::array<renders_with_pair, 1> renders_with_pairs;

    static const boost::array<vec4, 2> pulse_colors;

    enum kind_name {
        shield = 0,
        invuln
    };

    unsigned charge_time;
    int powerup_kind;

    powerup(vec2 center, int k)
        : sphere(center, MASS, RADIUS, SPRING, DAMP), powerup_kind(k), charge_time(0) { }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float);

    virtual void tick();
    virtual void on_collision(thing &o);

    void give_powerup(player *p);

    virtual char const * kind() const { return "powerup"; }

    powerup(FILE *bin)
        : sphere(0, bin, MASS, RADIUS, SPRING, DAMP), charge_time(0)
        { BATTLEMINTS_READ_SLOTS(*this, powerup_kind, powerup_kind, bin); }

    virtual void print(std::ostream &os) const
        { sphere::print(os); os << " kind:" << powerup_kind; }
};

struct pellet : point {
    static const float RADIUS;
    static const boost::array<vec4, 6> colors;

    static boost::array<renders_with_pair, 1> renders_with_pairs;

    pellet(vec2 center) : point(center, DOES_TICKS | CAN_OVERLAP) { }

    virtual void on_collision(thing &o);

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    virtual vec4 sphere_color(float)
        { return colors[board::current()->tick_count() % colors.size()]; }

    pellet(FILE *bin) : point(DOES_TICKS | CAN_OVERLAP, bin) {}
};

struct loose_pellet : sphere {
    static const float MASS, SPRING, DAMP, MIN_SPEED, MAX_SPEED;

    static const int LIFETIME = 5*60;

    int lifetime;

    loose_pellet(vec2 center)
        : sphere(center, MASS, pellet::RADIUS, SPRING, DAMP), lifetime(LIFETIME) { }

    virtual void tick();

    virtual void on_collision(thing &o);

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(pellet::renders_with_pairs.begin(), pellet::renders_with_pairs.end()); }

    virtual vec4 sphere_color(float)
        { return pellet::colors[board::current()->tick_count() % pellet::colors.size()]; }

    static loose_pellet *spawn(sphere const &from);
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

    virtual void die() { board::current()->particles.explode(this, true); }

    virtual void trigger(thing *scapegoat);

    virtual char const * kind() const { return "enemy"; }

protected:
    enemy(int flags, FILE *bin, float m, float r, float b, float d, float a, float re)
        : sphere(flags, bin, m, r, b, d), accel(a), responsiveness(re),
          target(NULL) {}
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

    virtual void die() { loose_pellet::spawn(*this); enemy::die(); }

    mini(FILE *bin)
        : enemy(0, bin, MASS, RADIUS, SPRING, DAMP, ACCEL, RESPONSIVENESS),
          color(colors[rand() % colors.size()]) { }
};

struct mega : enemy {
    static const float ACCEL, RADIUS, MASS, SPRING, DAMP, RESPONSIVENESS, DEATH_THRESHOLD2;
    static const vec4 COLOR;

    static boost::array<renders_with_pair, 2> renders_with_pairs;

    mega(vec2 ct)
        : enemy(ct, MASS, RADIUS, SPRING, DAMP, ACCEL, RESPONSIVENESS) { }

    virtual char const * kind() const { return "mega"; }

    virtual void die() { for (int i = 0; i < 10; ++i) loose_pellet::spawn(*this); enemy::die(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual vec4 sphere_color(float) { return COLOR; }

    void damage() { if (vnorm2(velocity) > DEATH_THRESHOLD2) die(); }

    virtual void wall_damage() { damage(); }
    virtual void post_damage() { damage(); }

    mega(FILE *bin)
        : enemy(0, bin, MASS, RADIUS, SPRING, DAMP, ACCEL, RESPONSIVENESS) { }
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

    bumper(FILE *bin) : sphere(0, bin, MASS, RADIUS, SPRING, DAMP) { }

public:
    static thing *from_bin(FILE *bin)
        { return new bumper(bin); }
};

struct switch_spring : sphere {
    static const float RADIUS, MASS, SLOT_LENGTH, SLOT_WIDTH, SPRING_FACTOR, DAMP;
    static const vec4 COLOR, TRIGGERED_COLOR, SLOT_COLOR;
    static const boost::array<vec2, 4> slot_vertices;
    
    static boost::array<renders_with_pair, 2> renders_with_pairs;

    vec2 axis, home;
    float slot_matrix[16];
    bool triggered;
    thing *last_touch;

    switch_spring(vec2 ct, vec2 ax)
        : sphere(ct - ax * SLOT_LENGTH, MASS, RADIUS, 0.0f, 0.0f),
          axis(ax), home(ct), triggered(false),
          last_touch(NULL)
        { _set_matrix(); }

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }
    virtual void draw_self() const;

    virtual vec4 sphere_color(float) { return triggered ? TRIGGERED_COLOR : COLOR; }

    virtual char const * kind() const { return "switch_spring"; }
    
    virtual void tick();
    virtual void on_collision(thing &o);

    switch_spring(FILE *bin)
        : sphere(0, bin, MASS, RADIUS, 0.0f, 0.0f), triggered(false), last_touch(NULL)
    {
        BATTLEMINTS_READ_SLOTS(*this, axis, axis, bin);
        home = center; center -= axis * SLOT_LENGTH;
        prev_center = center;
        _set_matrix();
    }

    virtual void print(std::ostream &os) const
        { thing::print(os); os << " axis:" << axis; }

private:
    void _set_matrix();
};

struct spawn : thing {
    thing *larva;

    spawn(thing *l)
        : thing(l->center, NO_COLLISION | DOES_TICKS), larva(l)
        { label = larva->label; }

    virtual ~spawn() { if (larva) delete larva; }
    virtual void trigger(thing *scapegoat)
    {
        board::current()->add_thing(larva);
        board::current()->remove_thing(this);
        larva->trigger(scapegoat);
        larva = NULL;
    }

    virtual char const * kind() const { return "spawn"; }

    virtual void print(std::ostream &os) const
        { thing::print(os); os << " larva:(" << *larva << ")"; }
};

}

#endif
