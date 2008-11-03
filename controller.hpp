#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include <boost/utility.hpp>

namespace battlemints {

struct controller : boost::noncopyable {
    virtual ~controller() {}

    virtual void setup() = 0;
    virtual void tick() = 0;
    virtual void draw() = 0;

    static controller *current() { return _current; }
    static void set_current(controller *next)
        { if (!_current) _activate_current(next); else _next = next; }

    static void delete_current() { delete _current; _current = NULL; }

    static void tick_current()
    {
        if (_next) {
            _activate_current(_next);
            _next = NULL;
        }

        _current->tick();
    }
    static void draw_current()
    {
        _current->draw();
    }

private:
    static void _activate_current(controller *current)
        { delete _current; _current = current; _current->setup(); }

    static controller *_current, *_next;
};

}

#endif
