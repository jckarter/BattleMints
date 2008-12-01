#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "thing.hpp"
#include "geom.hpp"
#include <boost/utility.hpp>
#include <vector>
#include <ostream>

namespace battlemints {

struct grid : boost::noncopyable {

    grid()
        : _origin(ZERO_VEC2), _cell_size_inv(ZERO_VEC2),
          _cell_dims(ZERO_VEC2), _pitch(0), cells(0)
        { }
    grid(rect space, vec2 cell_size);
    
    void add_thing(thing *t, rect bound);
    void move_thing(thing *t, rect old_bound, rect new_bound);
    void remove_thing(thing *t, rect bound);

    std::set<thing*> things_in_rect(rect r) const;

    std::vector< std::set<thing*> >::iterator cell_for_point(vec2 pt);
    std::vector< std::set<thing*> >::const_iterator cell_for_point(vec2 pt) const;

    void _dump(std::ostream &os) const {
        os << "=== === === === ===\n";
        for (int i = 0; i < cells.size(); ++i) {
            if (i % _pitch == 0) {
                if (i != 0) os << "}\n";
                os << "{\n";
            }
            os << " [\n";
            for (std::set<thing*>::const_iterator th = cells[i].begin(); th != cells[i].end(); ++th)
                os << "  " << **th << "\n";
            os << " ]\n";
        }
        os << "}\n";
    }

private:
    vec2 _origin, _cell_size_inv, _cell_dims; // members are order dependent
    int _pitch;

    template <typename T, typename BinaryFunctor>
    void _for_cells_in_rect(T t, rect bound, BinaryFunctor const &f);
    template <typename T, typename BinaryFunctor>
    void _for_cells_in_rect(T t, rect bound, BinaryFunctor const &f) const;

    bool _rects_require_movement(rect old_bound, rect new_bound) const;

public:
    std::vector< std::set<thing*> > cells;
};

}

#endif
