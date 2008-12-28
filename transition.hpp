#ifndef __TRANSITION_HPP__
#define __TRANSITION_HPP__

#include "controller.hpp"
#include "geom.hpp"

namespace battlemints {

struct transition : controller {
    controller *from, *to;
    unsigned slowdown, lifespan;
    vec4 color;
    int age;

    transition(controller *f, controller *t, unsigned s, unsigned l, vec4 c)
        : from(f), to(t), slowdown(s), lifespan(l), color(c), age(0) { }

    virtual ~transition() { delete from; }

    virtual void setup();
    virtual void tick();
    virtual void draw();

    virtual bool retains_former_controller() const { return true; } 
};

struct goal_transition : transition {
    goal_transition(controller *f, controller *t)
        : transition(f, t, 1, 60, make_vec4(1.0, 1.0, 1.0, 5.0)) { }
};

struct death_transition : transition {
    death_transition(controller *f, controller *t)
        : transition(f, t, 3, 120, make_vec4(1.0, 0.0, 0.0, 0.5)) { }
};

}

#endif
