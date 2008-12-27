#ifndef __TRANSITION_HPP__
#define __TRANSITION_HPP__

#include "controller.hpp"

namespace battlemints {

struct transition : controller {
    controller *from, *to;
    int age;

    transition(controller *f, controller *t)
        : from(f), to(t), age(0) { }

    virtual ~transition() { delete from; }

    virtual void setup();
    virtual void tick();
    virtual void draw();

    virtual bool retains_former_controller() const { return true; } 
};

}

#endif
