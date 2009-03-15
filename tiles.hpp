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
        : thing(ZERO_VEC2, NO_COLLISION), vertices(new std::vector<vertex>), buffer(0)
        { }

    virtual ~tile_vertices();

    virtual void awaken();

    void bind() const;
    void unbind() const;

    tile_vertices(FILE *bin)
        : thing(NO_COLLISION), vertices(NULL)
    {
        int length = data_from_bin<int>(bin);
        vertices = new std::vector<vertex>(length);
        safe_fread(&(*vertices)[0], sizeof(int), length, bin);
    }

private:
    static GLuint _buffer_for_vertices(std::vector<vertex> const &v);
};

struct tile : thing {
    struct vertex_range {
        int begin;
        int size;

        int end() const { return begin + size; }

        vertex_range() {}
        vertex_range(int s, int l) : begin(s), size(l) {}
    };

    vertex_range vertices;

    tile(vec2 center, GLint start, GLsizei length)
        : thing(center, NO_COLLISION), vertices(start, length) { }

    virtual renders_with_range renders_with() const
        { return tile_renderer::instance_null_arg_range; }

    tile(FILE *bin)
        : thing(NO_COLLISION, bin)
        { BATTLEMINTS_READ_SLOTS(*this, vertices, vertices, bin); }
};

}

#endif
