#ifndef __TILES_HPP__
#define __TILES_HPP__

#include "thing.hpp"
#include <vector>
#include <boost/array.hpp>
#include <OpenGLES/ES1/gl.h>

namespace battlemints {

struct tile_vertices : thing {
    struct vertex {
        vec2 center;
        vec4 color;
    };

    std::vector<vertex> *vertices;
    GLuint buffer;

    tile_vertices()
        : thing(INFINITYF, ZERO_VEC2, 0.0f), vertices(new std::vector<vertex>), buffer(0)
        { }

    virtual ~tile_vertices();

    virtual bool does_collisions() const { return false; }
    virtual void awaken();

    void bind() const;
    void unbind() const;

    static thing *from_json(Json::Value const &v);

private:
    static GLuint _buffer_for_vertices(std::vector<vertex> const &v);
};

struct tile : thing {
    struct vertex_range {
        GLint begin;
        GLsizei size;

        GLint end() const { return begin + size; }

        vertex_range() {}
        vertex_range(GLint s, GLsizei l) : begin(s), size(l) {}
    };

    vertex_range vertices;

    tile(vec2 center, GLint start, GLsizei length)
        : thing(INFINITYF, center, 0.0f), vertices(start, length) { }

    virtual bool does_collisions() const { return false; }

    static thing *from_json(Json::Value const &v);

    virtual renders_with_range renders_with() const
        { return tile_renderer::instance_null_arg_range; }
};

}

#endif
