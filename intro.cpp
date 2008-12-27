#include "intro.hpp"
#include "board.hpp"
#include "game.hpp"
#include "drawing.hpp"
#include <CoreGraphics/CoreGraphics.h>
#include <boost/cstdint.hpp>

namespace battlemints {

const boost::array<float, 8> intro::texcoords = {
    0.0,           1.0,
    320.0 / 512.0, 1.0,
    0.0,           (512.0-480.0)/512.0,
    320.0 / 512.0, (512.0-480.0)/512.0
};

intro::intro()
     : _splash_texture(_load_splash_texture()), _countdown(20), _peak(20)
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
    glDeleteTextures(1, &_splash_texture);
}

void
intro::draw()
{
    float intensity = _intensity();
    float intensity_sqrt  = sqrtf(intensity);
    float intensity_atten = 0.125*(intensity_sqrt + intensity*7);

    glColor4f(intensity_sqrt, intensity_atten, intensity_atten, 1.0);
    glBindTexture(GL_TEXTURE_2D, _splash_texture);

    glVertexPointer(2, GL_FLOAT, 0, (void*)&unit_radius_texcoords);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&texcoords);
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

GLuint
intro::_load_splash_texture()
{
    std::vector<uint32_t> pixmap(512*512);
    void *pixmap_data = (void*)&pixmap[0];

    CGContextRef context = make_bitmap_context(512, 512, pixmap_data);
    CGImageRef image = make_image("Default.png");
    CGContextDrawImage(context, CGRectMake(0, 0, 320, 480), image);
    CGContextRelease(context);
    CGImageRelease(image);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        512, 512, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixmap_data
    );

    return texture;
}

}
