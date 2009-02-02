#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "thing.hpp"
#include "geom.hpp"
#include <boost/utility.hpp>
#include <vector>
#include <ostream>

namespace battlemints {

struct grid : boost::noncopyable {

    struct cell {
        static const unsigned CELL_RESERVE = 16;

        typedef std::vector<thing*>::iterator iterator;
        typedef std::vector<thing*>::const_iterator const_iterator;

        std::vector<thing*> things;
        unsigned dynamic_begin_offset;

        cell() : dynamic_begin_offset(0) { things.reserve(CELL_RESERVE); }

        iterator dynamic_begin()
            { return things.begin() + dynamic_begin_offset; }
        const_iterator dynamic_begin() const
            { return things.begin() + dynamic_begin_offset; }

        bool has_dynamic() { return dynamic_begin_offset != things.size(); }
        
        void sort_statics();
    };

    typedef std::vector<cell>::iterator cell_iterator;
    typedef std::vector<cell>::const_iterator cell_const_iterator;

    grid(rect space, vec2 cell_size);

    void _draw() const;
    
    void sort_statics() { BOOST_FOREACH (cell &c, cells) { c.sort_statics(); } }

    void add_thing(thing *t);
    void move_thing(thing *t);
    void remove_thing(thing *t);

    cell_iterator cell_for_point(vec2 pt);
    cell_const_iterator cell_for_point(vec2 pt) const;

    void _dump(std::ostream &os) const {
        os << "=== === === === ===\n";
        for (int i = 0; i < cells.size(); ++i) {
            if (i % _pitch == 0) {
                if (i != 0) os << "}\n";
                os << "{\n";
            }
            os << " [\n";
            for (cell::const_iterator th = cells[i].things.begin();
                 th != cells[i].things.end();
                 ++th)
                os << "  " << **th << "\n";
            os << " ]\n";
        }
        os << "}\n";
    }

    int pitch() const { return _pitch; }

    template <typename UnaryFunctor>
    void for_cells_in_rect(rect bound, UnaryFunctor const &f)
    {
        std::vector<cell>::iterator start = cell_for_point(bound.low);
        std::vector<cell>::iterator right = cell_for_point(rect_lr(bound));
        unsigned width = right - start;
        std::vector<cell>::iterator end = cell_for_point(bound.high);

        for (std::vector<cell>::iterator y = start; y <= end; y += _pitch)
            for (std::vector<cell>::iterator x = y; x - y <= width; ++x)
                f(*x);
    }

private:
    vec2 _origin, _cell_size_inv, _cell_dims; // members are order dependent
    int _pitch;

public:
    std::vector<cell> cells;
};

}

#endif
