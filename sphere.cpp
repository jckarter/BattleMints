#include "sphere.hpp"
#include "drawing.hpp"
#include "game.hpp"
#include <cmath>
#include <vector>
#include <boost/cstdint.hpp>

namespace battlemints {

rect sphere::visibility_box()
{
    vec2 diagonal = make_vec2(radius);
    return make_rect(center - diagonal, center + diagonal);
}

rect sphere::collision_box()
{
    vec2 diagonal = make_vec2(radius);
    vec2 neg = vmin(make_vec2(0.0), velocity);
    vec2 pos = vmax(make_vec2(0.0), velocity);

    return make_rect(center - diagonal + neg, center + diagonal + pos);
}

void sphere::draw()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(center.x, center.y, 0.0f);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glVertexPointer(2, GL_FLOAT, 0, (void*)&_vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&unit_texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glPopMatrix();
}

void sphere::_set_up_drawing()
{
    float border_radius = radius + BORDER_THICKNESS;
    unsigned pixel_radius = 1 << (unsigned)ilogbf(border_radius * PIXELS_PER_GAME_UNIT) + 1;

    _texture = _make_sphere_texture(border_radius, (unsigned)pixel_radius);

    _vertices[0] = -border_radius; _vertices[1] = -border_radius;
    _vertices[2] =  border_radius; _vertices[3] = -border_radius;
    _vertices[4] = -border_radius; _vertices[5] =  border_radius;
    _vertices[6] =  border_radius; _vertices[7] =  border_radius;
}

void sphere::_tear_down_drawing()
{
    glDeleteTextures(1, &_texture);
}

void sphere::_render_sphere_texture(float border_radius, unsigned pixel_radius, void *data)
{
    CGContextRef context = make_bitmap_context(pixel_radius*2, pixel_radius*2, data);

    float scale = pixel_radius/border_radius;

    CGContextScaleCTM(context, scale, scale);
    CGContextTranslateCTM(context, border_radius, border_radius);

    CGContextSetLineWidth(context, BORDER_THICKNESS);

    vec4 stroke_color = color*0.75 - make_vec4(0.5);

    CGContextSetRGBStrokeColor(context, stroke_color.x, stroke_color.y, stroke_color.z, 1.0);
    CGContextSetRGBFillColor(context, color.x, color.y, color.z, 1.0);

    CGRect ellipse_rect = CGRectMake(-radius, -radius, radius*2, radius*2);

    CGContextFillEllipseInRect(context, ellipse_rect);
    CGContextStrokeEllipseInRect(context, ellipse_rect);

    CGContextRelease(context);
}

GLuint sphere::_make_sphere_texture(float radius, unsigned pixel_radius)
{
    std::vector<uint32_t> pixmap((std::vector<uint32_t>::size_type)(pixel_radius*pixel_radius*4));

    void *pixmap_data = (void*)&pixmap[0];

    _render_sphere_texture(radius, pixel_radius, pixmap_data);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        pixel_radius*2, pixel_radius*2, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixmap_data
    );

    return texture;
}

}
