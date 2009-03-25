#ifndef __DRAWING_HPP__
#define __DRAWING_HPP__

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>
#include "geom.hpp"

namespace battlemints {

CGContextRef make_bitmap_context(unsigned w, unsigned h, void *data);
CGImageRef make_image(char const *filename);

struct model : boost::noncopyable {
    GLuint vertex_buffer, element_buffer;
    unsigned num_vertices, num_elements;

    model(std::vector<GLfloat> const &vertices, std::vector<GLushort> const &elements);
    ~model();

    void draw() const;

    static model *from_file(std::string const &name);
};

extern const boost::array<float, 8> unit_texcoords;
extern const boost::array<float, 8> unit_radius_texcoords;

struct gl_texture : boost::noncopyable {
    GLuint texture;
    gl_texture() : texture(0) { }
    gl_texture(GLuint t) : texture(t) { }
    virtual ~gl_texture() { glDeleteTextures(1, &texture); }

    static unsigned lpot(float f) { return 1 << (unsigned)ilogbf(f) + 1; }
    static unsigned lpot(unsigned i)
        { i|=--i>>1; i|=i>>2; i|=i>>4; i|=i>>8; i|=i>>16; return i+1; }
};

struct sphere_texture : gl_texture {
    boost::array<GLfloat, 8> vertices;

    sphere_texture(float radius);

private:
    void _render_sphere_texture(float radius, float border_radius, unsigned pixel_radius, void *data);
    GLuint _make_sphere_texture(float radius, float border_radius, unsigned pixel_radius);
};

struct image_texture : gl_texture {
    boost::array<GLfloat, 8> texcoords;
    
    image_texture(CGImageRef image);

    static image_texture *from_file(std::string const &name);
};

struct sphere_face : boost::noncopyable {
    static const float ROTATE_SPAN, ROTATE_FACTOR;
    static const unsigned MESH_RESOLUTION = 5;
    static const unsigned MESH_VERTICES = (MESH_RESOLUTION+1) * 2;

    static GLuint array_buffer;

    enum state { asleep = 0, normal, stressed, strained, panicked };

    image_texture *texture;

    sphere_face(image_texture *t) : texture(t) { }
    ~sphere_face() { delete texture; }

    static void global_start();
    static void global_finish();

    static sphere_face *from_file(std::string const &name)
        { return new sphere_face(image_texture::from_file(name + ".png")); }

    static state state_for_course(vec2 velocity, vec2 accel);
    static float rotation(float magnitude);
};

struct font : image_texture {
    static const vec2 GLYPH_TEXCOORD_PITCH, GLYPH_TEXCOORD_SIZE;
    static const vec2 GLYPH_VERTEX_SIZE;
    static font *from_file(std::string const &name);

    struct vertex {
        vec2 center;
        vec2 texcoord;
    };

    font(CGImageRef image) : image_texture(image) { }
    
    static void draw_string(std::string const &s);
};

}

#endif
