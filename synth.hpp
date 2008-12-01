#ifndef __SYNTH_HPP__
#define __SYNTH_HPP__

#include <boost/preprocessor.hpp>
#include <boost/cstdint.hpp>
#include <cmath>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include "geom.hpp"

namespace battlemints {

static const float SYNTH_SAMPLE_LENGTH = 1.0f/44100.0f;
static const float SYNTH_AMP = (float)std::numeric_limits<int16_t>::max();

typedef float FloatFunc();

inline float one() { return 1.0f; }
inline float zero() { return 0.0f; }
template<unsigned num, unsigned denom>
inline float fraction() { return (float)num/(float)denom; }
template<unsigned num>
inline float integer() { return (float)num; }

template<typename T, FloatFunc Amplitude = one, FloatFunc Frequency = one, FloatFunc Phase = zero>
struct synth_base {
    static void synth(float &y, float x, float)
        { y += Amplitude() * T::f(Frequency()*(x + Phase())); }
};

template<FloatFunc Amplitude = one>
struct noise_synth
    : synth_base<noise_synth<Amplitude>, Amplitude>
{
    static float f(float) { return (float)rand()/(float)RAND_MAX; }
};

template<FloatFunc Amplitude = one, FloatFunc Frequency = one, FloatFunc Phase = zero>
struct sine_wave_synth
    : synth_base<sine_wave_synth<Amplitude, Frequency, Phase>, Amplitude, Frequency, Phase>
{
    static float f(float x) { return fast_cos_2pi(x); }
};

template<FloatFunc Amplitude = one, FloatFunc Frequency = one, FloatFunc Phase = zero>
struct square_wave_synth
    : synth_base<square_wave_synth<Amplitude, Frequency, Phase>, Amplitude, Frequency, Phase>
{
    static float f(float x) { return roundf(x) - x <= 0.0f ? 1.0f : -1.0f; }
};

template<FloatFunc Amplitude = one, FloatFunc Frequency = one, FloatFunc Phase = zero>
struct saw_wave_synth
    : synth_base<saw_wave_synth<Amplitude, Frequency, Phase>, Amplitude, Frequency, Phase>
{
    static float f(float x) { return 2.0f * ((x - 0.5f) - floorf(x - 0.5f)) - 1.0f; }
};

template<FloatFunc Amplitude = one, FloatFunc Frequency = one, FloatFunc Phase = zero>
struct triangle_wave_synth
    : synth_base<saw_wave_synth<Amplitude, Frequency, Phase>, Amplitude, Frequency, Phase>
{
    static float f(float x) { return 2.0f * fabsf(x - 2.0 * floorf(0.5*x) - 1.0f) - 1.0f; }
};

template<FloatFunc Attack, FloatFunc Decay>
struct envelope {
    static void synth(float &y, float x, float time)
    {
        float n;
        if (time < Attack())
            n = time/Attack() * expf(1.0f - time/Attack());
        else
            n = exp(-0.69f/Decay() * (time - Attack()));
        y *= n;
    }
};

template<void SynthSampleFunc(float &, float, float)>
inline void _synth_loop(int16_t *buffer, unsigned start, unsigned length, float amplitude, float frequency, float phase)
{
    float startf = (float)start, lengthf = (float)length;
    for (float i = 0.0f; i < lengthf; i += 1.0f, ++buffer) {
        float y = 0.0f;
        float time = SYNTH_SAMPLE_LENGTH*(startf + i);
        SynthSampleFunc(y, frequency*(time + phase), time);
        *buffer = (int16_t)(amplitude * SYNTH_AMP * y);
    }
}

#define _SYNTH_TEMPLATE_PARAMS_(_, __, elem) (typename elem)
#define SYNTH_TEMPLATE_PARAMS(params) \
    BOOST_PP_SEQ_ENUM(BOOST_PP_SEQ_FOR_EACH(_SYNTH_TEMPLATE_PARAMS_, _, params))

#define _SYNTH_CALLS_(_, __, elem) elem::synth(y, x, time);
#define SYNTH_CALLS(params) \
    BOOST_PP_SEQ_FOR_EACH(_SYNTH_CALLS_, _, params)

#define SYNTH_TEMPLATE_ARGS(params) \
    BOOST_PP_SEQ_ENUM(params)

#define DEFINE_SYNTH_FOR(params) \
    template<SYNTH_TEMPLATE_PARAMS(params)> \
    inline void _synth_sample(float &y, float x, float time) { SYNTH_CALLS(params) } \
    template<SYNTH_TEMPLATE_PARAMS(params)> \
    void synth(int16_t *buffer, unsigned start, unsigned length, float amplitude, float frequency, float phase) \
        { _synth_loop<_synth_sample<SYNTH_TEMPLATE_ARGS(params)> >(buffer, start, length, amplitude, frequency, phase); }

DEFINE_SYNTH_FOR((S1))
DEFINE_SYNTH_FOR((S1)(S2))
DEFINE_SYNTH_FOR((S1)(S2)(S3))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4)(S5))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4)(S5)(S6))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4)(S5)(S6)(S7))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4)(S5)(S6)(S7)(S8))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4)(S5)(S6)(S7)(S8)(S9))
DEFINE_SYNTH_FOR((S1)(S2)(S3)(S4)(S5)(S6)(S7)(S8)(S9)(S10))

}

#endif
