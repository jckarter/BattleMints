#include "drawing.hpp"
#include "game.hpp"
#include <UIKit/UIKit.h>
#include <CoreGraphics/CoreGraphics.h>
#include <iostream>

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

sphere_texture::sphere_texture(float radius)
{
    float border_radius = radius + BORDER_THICKNESS;
    unsigned pixel_radius = lpot(border_radius * PIXELS_PER_GAME_UNIT);

    texture = _make_sphere_texture(radius, border_radius, (unsigned)pixel_radius);

    vertices[0] = -border_radius; vertices[1] = -border_radius;
    vertices[2] =  border_radius; vertices[3] = -border_radius;
    vertices[4] = -border_radius; vertices[5] =  border_radius;
    vertices[6] =  border_radius; vertices[7] =  border_radius;
}

void sphere_texture::_render_sphere_texture(float radius, float border_radius, unsigned pixel_radius, void *data)
{
    CGContextRef context = make_bitmap_context(pixel_radius*2, pixel_radius*2, data);

    float scale = pixel_radius/border_radius;

    CGContextScaleCTM(context, scale, scale);
    CGContextTranslateCTM(context, border_radius, border_radius);

    CGContextSetLineWidth(context, BORDER_THICKNESS);

    CGContextSetRGBStrokeColor(context, 0.25f, 0.25f, 0.25f, 1.0f);
    CGContextSetRGBFillColor(context, 1.0f, 1.0f, 1.0f, 1.0f);

    CGRect ellipse_rect = CGRectMake(-radius, -radius, radius*2.0f, radius*2.0f);

    CGContextFillEllipseInRect(context, ellipse_rect);
    CGContextStrokeEllipseInRect(context, ellipse_rect);

    CGContextRelease(context);
}

GLuint sphere_texture::_make_sphere_texture(float radius, float border_radius, unsigned pixel_radius)
{
    std::vector<uint32_t> pixmap((std::vector<uint32_t>::size_type)(pixel_radius*pixel_radius*4));

    void *pixmap_data = (void*)&pixmap[0];

    _render_sphere_texture(radius, border_radius, pixel_radius, pixmap_data);

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

image_texture::image_texture(CGImageRef image)
{
    unsigned real_width = CGImageGetWidth(image), real_height = CGImageGetHeight(image),
             texture_width = lpot(real_width), texture_height = lpot(real_height);
    std::vector<uint32_t> pixmap(texture_width*texture_height);
    void *pixmap_data = (void*)&pixmap[0];

    CGContextRef context = make_bitmap_context(texture_width, texture_height, pixmap_data);
    CGContextDrawImage(context, CGRectMake(0, 0, real_width, real_height), image);
    CGContextRelease(context);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
        texture_width, texture_height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, pixmap_data
    );

    float far_x = (float)real_width/(float)texture_width,
          far_y = (float)(texture_height-real_height)/(float)texture_height;

    texcoords[0] = 0.0;   texcoords[1] = 1.0;
    texcoords[2] = far_x; texcoords[3] = 1.0;
    texcoords[4] = 0.0;   texcoords[5] = far_y;
    texcoords[6] = far_x; texcoords[7] = far_y;
}

image_texture *
image_texture::from_file(std::string const &name)
{
    CGImageRef image = make_image(name.c_str());
    finally<CGImageRef> release_image(image, CGImageRelease);
    image_texture *r =  new image_texture(image);
    return r;
}

GLuint sphere_face::array_buffer;

