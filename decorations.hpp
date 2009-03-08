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

    decoration(vec2 center) : thing(0.0f, center, 0.0f, NO_COLLISION) {}

    virtual params decoration_params() = 0;
    virtual bool does_ticks() { return false; }
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
    static std::map<std::string, signface> signface_names;

    boost::array<vec2, 4> texcoords;

    sign(vec2 center, signface s) : decoration(center)
    {
        texcoords = (boost::array<vec2, 4>){
            make_vec2(0.125f*s,          1.0f),
            make_vec2(0.125f*s + 0.125f, 1.0f),
            make_vec2(0.125f*s,          0.0f),
            make_vec2(0.125f*s + 0.125f, 0.0f),
        };
    }

    virtual params decoration_params();

    virtual char const * kind() const { return "sign"; }
    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    static thing *from_json(Json::Value const &v);
};

}

#endif
