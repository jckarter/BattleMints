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

}
