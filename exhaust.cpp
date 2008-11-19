#include "exhaust.hpp"
#include "board.hpp"

namespace battlemints {

static const unsigned EXHAUST_LIFE_EXPECTANCY = 60;

const boost::array<float, 8> exhaust::vertices = {
      0.0,   4.0,
     -9.0,  24.0,
      0.0, - 4.0,
     -9.0, -24.0
};

rect exhaust::visibility_box()
{
    return make_rect(center, center);
}

void exhaust::draw()
{
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    vec2 d = direction * (float)age/(float)EXHAUST_LIFE_EXPECTANCY;
    vec2 n = vperp(d);
    glTranslatef(center.x, center.y, 0.0);
    boost::array<float, 16> matrix = {
        d.x, d.y, 0.0, 0.0,
        n.x, n.y, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    };
    glMultMatrixf((GLfloat*)&matrix);

    vec4 color = _color(age);
    glColor4f(color.x, color.y, color.z, color.w);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
    glDrawArrays(GL_LINES, 0, 4);

    glPopMatrix();
}

void exhaust::tick()
{
    ++age;
    if (age > EXHAUST_LIFE_EXPECTANCY)
        board::current()->remove_thing(this);
}

vec4 exhaust::_color(unsigned age)
{
    float a = 1.0 - (float)age/(float)EXHAUST_LIFE_EXPECTANCY;
    float a2 = sqrtf(a);

    return make_vec4(a2, (a+a2)*0.5, a, a*a);
}

}
