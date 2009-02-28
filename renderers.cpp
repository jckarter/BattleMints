#include "renderers.hpp"
#include "dramatis_personae.hpp"
#include "tiles.hpp"
#include <list>
#include <iostream>

namespace battlemints {

sphere_renderer *sphere_renderer::instance = NULL;
face_renderer *face_renderer::instance = NULL;
tile_renderer *tile_renderer::instance = NULL;
decoration_renderer *decoration_renderer::instance = NULL;
self_renderer *self_renderer::instance = NULL;

renders_with_pair tile_renderer::instance_null_arg, self_renderer::instance_null_arg;

const renders_with_range
    renderer::null_range
        = boost::make_iterator_range((renders_with_pair*)NULL, (renders_with_pair*)NULL),
    tile_renderer::instance_null_arg_range
        = boost::make_iterator_range(&tile_renderer::instance_null_arg,
                                     &tile_renderer::instance_null_arg + 1);

face_renderer::face_id
    face_renderer::PLAYER_FACE = (face_renderer::face_id)"player",
    face_renderer::MINI_FACE   = (face_renderer::face_id)"mini",
    face_renderer::MEGA_FACE   = (face_renderer::face_id)"mega";

decoration_renderer::decoration_id
    decoration_renderer::SIGN_DECORATION = (decoration_renderer::decoration_id)"signs";

void
renderer::global_start()
{
    sphere_renderer::instance = new sphere_renderer;
    face_renderer::instance = new face_renderer;
    tile_renderer::instance = new tile_renderer;
    decoration_renderer::instance = new self_renderer;
    self_renderer::instance = new self_renderer;

    tile_renderer::instance_null_arg = (renders_with_pair){ tile_renderer::instance, NULL };

    _prebuild_textures();
}

void
renderer::global_finish()
{
    delete self_renderer::instance;
    delete tile_renderer::instance;
    delete face_renderer::instance;
    delete sphere_renderer::instance;
    delete decoration_renderer::instance;
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
    sphere_renderer::instance->make_texture(switch_spring::RADIUS);

    decoration_renderer::instance->make_texture(decoration_renderer::SIGN_DECORATION);
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
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

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

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void
tile_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    board::current()->tile_vertices_thing->bind();
    BOOST_FOREACH (thing *th, ranges) {
        tile *t = static_cast<tile*>(th);
        glDrawArrays(GL_TRIANGLE_FAN, t->vertices.begin, t->vertices.size);
    }
    board::current()->tile_vertices_thing->unbind();
}

void
decoration_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    decoration_id decoration = parameter_as<decoration_id>(p);

    image_texture *tex = decoration_cache[decoration]; 
        
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, tex->texture);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);

    BOOST_FOREACH (thing *th, things) {
        decoration *d = static_cast<decoration*>(th);
        decoration::params params = d->decoration_params();

        glPushMatrix();
        glTranslatef(d->center.x, d->center.y, 0.0f);
        glVertexPointer(2, GL_FLOAT, 0, (void*)params.vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)params.texcoords);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glPopMatrix();
    }
}

image_texture *
decoration_renderer::make_texture(decoration_id d)
{
    image_texture *decoration = image_texture::from_file((char const *)face);
    sphere_face_cache[face] = f;
    return f;
}

void
self_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    BOOST_FOREACH (thing *th, things)
        th->draw_self();
}

}
