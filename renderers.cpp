#include "renderers.hpp"
#include "dramatis_personae.hpp"
#include "tiles.hpp"
#include "decorations.hpp"
#include <list>
#include <iostream>

namespace battlemints {

sphere_renderer *sphere_renderer::instance = NULL;
spike_renderer *spike_renderer::instance = NULL;
face_renderer *face_renderer::instance = NULL;
tile_renderer *tile_renderer::instance = NULL;
decoration_renderer *decoration_renderer::instance = NULL;
self_renderer *self_renderer::instance = NULL;

renders_with_pair tile_renderer::instance_null_arg;

const renders_with_range
    renderer::null_range
        = boost::make_iterator_range((renders_with_pair*)NULL, (renders_with_pair*)NULL),
    tile_renderer::instance_null_arg_range
        = boost::make_iterator_range(&tile_renderer::instance_null_arg,
                                     &tile_renderer::instance_null_arg + 1);

face_renderer::face_id
    face_renderer::PLAYER_FACE        = (face_renderer::face_id)"player",
    face_renderer::PLAYER_INVULN_FACE = (face_renderer::face_id)"player_invuln",
    face_renderer::MINI_FACE          = (face_renderer::face_id)"mini",
    face_renderer::MINI_WHITE_FACE    = (face_renderer::face_id)"mini-white",
    face_renderer::MEGA_FACE          = (face_renderer::face_id)"mega";

decoration_renderer::decoration_id
    decoration_renderer::SIGN_DECORATION = (decoration_renderer::decoration_id)"signs.png",
    decoration_renderer::BATTLEMINTS_FLAG_DECORATION = (decoration_renderer::decoration_id)"battlemints-flag.png",
    decoration_renderer::FLAGPOST_DECORATION = (decoration_renderer::decoration_id)"flagpost.png",
    decoration_renderer::START_BANNER_DECORATION = (decoration_renderer::decoration_id)"start-banner.png";

void
renderer::global_start()
{
    sphere_renderer::instance = new sphere_renderer;
    spike_renderer::instance = new spike_renderer;
    face_renderer::instance = new face_renderer;
    tile_renderer::instance = new tile_renderer;
    decoration_renderer::instance = new decoration_renderer;
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
    face_renderer::instance->make_face(face_renderer::PLAYER_INVULN_FACE);
    face_renderer::instance->make_face(face_renderer::MINI_FACE);
    face_renderer::instance->make_face(face_renderer::MINI_WHITE_FACE);
    face_renderer::instance->make_face(face_renderer::MEGA_FACE);

    sphere_renderer::instance->make_texture(player::SHIELD_RADIUS);
    sphere_renderer::instance->make_texture(pellet::RADIUS);
    sphere_renderer::instance->make_texture(player::RADIUS);
    sphere_renderer::instance->make_texture(mini::RADIUS);
    sphere_renderer::instance->make_texture(shield_mini::OUTER_RADIUS);
    sphere_renderer::instance->make_texture(mega::RADIUS);
    sphere_renderer::instance->make_texture(bumper::INNER_RADIUS);
    sphere_renderer::instance->make_texture(trigger_switch::RADIUS);
    sphere_renderer::instance->make_texture(heavy_switch::RADIUS);

    spike_renderer::instance->make_texture(durian::RADIUS);

    decoration_renderer::instance->make_texture(decoration_renderer::SIGN_DECORATION);
    decoration_renderer::instance->make_texture(decoration_renderer::BATTLEMINTS_FLAG_DECORATION);
    decoration_renderer::instance->make_texture(decoration_renderer::FLAGPOST_DECORATION);
    decoration_renderer::instance->make_texture(decoration_renderer::START_BANNER_DECORATION);
}

template<typename RenderedTexture>
rendered_renderer<RenderedTexture>::~rendered_renderer()
{
    BOOST_FOREACH (typename texture_cache_map::value_type const &p, texture_cache)
        delete p.second;
}

template<typename RenderedTexture>
RenderedTexture *
rendered_renderer<RenderedTexture>::make_texture(float radius)
{
    RenderedTexture *t = new RenderedTexture(radius);
    texture_cache[radius] = t;
    return t;
}

template<typename RenderedTexture>
void
rendered_renderer<RenderedTexture>::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    float radius = parameter_as<float>(p);

    RenderedTexture *st = texture_cache[radius];

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

template struct rendered_renderer<sphere_texture>;
template struct rendered_renderer<spike_texture>;

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
        glScalef(sph->face_radius, sph->face_radius, sph->face_radius);
        if (st != sphere_face::panicked && st != sphere_face::asleep) {
            vec2 cur_accel = vnormalize(sph->cur_accel);
            glRotatef(sphere_face::rotation(cur_accel.y), -1.0, 0.0, 0.0);
            glRotatef(sphere_face::rotation(cur_accel.x),  0.0, 1.0, 0.0);
        }

        glDrawArrays(GL_TRIANGLE_STRIP, 0, sphere_face::MESH_VERTICES);
        glPopMatrix();

        glMatrixMode(GL_TEXTURE);
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
    BOOST_FOREACH (thing *th, things) {
        tile *t = static_cast<tile*>(th);
        glDrawArrays(GL_TRIANGLE_FAN, t->vertices.begin, t->vertices.size);
    }
    board::current()->tile_vertices_thing->unbind();
}

void
decoration_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    decoration_id deco = parameter_as<decoration_id>(p);

    image_texture *tex = decoration_cache[deco]; 
        
    glEnable(GL_TEXTURE_2D);
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, tex->texture);
    glMatrixMode(GL_MODELVIEW);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    BOOST_FOREACH (thing *th, things) {
        decoration *d = static_cast<decoration*>(th);
        decoration::params params = d->decoration_params(deco);

        glPushMatrix();
        glTranslatef(d->center.x, d->center.y, 0.0f);
        glVertexPointer(2, GL_FLOAT, 0, (void*)params.vertices);
        glTexCoordPointer(2, GL_FLOAT, 0, (void*)params.texcoords);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, params.count);

        glPopMatrix();
    }

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

image_texture *
decoration_renderer::make_texture(decoration_id d)
{
    image_texture *deco = image_texture::from_file((char const *)d);
    decoration_cache[d] = deco;
    return deco;
}

void
self_renderer::draw(std::vector<thing*> const &things, renderer_parameter p)
{
    BOOST_FOREACH (thing *th, things)
        th->draw_self();
}

}
