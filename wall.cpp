#include "wall.hpp"
#include "game.hpp"

namespace battlemints {

rect
wall::visibility_box()
{
    return make_rect(vmin(endpoint_a, endpoint_b), vmax(endpoint_a, endpoint_b));
}

rect
wall::collision_box()
{
    return make_rect(vmin(endpoint_a, endpoint_b), vmax(endpoint_a, endpoint_b));
}

void
wall::draw()
{
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&_vertices);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void
wall::_set_up_vertices()
{
    vec2 thickness = normal * BORDER_THICKNESS * 0.5;
    vec2 ll = endpoint_a - thickness, lr = endpoint_b - thickness,
         ul = endpoint_a + thickness, ur = endpoint_b + thickness;

    _vertices[0] = ll.x; _vertices[1] = ll.y;
    _vertices[2] = lr.x; _vertices[3] = lr.y;
    _vertices[4] = ul.x; _vertices[5] = ul.y;
    _vertices[6] = ur.x; _vertices[7] = ur.y;
}

}
