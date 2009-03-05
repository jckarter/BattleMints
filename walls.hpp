#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include "thing.hpp"
#include "collision.hpp"
#include "serialization.hpp"
#include "board.hpp"
#include <boost/array.hpp>

namespace battlemints {

struct wall : line {
    wall(vec2 pt_a, vec2 pt_b) : line(pt_a, pt_b) { }

    virtual void on_collision(thing &o) { o.wall_damage(); }
#ifdef DRAW_WALLS
    virtual renders_with_range renders_with() const
    {
        static boost::array<renders_with_pair, 1> renders_with_pairs;
        renders_with_pairs = (boost::array<renders_with_pair, 1>)
            {{ { self_renderer::instance, (renderer_parameter)"wall" } }};
        
        return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end());
    }
    virtual void draw_self() const {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        boost::array<vec2, 2> vertices = { endpoint_a, endpoint_b };
    
        glColor4f(0.0, 1.0, 0.0, 1.0);
        glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
        glDrawArrays(GL_LINES, 0, 2);
    }
#endif

    virtual char const * kind() const { return "wall"; }

    static thing *from_json(Json::Value const &v) { return line::from_json<wall>(v); }
};

struct wallpost : point {
    wallpost(vec2 ct) : point(ct) {  }

#ifdef DRAW_WALLS
    virtual renders_with_range renders_with() const
    {
        static boost::array<renders_with_pair, 1> renders_with_pairs;
        renders_with_pairs = (boost::array<renders_with_pair, 1>)
            {{ { self_renderer::instance, (renderer_parameter)"wallpost" } }};
        
        return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end());
    }
    virtual void draw_self() const {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glPointSize(3.0f);
        glColor4f(0.0, 1.0, 0.0, 1.0);
        glVertexPointer(2, GL_FLOAT, 0, (void*)&center);
        glDrawArrays(GL_POINTS, 0, 1);
    }
#endif

    virtual void on_collision(thing &o) { o.post_damage(); }

    virtual char const * kind() const { return "wallpost"; }

    static thing *from_json(Json::Value const &v) { return point::from_json<wallpost>(v); }
};

struct door : wall {
    static const vec4 COLOR;
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    boost::array<vec2, 2> vertices;

    door(vec2 pt_a, vec2 pt_b) : wall(pt_a, pt_b)
        { vertices = (boost::array<vec2, 2>){ pt_a, pt_b }; }

    virtual bool does_ticks() const { return true; }

    virtual renders_with_range renders_with() const 
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    virtual void draw_self() const {
        glDisable(GL_TEXTURE_2D);
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);

        glPointSize(3.0f);
        glColor4f(COLOR.x, COLOR.y, COLOR.z, COLOR.w);
        glVertexPointer(2, GL_FLOAT, 0, (void*)&vertices);
        glDrawArrays(GL_LINES, 0, 2);
    }

    virtual void trigger(thing *scapegoat) { board::current()->remove_thing(this); }
    virtual char const * kind() const { return "door"; }

    static thing *from_json(Json::Value const &v) { return line::from_json<door>(v); }
};

void global_start_walls();

}

#endif
