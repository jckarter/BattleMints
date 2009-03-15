#ifndef __DECORATIONS_HPP__
#define __DECORATIONS_HPP__

#include "thing.hpp"

namespace battlemints {

void global_start_decorations();

struct decoration : thing
{
    struct params {
        void const *vertices, *texcoords;
    };

    decoration(vec2 center) : thing(center, NO_COLLISION) {}

    virtual params decoration_params() = 0;

protected:
    decoration(int flags, FILE *bin) : thing(NO_COLLISION | flags, bin) {}
};

struct sign : decoration
{
    enum signface {
        mini_xing = 0,
        mega_xing,
        narrows,
        spikes,
        slow,
        stop,
        leftarrow,
        rightarrow
    };

    static const boost::array<vec2, 4> vertices;
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    boost::array<vec2, 4> texcoords;

    sign(vec2 center, int s) : decoration(center)
    {
        _set_texcoords(s);
    }

    virtual params decoration_params();

    virtual char const * kind() const { return "sign"; }
    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    sign(FILE *bin) : decoration(0, bin)
    {
        int face = data_from_bin<int>(bin);
        _set_texcoords(face);
    }

private:
    void _set_texcoords(int face)
    {
        texcoords = (boost::array<vec2, 4>){
            make_vec2(0.125f*s,          1.0f),
            make_vec2(0.125f*s + 0.125f, 1.0f),
            make_vec2(0.125f*s,          0.0f),
            make_vec2(0.125f*s + 0.125f, 0.0f),
        };
    }
};

}

#endif