void sphere_face::global_start()
{
    struct {
        boost::array<float, MESH_VERTICES*3> vertices;
        boost::array<float, MESH_VERTICES*2> texcoords;
    } b;
    static const float HEIGHT = 0.25*M_PI;

    for (unsigned i = 0; i <= MESH_RESOLUTION; ++i) {
        float theta = (float)i/(float)MESH_RESOLUTION;
        b.vertices[i*6  ] = b.vertices[i*6+3] = fast_cos_2pi(-0.25f + 0.5*theta);
        b.vertices[i*6+1] = HEIGHT; b.vertices[i*6+4] = -HEIGHT;
        b.vertices[i*6+2] = b.vertices[i*6+5] = fast_sin_2pi(-0.25f + 0.5*theta);

        b.texcoords[i*4  ] = b.texcoords[i*4+2] = theta;
        b.texcoords[i*4+1] = 0.0f; b.texcoords[i*4+3] = 1.0f;
    }

    glGenBuffers(1, &array_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, array_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(b), &b, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void sphere_face::global_finish()
{
    glDeleteBuffers(1, &array_buffer);
}

sphere_face::state
sphere_face::state_for_course(vec2 velocity, vec2 accel)
{
    if (accel == ZERO_VEC2)
        return asleep;

    float course = vdot(velocity, accel);
    float speed = vnorm(velocity), ideal = 0.75f*max_speed_for_accel(vnorm(accel));
    if (course < 0.0f)
        return speed > ideal ? panicked : strained;
    else
        return speed > ideal ? normal   : stressed;
}

float sphere_face::rotation(float magnitude)
{
    return sphere_face::ROTATE_SPAN*(1.0f - 1.0f/(sphere_face::ROTATE_FACTOR*magnitude + 1.0f));
}

namespace {

void _glyph_row_col(char c, int &out_row, int &out_col)
{
    c -= ' ';
    out_row = c >> 4;
    out_col = c & 15;
}

}

const vec2 font::GLYPH_TEXCOORD_PITCH = make_vec2(8.0/128.0, 16.0/128.0),
           font::GLYPH_TEXCOORD_SIZE  = make_vec2(6.0/128.0, 13.0/128.0),
           font::GLYPH_VERTEX_SIZE    = make_vec2(12.0, 26.0);

font::font(CGImageRef image) : image_texture(image)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

font *
font::from_file(std::string const &name)
{
    CGImageRef image = make_image((name + ".png").c_str());
    finally<CGImageRef> release_image(image, CGImageRelease);
    font *r =  new font(image);

    return r;
}

void
font::draw_string(std::string const &s)
{
    std::vector<vertex> vertices(4);

    glVertexPointer(2, GL_FLOAT, sizeof(vertex), (void*)&vertices[0].center);
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (void*)&vertices[0].texcoord);

    int i = 0;
    std::string::const_iterator si = s.begin();
    for (; si != s.end(); ++i, ++si) {
        int row, col;
        _glyph_row_col(*si, row, col);
        
        vertices[0] = (vertex){
            make_vec2(i  *GLYPH_VERTEX_SIZE.x,   0.0f),
            make_vec2(
                col*GLYPH_TEXCOORD_PITCH.x,
                row*GLYPH_TEXCOORD_PITCH.y + GLYPH_TEXCOORD_SIZE.y
            )
        };
        vertices[1] = (vertex){
            make_vec2((i+1)*GLYPH_VERTEX_SIZE.x,   0.0f),
            make_vec2(
                col*GLYPH_TEXCOORD_PITCH.x + GLYPH_TEXCOORD_SIZE.x,
                row*GLYPH_TEXCOORD_PITCH.y + GLYPH_TEXCOORD_SIZE.y
            )
        };
        vertices[2] = (vertex){
            make_vec2(i  *GLYPH_VERTEX_SIZE.x,   GLYPH_VERTEX_SIZE.y),
            make_vec2(
                col*GLYPH_TEXCOORD_PITCH.x,
                row*GLYPH_TEXCOORD_PITCH.y
            )
        };
        vertices[3] = (vertex){
            make_vec2((i+1)*GLYPH_VERTEX_SIZE.x,   GLYPH_VERTEX_SIZE.y),
            make_vec2(
                col*GLYPH_TEXCOORD_PITCH.x + GLYPH_TEXCOORD_SIZE.x,
                row*GLYPH_TEXCOORD_PITCH.y
            )
        };
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
}

}

