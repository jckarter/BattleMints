#include "tiles.hpp"
#include "board.hpp"
#include <cstddef>

namespace battlemints {

tile_vertices::~tile_vertices()
{
    if (buffer != 0)
        glDeleteBuffers(1, &buffer);
    if (vertices != NULL)
        delete vertices;
}

void
tile_vertices::awaken()
{
    buffer = _buffer_for_vertices(*vertices);
    delete vertices;
    vertices = NULL;

    board::current()->tile_vertices_thing = this;
}

void
tile_vertices::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);    

    glVertexPointer(2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, center));
    glColorPointer(4, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, color));
}

void
tile_vertices::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableClientState(GL_COLOR_ARRAY);
}

thing *
tile_vertices::from_json(Json::Value const &v)
{
    tile_vertices *vertices_thing = new tile_vertices;

    Json::Value const &vertices_json = v["vertices"];
    if (!vertices_json.isArray())
        throw invalid_board_json("tile_vertices must have a vertices field that is an array");
    for (int i = 0; i < vertices_json.size(); ++i)
        vertices_thing->vertices->push_back(_vertex_from_json(vertices_json[i]));

    return vertices_thing;
}

GLuint
tile_vertices::_buffer_for_vertices(std::vector<vertex> const &v)
{
    GLuint b;
    glGenBuffers(1, &b);
    glBindBuffer(GL_ARRAY_BUFFER, b);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(vertex), &v[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return b;
}

thing *
tile::from_json(Json::Value const &v)
{
    vec2 center = vec2_from_json(v["center"]);
    GLint vertex_start = v["vertex_start"].asInt();
    GLsizei vertex_length = v["vertex_length"].asUInt();

    return new tile(center, vertex_start, vertex_length);
}

}
