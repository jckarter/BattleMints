#ifndef __TILES_HPP__
#define __TILES_HPP__

#include "thing.hpp"
#include <boost/array.hpp>
#include <OpenGLES/ES1/gl.h>

namespace battlemints {

struct tile : thing {
    GLfloat transform[16];

    tile(float xx, float xy, float yx, float yy, float ox, float oy)
        : thing(INFINITYF, make_vec2(ox, oy), 1.0f) {

        transform[ 0] = xx;  transform[ 1] = xy;  transform[ 2] = 0.0; transform[ 3] = 0.0;
        transform[ 4] = yx;  transform[ 5] = yy;  transform[ 6] = 0.0; transform[ 7] = 0.0;
        transform[ 8] = 0.0; transform[ 9] = 0.0; transform[10] = 1.0; transform[11] = 0.0;
        transform[12] = ox;  transform[13] = oy;  transform[14] = 0.0; transform[15] = 1.0;
    }

    virtual bool does_collisions() const { return false; }

protected:
    template<typename Tile>
    static thing *from_json(Json::Value const &v)
    {
        Json::Value const &transform = v["transform"];
        if (!transform.isArray() && v.size() != 6)
            throw invalid_board_json("transform value must be an array of 6 elements");
        return new Tile(
            transform[0u].asDouble(),
            transform[1u].asDouble(),
            transform[2u].asDouble(),
            transform[3u].asDouble(),
            transform[4u].asDouble(),
            transform[5u].asDouble()
        );
    }

    template<typename Tile>
    void _draw()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(transform);

        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glColor4f(Tile::tile_color.x, Tile::tile_color.y, Tile::tile_color.z, Tile::tile_color.w);
        glVertexPointer(2, GL_FLOAT, 0, (void*)&Tile::tile_vertices);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, Tile::tile_vertices.static_size);
    };
};

#define TILE_STRUCT(name, vertices) \
    struct name : tile { \
        static const vec4 tile_color; \
        static const boost::array<vec2, vertices> tile_vertices; \
        name(float xx, float xy, float yx, float yy, float ox, float oy) \
            : tile(xx, xy, yx, yy, ox, oy) { } \
        static thing *from_json(Json::Value const &v) { return tile::from_json<name>(v); } \
        virtual void draw() { tile::_draw<name>(); } \
    }

TILE_STRUCT(tile_octagon,    8);
TILE_STRUCT(tile_hexagon_90, 6);
TILE_STRUCT(tile_hexagon,    6);
TILE_STRUCT(tile_trapezoid,  4);
TILE_STRUCT(tile_square,     4);
TILE_STRUCT(tile_rhombus_60, 4);
TILE_STRUCT(tile_rhombus_45, 4);
TILE_STRUCT(tile_rhombus_30, 4);
TILE_STRUCT(tile_triangle,   3);
TILE_STRUCT(arrow,           5);

}

#endif
