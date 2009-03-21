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
    if (theme != NULL)
        delete theme;
}

void
tile_vertices::awaken()
{
    buffer = _buffer_for_vertices(*vertices);
    delete vertices;
    vertices = NULL;

    theme = image_texture::from_file(board::current()->theme + "-theme.png");

    board::current()->tile_vertices_thing = this;
}

void
tile_vertices::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, buffer);

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glBindTexture(GL_TEXTURE_2D, theme->texture);

    glVertexPointer(2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, center));
    glTexCoordPointer(2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, texcoord));
}

void
tile_vertices::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
