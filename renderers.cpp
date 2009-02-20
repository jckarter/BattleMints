#include "renderers.hpp"
#include "dramatis_personae.hpp"
#include "tiles.hpp"
#include <list>

namespace battlemints {

sphere_renderer *sphere_renderer::instance = NULL;
face_renderer *face_renderer::instance = NULL;
tile_renderer *tile_renderer::instance = NULL;
self_renderer *self_renderer::instance = NULL;

renders_with_pair tile_renderer::instance_null_arg, self_renderer::instance_null_arg;

const renders_with_range
    renderer::null_range
        = boost::make_iterator_range((renders_with_pair*)NULL, (renders_with_pair*)NULL),
    tile_renderer::instance_null_arg_range
        = boost::make_iterator_range(&tile_renderer::instance_null_arg,
                                     &tile_renderer::instance_null_arg + 1),
    self_renderer::instance_null_arg_range
        = boost::make_iterator_range(&self_renderer::instance_null_arg,
                                     &self_renderer::instance_null_arg + 1);

face_renderer::face_id
    face_renderer::PLAYER_FACE = (face_renderer::face_id)"player",
    face_renderer::MINI_FACE   = (face_renderer::face_id)"mini",
    face_renderer::MEGA_FACE   = (face_renderer::face_id)"mega";

void
renderer::global_start()
{
    sphere_renderer::instance = new sphere_renderer;
    face_renderer::instance = new face_renderer;
    tile_renderer::instance = new tile_renderer;
    self_renderer::instance = new self_renderer;

    tile_renderer::instance_null_arg = (renders_with_pair){ tile_renderer::instance, NULL };
    self_renderer::instance_null_arg = (renders_with_pair){ self_renderer::instance, NULL };

    _prebuild_textures();
}

void
renderer::global_finish()
{
    delete self_renderer::instance;
    delete tile_renderer::instance;
    delete face_renderer::instance;
    delete sphere_renderer::instance;
}

void
renderer::_prebuild_textures()
{
    face_renderer::instance->make_face(face_renderer::PLAYER_FACE);
    face_renderer::instance->make_face(face_renderer::MINI_FACE);
    face_renderer::instance->make_face(face_renderer::MEGA_FACE);

    sphere_renderer::instance->make_texture(player::RADIUS);
    sphere_renderer::instance->make_texture(mini::RADIUS);
    sphere_renderer::instance->make_texture(mega::RADIUS);
    sphere_renderer::instance->make_texture(bumper::INNER_RADIUS);
}

sphere_renderer::~sphere_renderer()
{
    BOOST_FOREACH (sphere_texture_cache_map::value_type const &p, sphere_texture_cache)
        delete p.second;
}

sphere_texture *
sphere_renderer::make_texture(float radius)
{
    sphere_texture *t = new sphere_texture(radius);
    sphere_texture_cache[radius] = t;
    return t;
}

void
sphere_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    float radius = parameter_as<float>(p);

    sphere_texture *st = sphere_texture_cache[radius];

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, st->texture);
    glVertexPointer(2, GL_FLOAT, 0, (void*)&st->vertices);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)&unit_texcoords);

    BOOST_FOREACH (thing *th, things) {
        sphere *sph = static_cast<sphere*>(th);

        vec4 color = sph->sphere_color(radius);
        glColor4f(color.x, color.y, color.z, color.w);
        glPushMatrix();
        glTranslatef(sph->center.x, sph->center.y, 0.0f);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glPopMatrix();
    }
}

face_renderer::~face_renderer()
{
    BOOST_FOREACH (sphere_face_cache_map::value_type const &p, sphere_face_cache)
        delete p.second;
}

sphere_face *
face_renderer::make_face(face_id face)
{
    sphere_face *f = sphere_face::from_file((char const *)face);
    sphere_face_cache[face] = f;
    return f;
}

void
face_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    face_id face = parameter_as<face_id>(p);

    sphere_face *sf = sphere_face_cache[face]; 

    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, sf->texture->texture);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_face::array_buffer);
    glVertexPointer(3, GL_FLOAT, 0, (void*)0);
    glTexCoordPointer(2, GL_FLOAT, 0, (void*)(sizeof(float)*sphere_face::MESH_VERTICES*3));

    glMatrixMode(GL_TEXTURE);
    glScalef(0.5f, 0.25f, 1.0f);

    BOOST_FOREACH (thing *th, things) {
        sphere *sph = static_cast<sphere*>(th);
        sphere_face::state st = sphere_face::state_for_course(sph->velocity, sph->cur_accel);
        GLfloat col = (GLfloat)(st / 4);
        GLfloat row = (GLfloat)(st % 4);

        glMatrixMode(GL_TEXTURE);
        glPushMatrix();
        glTranslatef(col, row, 0.0f);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glTranslatef(sph->center.x, sph->center.y, 0.0f);
        glScalef(sph->radius, sph->radius, sph->radius);
        if (st != sphere_face::panicked) {
            glRotatef(sphere_face::rotation(sph->cur_accel.y), -1.0, 0.0, 0.0);
            glRotatef(sphere_face::rotation(sph->cur_accel.x),  0.0, 1.0, 0.0);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere_face::MESH_VERTICES);
        glPopMatrix();

        glMatrixMode(GL_TEXTURE);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

namespace {
    void join_ranges(std::list<tile::vertex_range> &out_ranges, std::vector<thing*> const &tiles)
    {
        typedef std::list<tile::vertex_range>::iterator range_iter;
        typedef boost::unordered_map<GLint, range_iter> endpoint_map;

        endpoint_map endpoints;

        BOOST_FOREACH (thing *th, tiles) {
            tile *t = static_cast<tile*>(th);

            endpoint_map::iterator prev = endpoints.find(t->vertices.begin);
            endpoint_map::iterator next = endpoints.find(t->vertices.end());

            if (prev != endpoints.end() && next != endpoints.end()) {
                endpoints.erase(t->vertices.begin);
                endpoints.erase(t->vertices.end());

                prev->second->size += t->vertices.size + next->second->size;
                endpoints[prev->second->end()] = prev->second;
                out_ranges.erase(next->second);
            }
            else if (prev != endpoints.end()) {
                endpoints.erase(t->vertices.begin);
                prev->second->size += t->vertices.size;
                endpoints[t->vertices.end()] = prev->second;
            }
            else if (next != endpoints.end()) {
                endpoints.erase(t->vertices.end());
                next->second->begin  = t->vertices.begin;
                next->second->size  += t->vertices.size;
                endpoints[t->vertices.begin] = next->second;
            }
            else {
                out_ranges.push_front(t->vertices);
                endpoints[t->vertices.begin] = out_ranges.begin();
                endpoints[t->vertices.end()] = out_ranges.begin();
            }
        }
    }
}

void
tile_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    std::list<tile::vertex_range> ranges;

    join_ranges(ranges, things);

    board::current()->tile_vertices_thing->bind();
    BOOST_FOREACH (tile::vertex_range const &r, ranges)
        glDrawArrays(GL_TRIANGLE_FAN, r.begin, r.size);
    board::current()->tile_vertices_thing->unbind();
}

void
self_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    BOOST_FOREACH (thing *th, things)
        th->draw_self();
}

}
