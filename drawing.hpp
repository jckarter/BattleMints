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

struct sphere_texture : boost::noncopyable {
    GLuint texture;
    boost::array<GLfloat, 8> vertices;

    sphere_texture(float radius, vec4 color);
    ~sphere_texture();

    void draw() const;

private:
    void _render_sphere_texture(float radius, float border_radius, unsigned pixel_radius, vec4 color, void *data);
    GLuint _make_sphere_texture(float radius, float border_radius, unsigned pixel_radius, vec4 color);
};

struct sphere_face : boost::noncopyable {
    static const float PANIC_SPIN_FACTOR, ROTATE_SPAN, ROTATE_FACTOR;

    model *asleep, *normal, *stressed, *strained, *panicked;
    float panic_spin;

    sphere_face(model *a, model *n, model *se, model *sa, model *p)
        : asleep(a), normal(n), stressed(se), strained(sa), panicked(p), panic_spin(0.0f) { }
    ~sphere_face() { delete asleep; delete normal; delete stressed; delete strained; delete panicked; }

    void draw_for_course(vec2 velocity, vec2 accel);

    static sphere_face *from_file_set(std::string const &name);

private:
    model *_model_for_course(vec2 velocity, vec2 accel);
};

}

#endif
