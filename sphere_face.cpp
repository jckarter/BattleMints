#include "sphere_face.hpp"

namespace battlemints {

const float sphere_face::PANIC_SPIN_FACTOR = 5.0f, sphere_face::FACE_ROTATE_FACTOR = 1.0f;

inline model *
sphere_face::_model_for_course(vec2 velocity, vec2 accel)
{
    if (velocity == ZERO_VEC2 && accel == ZERO_VEC2)
        return asleep;

    float course = vdot(velocity, accel);
    float speed = vnorm(velocity), ideal = 0.75f*max_speed_for_accel(vnorm(accel));
    if (course < 0.0f)
        return speed > ideal ? panicked : strained;
    else
        return speed > ideal ? normal   : stressed;
}

static inline float _rotation(float magnitude) { return sphere_face::FACE_ROTATE_FACTOR*(1.0f - 1.0f/(magnitude + 1.0f)); }

void
sphere_face::draw_for_course(vec2 velocity, vec2 accel)
{
    glPushMatrix();

    model *mod = _model_for_course(velocity, accel);
    if (mod == panicked) {
        panic_spin += vnorm2(velocity) * PANIC_SPIN_FACTOR;
        glRotatef(panic_spin, 0.0, 1.0, 0.0);
    }
    else {
        panic_spin = 0.0f;
        glRotatef(_rotation(accel.y), 1.0, 0.0, 0.0);
        glRotatef(_rotation(accel.x), 0.0, 1.0, 0.0);
    }

    mod->draw();

    glPopMatrix();
}

sphere_face *
sphere_face::from_file_set(std::string const &name)
{
    return new sphere_face(
        model::from_file(name + "-asleep"),
        model::from_file(name + "-normal"),
        model::from_file(name + "-stressed"),
        model::from_file(name + "-strained"),
        model::from_file(name + "-panicked")
    );
}

}
