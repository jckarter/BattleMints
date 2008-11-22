#ifndef __SYNTH_HPP__
#define __SYNTH_HPP__

#include <boost/cstdint.hpp>
#include <cmath>
#include <limits>
#include <cstdio>
#include <cstdlib>

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
    static float f(float x) { return sinf(2.0f*(float)M_PI*x); }
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
inline void _synth_loop(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
{
    float lengthf = (float)length;
    for (float i = 0.0f; i < lengthf; i += 1.0f, ++buffer) {
        float y = 0.0f;
        float time = SYNTH_SAMPLE_LENGTH*i;
        SynthSampleFunc(y, frequency*(time + phase), time);
        *buffer = (int16_t)(amplitude * SYNTH_AMP * y);
    }
}

template<typename S1>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
}
template<typename S1>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
}
template<typename S1, typename S2>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2, typename S3>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
    S3::synth(y, x, time);
}
template<typename S1, typename S2, typename S3>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2, S3> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2, typename S3, typename S4>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
    S3::synth(y, x, time);
    S4::synth(y, x, time);
}
template<typename S1, typename S2, typename S3, typename S4>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2, S3, S4> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2, typename S3, typename S4, typename S5>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
    S3::synth(y, x, time);
    S4::synth(y, x, time);
    S5::synth(y, x, time);
}
template<typename S1, typename S2, typename S3, typename S4, typename S5>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2, S3, S4, S5> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2, typename S3, typename S4, typename S5, typename S6>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
    S3::synth(y, x, time);
    S4::synth(y, x, time);
    S5::synth(y, x, time);
    S6::synth(y, x, time);
}
template<typename S1, typename S2, typename S3, typename S4, typename S5, typename S6>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2, S3, S4, S5, S6> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
    S3::synth(y, x, time);
    S4::synth(y, x, time);
    S5::synth(y, x, time);
    S6::synth(y, x, time);
    S7::synth(y, x, time);
}
template<typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2, S3, S4, S5, S6, S7> >(buffer, length, amplitude, frequency, phase); }

template<typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8>
inline void _synth_sample(float &y, float x, float time)
{
    S1::synth(y, x, time);
    S2::synth(y, x, time);
    S3::synth(y, x, time);
    S4::synth(y, x, time);
    S5::synth(y, x, time);
    S6::synth(y, x, time);
    S7::synth(y, x, time);
    S8::synth(y, x, time);
}
template<typename S1, typename S2, typename S3, typename S4, typename S5, typename S6, typename S7, typename S8>
void synth(int16_t *buffer, unsigned length, float amplitude, float frequency, float phase)
    { _synth_loop<_synth_sample<S1, S2, S3, S4, S5, S6, S7, S8> >(buffer, length, amplitude, frequency, phase); }



}

#endif
