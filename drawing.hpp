#ifndef __DRAWING_HPP__
#define __DRAWING_HPP__

#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#include <CoreGraphics/CoreGraphics.h>
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <string>
#include <vector>
#include "thing.hpp"

namespace battlemints {

CGContextRef make_bitmap_context(unsigned w, unsigned h, void *data);
CGImageRef   make_image(char const *filename);

struct model : boost::noncopyable {
    GLuint vertex_buffer, element_buffer;
    unsigned num_vertices, num_elements;

    model(std::vector<GLfloat> const &vertices, std::vector<GLushort> const &elements);
    ~model();

    void draw() const;
};

struct sphere_texture : boost::noncopyable {
    static const boost::array<float, 8> unit_texcoords;

    GLuint texture;
    boost::array<GLfloat, 8> vertices;

    sphere_texture(float radius, vec4 color);
    ~sphere_texture();

    void draw() const;

private:
    void _render_sphere_texture(float radius, float border_radius, unsigned pixel_radius, vec4 color, void *data);
    GLuint _make_sphere_texture(float radius, float border_radius, unsigned pixel_radius, vec4 color);
};

}

#endif
