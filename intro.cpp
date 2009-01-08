#include "intro.hpp"
#include "board.hpp"
#include "game.hpp"
#include "drawing.hpp"
#include <CoreGraphics/CoreGraphics.h>
#include <boost/cstdint.hpp>

namespace battlemints {

intro::intro()
     : _splash_texture(image_texture::from_file("Default.png")), _countdown(20), _peak(20)
{ }

void
intro::setup()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(1.0, 1.0, 1.0, 1.0);
}

intro::~intro()
{
    delete _splash_texture;
}

void
intro::draw()
{
    float intensity = _intensity();
    float intensity_sqrt  = sqrtf(intensity);
    float intensity_atten = 0.125*(intensity_sqrt + intensity*7);

    glColor4f(intensity_sqrt, intensity_atten, intensity_atten, 1.0);
    glBindTexture(GL_TEXTURE_2D, _splash_texture->texture);

    glVertexPointer(2, GL_FLOAT, 0, (void*)&unit_radius_texcoords);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&_splash_texture->texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
intro::tick()
{
    if (_countdown == 0)
        controller::set_current(board::from_file("demo"));
    else
        --_countdown;
}

}
