#ifndef __FACE_HPP__
#define __FACE_HPP__

#include "drawing.hpp"
#include <boost/utility.hpp>

namespace battlemints {

struct sphere_face : boost::noncopyable {
    static const float PANIC_SPIN_FACTOR, ROTATE_SPAN, ROTATE_FACTOR;

    model *asleep, *normal, *stressed, *strained, *panicked;
    float panic_spin;

    sphere_face(model *a, model *n, model *se, model *sa, model *p)
        : asleep(a), normal(n), stressed(se), strained(sa), panicked(p), panic_spin(0.0f) { }
    ~sphere_face() { delete asleep; delete normal; delete stressed; delete strained; delete panicked; }

    void draw_for_course(vec2 velocity, vec2 accel);

    static sphere_face *from_file_set(std::string const &name);

private:
    model *_model_for_course(vec2 velocity, vec2 accel);
};

}

#endif
