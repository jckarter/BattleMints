#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "thing.hpp"
#include "geom.hpp"
#include <boost/utility.hpp>
#include <vector>
#include <ostream>

namespace battlemints {

struct grid : boost::noncopyable {
    static const unsigned CELL_RESERVE = 16;

    typedef std::vector<thing*> cell;

    grid(rect space, vec2 cell_size);

    void _draw() const;
    
    void add_thing(thing *t);
    void move_thing(thing *t);
    void remove_thing(thing *t);

    std::set<thing*> things_in_rect(rect r) const;

    std::vector<cell>::iterator cell_for_point(vec2 pt);
    std::vector<cell>::const_iterator cell_for_point(vec2 pt) const;

    void _dump(std::ostream &os) const {
        os << "=== === === === ===\n";
        for (int i = 0; i < cells.size(); ++i) {
            if (i % _pitch == 0) {
                if (i != 0) os << "}\n";
                os << "{\n";
            }
            os << " [\n";
            for (cell::const_iterator th = cells[i].begin(); th != cells[i].end(); ++th)
                os << "  " << **th << "\n";
            os << " ]\n";
        }
        os << "}\n";
    }

    int pitch() const { return _pitch; }

private:
    vec2 _origin, _cell_size_inv, _cell_dims; // members are order dependent
    int _pitch;

    template <typename T, typename BinaryFunctor>
    void _for_cells_in_rect(T t, rect bound, BinaryFunctor const &f);
    template <typename T, typename BinaryFunctor>
    void _for_cells_in_rect(T t, rect bound, BinaryFunctor const &f) const;

public:
    std::vector<cell> cells;
};

}

#endif
