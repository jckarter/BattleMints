#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "drawing.hpp"
#include "transition.hpp"

namespace battlemints {

static const int TRANSITION_FADE_OUT = 60;

void
transition::setup()
{
    age = 0;
}

void
transition::tick()
{
    from->tick();
    ++age;

    if (age == TRANSITION_FADE_OUT)
        controller::set_current(to);
}

void
transition::draw()
{
    from->draw();

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glColor4f(1.0f, 1.0f, 1.0f, 5.0f*(float)age/(float)TRANSITION_FADE_OUT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glVertexPointer(2, GL_FLOAT, 0, (void*)&unit_radius_texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
}

}
