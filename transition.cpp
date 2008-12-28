#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include "drawing.hpp"
#include "transition.hpp"

namespace battlemints {

void
transition::setup()
{
    age = 0;
}

void
transition::tick()
{
    ++age;
    if (age % slowdown == 0)
        from->tick();

    if (age == lifespan)
        controller::set_current(to);
}

void
transition::draw()
{
    from->draw();

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glColor4f(color.x, color.y, color.z, color.w*(float)age/(float)lifespan);
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
