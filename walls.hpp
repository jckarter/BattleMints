#ifndef __WALLS_HPP__
#define __WALLS_HPP__

#include "thing.hpp"
#include "collision.hpp"
#include "serialization.hpp"
#include "board.hpp"
#include <boost/array.hpp>

namespace battlemints {

struct wall : line {
    wall(vec2 pt_a, vec2 pt_b) : line(pt_a, pt_b, 0) { }
    wall(vec2 pt_a, vec2 pt_b, int flags) : line(pt_a, pt_b, flags) { }

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

    wall(FILE *bin) : line(0, bin) {}
protected:
    wall(int flags, FILE *bin) : line(flags, bin) {}
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

    wallpost(FILE *bin) : point(0, bin) {}
};

struct door : wall {
    static const vec4 CLUMP_COLOR, COLOR;
    static boost::array<renders_with_pair, 1> renders_with_pairs;
    static const unsigned NUM_CLUMPS = 3;

    struct {
        boost::array<vec2, 2> beam;
        boost::array<vec2, NUM_CLUMPS> clumps;
    } vertices;
    boost::array<vec2, NUM_CLUMPS> clump_velocities;
    boost::array<int, NUM_CLUMPS> clump_lives;

    door(vec2 pt_a, vec2 pt_b) : wall(pt_a, pt_b, DOES_TICKS) { _init_draw(); }
    virtual void tick();

    virtual renders_with_range renders_with() const 
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    virtual void draw_self() const;
    virtual void trigger(thing *scapegoat) { board::current()->remove_thing(this); }
    virtual char const * kind() const { return "door"; }

    door(FILE *bin) : wall(DOES_TICKS, bin) { _init_draw(); }

private:
    void _init_draw();
    void _reset_clump(unsigned i);
};

void global_start_walls();

}

#endif
