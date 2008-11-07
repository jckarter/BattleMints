#include "wall.hpp"
#include "game.hpp"
#include "serialization.hpp"

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
    glDrawArrays(GL_LINES, 0, 2);
}

void
wall::_set_up_vertices()
{
    _vertices[0] = endpoint_a.x; _vertices[1] = endpoint_a.y;
    _vertices[2] = endpoint_b.x; _vertices[3] = endpoint_b.y;
}

thing *
wall::from_json(Json::Value const &v)
{
    vec2 endpoint_a = vec2_from_json(v["endpoint_a"]);
    vec2 endpoint_b = vec2_from_json(v["endpoint_b"]);

    return new wall(endpoint_a, endpoint_b);
}

}
