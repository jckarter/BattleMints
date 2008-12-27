#include "drawing.hpp"
#include "game.hpp"
#include <UIKit/UIKit.h>
#include <CoreGraphics/CoreGraphics.h>

namespace battlemints {

CGContextRef make_bitmap_context(unsigned w, unsigned h, void *data)
{
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(
        data, w, h, 8, w*4,
        colorspace, kCGImageAlphaPremultipliedLast
    );
    CGColorSpaceRelease(colorspace);
    return context;
}

CGImageRef make_image(char const *filename)
{
    UIImage *ui_image = [UIImage imageNamed:[NSString stringWithUTF8String:filename]];
    CGImageRef ret = [ui_image CGImage];
    CGImageRetain(ret);
    return ret;
}

model::model(std::vector<GLfloat> const &vertices, std::vector<GLushort> const &elements)
    : num_vertices(vertices.size()), num_elements(elements.size())
{
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_elements * sizeof(GLushort), &elements[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

model::~model()
{
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &element_buffer);
}

void
model::draw() const
{
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    glDrawElements(GL_TRIANGLES, num_elements, GL_UNSIGNED_SHORT, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

const boost::array<float, 8> unit_texcoords = {
    0.0, 0.0,
    1.0, 0.0,
    0.0, 1.0,
    1.0, 1.0
};

const boost::array<float, 8> unit_radius_texcoords = {
    -1.0, -1.0,
     1.0, -1.0,
    -1.0,  1.0,
     1.0,  1.0
};

sphere_texture::sphere_texture(float radius, vec4 color)
{
    float border_radius = radius + BORDER_THICKNESS;
    unsigned pixel_radius = 1 << (unsigned)ilogbf(border_radius * PIXELS_PER_GAME_UNIT) + 1;

    texture = _make_sphere_texture(radius, border_radius, (unsigned)pixel_radius, color);

    vertices[0] = -border_radius; vertices[1] = -border_radius;
    vertices[2] =  border_radius; vertices[3] = -border_radius;
    vertices[4] = -border_radius; vertices[5] =  border_radius;
    vertices[6] =  border_radius; vertices[7] =  border_radius;
}

sphere_texture::~sphere_texture()
{
    glDeleteTextures(1, &texture);
}

void sphere_texture::draw() const
{
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, texture);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&unit_texcoords);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void sphere_texture::_render_sphere_texture(float radius, float border_radius, unsigned pixel_radius, vec4 color, void *data)
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

GLuint sphere_texture::_make_sphere_texture(float radius, float border_radius, unsigned pixel_radius, vec4 color)
{
    std::vector<uint32_t> pixmap((std::vector<uint32_t>::size_type)(pixel_radius*pixel_radius*4));

    void *pixmap_data = (void*)&pixmap[0];

    _render_sphere_texture(radius, border_radius, pixel_radius, color, pixmap_data);

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        pixel_radius*2, pixel_radius*2, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixmap_data
    );

    return texture;
}

}
