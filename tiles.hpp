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
        : thing(INFINITYF, ZERO_VEC2, 1.0f), vertices(new std::vector<vertex>), buffer(0)
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
    GLint vertex_start;
    GLsizei vertex_length;

    tile(vec2 center, GLint s, GLsizei l)
        : thing(INFINITYF, center, 1.0f), vertex_start(s), vertex_length(l) { }

    virtual bool does_collisions() const { return false; }

    static thing *from_json(Json::Value const &v);

    void draw();
};

}

#endif
