#ifndef __GRID_HPP__
#define __GRID_HPP__

#include "thing.hpp"
#include "geom.hpp"
#include <boost/utility.hpp>
#include <vector>

namespace battlemints {

struct grid : boost::noncopyable {

    grid() : _origin(make_vec2(0.0)), _cell_size_inv(make_vec2(0.0)), _pitch(0), cells(0) { }
    grid(rect space, vec2 cell_size);
    
    void add_thing(thing *t, rect bound);
    void move_thing(thing *t, rect old_bound, rect new_bound);
    void remove_thing(thing *t, rect bound);

    std::set<thing*> things_in_rect(rect r) const;

    std::vector< std::set<thing*> >::iterator cell_for_point(vec2 pt);
    std::vector< std::set<thing*> >::const_iterator cell_for_point(vec2 pt) const;

private:
    vec2 _origin, _cell_size_inv; // members are order dependent
    int _pitch;

    template <typename T, typename BinaryFunctor>
    void _for_cells_in_rect(T t, rect bound, BinaryFunctor const &f);
    template <typename T, typename BinaryFunctor>
    void _for_cells_in_rect(T t, rect bound, BinaryFunctor const &f) const;

public:
    std::vector< std::set<thing*> > cells;
};

}

#endif
