#include "board.hpp"
#include "explosion.hpp"
#include "exhaust.hpp"
#include "geom.hpp"

namespace battlemints {

static const float EXPLOSION_MIN_SPEED = 0.5f;
static const float EXPLOSION_MAX_SPEED = 1.0f;
static const unsigned EXPLOSION_PARTICLE_COUNT = 300;
static const unsigned EXPLOSION_LIFE_EXPECTANCY = EXHAUST_LIFE_EXPECTANCY;
static const float EXPLOSION_VISIBLE_RADIUS = 5.0f;

inline vec2
explosion::_random_velocity()
{
    float rho   = rand_between(EXPLOSION_MIN_SPEED, EXPLOSION_MAX_SPEED);
    float theta = rand_between(0.0, 1.0);

    return make_vec2(rho * fast_cos_2pi(theta), rho * fast_sin_2pi(theta));
}

inline float
explosion::_random_size()
{
    return rand_between(0.5f, 2.0f);
}

explosion::explosion(vec2 ce)
    : thing(0.0f, ce, 1.0f), age(0), particles(EXPLOSION_PARTICLE_COUNT)
{
    BOOST_FOREACH (particle &p, particles) {
        p.center = ce;
        p.velocity = _random_velocity();
        p.size = _random_size();
    }
}

void
explosion::explode(thing *th)
{
    if (board::current()->thing_lives(th)) {
        explosion *ex = new explosion(th->center);
        board::current()->replace_thing(th, ex);
    }
}

rect
explosion::visibility_box()
{
    vec2 radius_v = make_vec2(EXPLOSION_VISIBLE_RADIUS);
    
    return make_rect(center - radius_v, center + radius_v);
}

void
explosion::draw()
{
#if 1
    if (age < 1) {
        glClearColor(0.9f, 0.85f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    } else {
        unsigned num_particles = particles.size();

        std::vector<vec2> vertices(num_particles);
        std::vector<vec4> colors(num_particles);
        std::vector<float> sizes(num_particles);

        std::vector<particle>::const_iterator pi = particles.begin();
        std::vector<vec2>::iterator vi = vertices.begin();
        std::vector<vec4>::iterator ci = colors.begin();
        std::vector<float>::iterator si = sizes.begin();

        float age_fract = (float)age/(float)EXPLOSION_LIFE_EXPECTANCY;
        float age_factor = 1.0f - age_fract;
        for (; pi != particles.end(); ++pi, ++vi, ++ci, ++si) {
            *vi = pi->center;
            *ci = exhaust::color(age);
            *si = pi->size * age_factor;
        }

        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_POINT_SIZE_ARRAY_OES);

        glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices[0]);
        glColorPointer(4, GL_FLOAT, 0, (void*)&colors[0]);
        glPointSizePointerOES(GL_FLOAT, 0, (void*)&sizes[0]);
        glDrawArrays(GL_POINTS, 0, num_particles);

        glDisableClientState(GL_POINT_SIZE_ARRAY_OES);
        glDisableClientState(GL_COLOR_ARRAY);
    }
#endif
}

void
explosion::tick()
{
    BOOST_FOREACH (particle &p, particles)
        p.center += p.velocity;

    if (++age > EXPLOSION_LIFE_EXPECTANCY)
        board::current()->remove_thing(this);
}

}
