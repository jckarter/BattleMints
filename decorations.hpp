#ifndef __DECORATIONS_HPP__
#define __DECORATIONS_HPP__

#include "thing.hpp"
#include "renderers.hpp"

namespace battlemints {

void global_start_decorations();

struct decoration : thing
{
    struct params {
        void const *vertices, *texcoords;
        unsigned count;
    };

    decoration(vec2 center) : thing(center, NO_COLLISION) {}

    virtual params decoration_params(decoration_renderer::decoration_id d) = 0;

protected:
    decoration(int flags, FILE *bin) : thing(NO_COLLISION | flags, bin) {}
};

struct flag : decoration
{
    static const int NUM_VERTICES = 18;
    static const float
        FLAP_MIN_AMPLITUDE, FLAP_MIN_INITIAL_AMPLITUDE, FLAP_MAX_INITIAL_AMPLITUDE,
        BASE_FLAP_MIN_WAVELENGTH, BASE_FLAP_MAX_WAVELENGTH,
        BASE_FLAP_MIN_INITIAL_AMPLITUDE, BASE_FLAP_MAX_INITIAL_AMPLITUDE,
        FLAP_AMPLITUDE_DAMP, FLAP_MIN_WAVELENGTH, FLAP_MAX_WAVELENGTH, FLAP_PHASE_STEP,
        WIDTH, HEIGHT, HEIGHT_OFFSET;

    static boost::array<vec2, NUM_VERTICES> texcoords;

    static boost::array<vec2, 4> flagpost_texcoords;
    static boost::array<vec2, 4> flagpost_vertices;

    boost::array<vec2, NUM_VERTICES> vertices;

    struct flap {
        float wavelength, amplitude, delta_wavelength, phase;

        float height(float theta) const
            { return amplitude * fast_cos_2pi((phase + theta)*wavelength); }
    };
    flap base_flap;

    virtual void tick();

    virtual params decoration_params(decoration_renderer::decoration_id d);

    flag(FILE *bin);

private:
    void _reset_flap(flap &f);
    void _reset_base_flap(flap &f);
};

struct battlemints_flag : flag
{
    static boost::array<renders_with_pair, 2> renders_with_pairs;

    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    virtual char const * kind() const { return "battlemints_flag"; }

    battlemints_flag(FILE *bin) : flag(bin) { }
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
        fallingrocks,
        blank,
        goalup,
        goalleft,
        goaldown,
        goalright,
        switchup,
        switchleft,
        switchdown,
        switchright
    };

    static const boost::array<vec2, 4> vertices;
    static boost::array<renders_with_pair, 1> renders_with_pairs;

    boost::array<vec2, 4> texcoords;

    sign(vec2 center, int s) : decoration(center)
    {
        _set_texcoords(s);
    }

    virtual params decoration_params(decoration_renderer::decoration_id d);

    virtual char const * kind() const { return "sign"; }
    virtual renders_with_range renders_with() const
        { return boost::make_iterator_range(renders_with_pairs.begin(), renders_with_pairs.end()); }

    sign(FILE *bin) : decoration(0, bin)
    {
        int face = data_from_bin<int>(bin);
        _set_texcoords(face);
    }

    virtual void print(std::ostream &os) const
        { decoration::print(os); os << " signface:" << texcoords[0].x*8.0f; }

private:
    void _set_texcoords(int face)
    {
        int column = face % 8;
        int row    = face / 8;
        texcoords = (boost::array<vec2, 4>){
            make_vec2(0.125f*column,          0.5f*row + 0.5f),
            make_vec2(0.125f*column + 0.125f, 0.5f*row + 0.5f),
            make_vec2(0.125f*column,          0.5f*row       ),
            make_vec2(0.125f*column + 0.125f, 0.5f*row       ),
        };
    }
};

}

#endif
