#include "walls.hpp"
#include "board.hpp"
#include "serialization.hpp"

namespace battlemints {

#if 0
void
wall_strip::draw()
{
    glDisable(GL_TEXTURE_2D);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

    glColor4f(0.0, 0.0, 0.0, 1.0);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices[0]);

    glDrawArrays(closed ? GL_LINE_LOOP : GL_LINE_STRIP, 0, vertices.size());
}
#endif

void
wall_strip::_generate_collision_things()
{
    board *b = board::current();
    
    for (std::vector<vec2>::const_iterator vi = vertices.begin();
         vi + 1 != vertices.end();
         ++vi) {

        b->add_thing(new wallpost(*vi));
        b->add_thing(new wall(vi[0], vi[1]));
    }
    b->add_thing(new wallpost(vertices.end()[-1]));
    if (closed)
        b->add_thing(new wall(vertices.end()[-1], vertices.begin()[0]));
}

thing *
wall_strip::from_json(Json::Value const &v)
{
    Json::Value vertices_json = v["vertices"];
    bool closed = v["closed"].asBool();

    if (!vertices_json.isArray())
        throw invalid_board_json("vertices element of wall_strip must be array");
    std::vector<vec2> vertices(vertices_json.size());

    for (unsigned i = 0; i < vertices_json.size(); ++i)
        vertices[i] = vec2_from_json(vertices_json[i]);

    return new wall_strip(vertices.begin(), vertices.end(), closed);
}

}
