#ifndef __GEOM_HPP__
#define __GEOM_HPP__

#include <algorithm>
#include <iostream>
#include <cmath>

namespace battlemints {

inline float fast_trunc(float x)
{
    union { float f; int bits; } xx = { x };
    int shift = 23 + 127 - ((xx.bits & 0x7f800000) >> 23);
    xx.bits &= (0x7fffff << shift) | 0xff800000;
    return xx.f;
}

inline float fast_cos_2pi(float x)
{
    float xp = x - 0.5f - floorf(x);
    return 16.0f*xp*fabsf(xp) - 8.0f*xp;
}

inline float fast_sin_2pi(float x)
    { return fast_cos_2pi(0.25f - x); }

template<typename V, typename N>
inline N blend(V zero, V one, N factor)
    { return zero + factor*(one - zero); }

struct vec2 {
    float x, y; 

    inline static vec2 make(float a, float b) { return (vec2){a, b}; }

    inline vec2 operator-() const { return make(-x, -y); }

    inline vec2 operator+(vec2 o) const { return make(x + o.x, y + o.y); }
    inline vec2 operator-(vec2 o) const { return make(x - o.x, y - o.y); }
    inline vec2 operator*(vec2 o) const { return make(x * o.x, y * o.y); }
    inline vec2 operator/(vec2 o) const { return make(x / o.x, y / o.y); }

    inline vec2 operator*(float o) const { return make(x * o, y * o); }
    inline vec2 operator/(float o) const { return make(x / o, y / o); }

    inline bool operator<(vec2 o) const { return x == o.x ? y < o.y : x < o.x; }

    inline vec2 &operator+=(vec2 o) { x += o.x; y += o.y; return *this; }
    inline vec2 &operator-=(vec2 o) { x -= o.x; y -= o.y; return *this; }
    inline vec2 &operator*=(vec2 o) { x *= o.x; y *= o.y; return *this; }
    inline vec2 &operator/=(vec2 o) { x /= o.x; y /= o.y; return *this; }

    inline vec2 &operator*=(float o) { x *= o; y *= o; return *this; }
    inline vec2 &operator/=(float o) { x /= o; y /= o; return *this; }
};

inline vec2 operator*(float n, vec2 v) { return v*n; }
inline vec2 operator/(float n, vec2 v) { return vec2::make(n / v.x, n / v.y); }

inline vec2 make_vec2(float a, float b) { return vec2::make(a,b); }
inline vec2 make_vec2(float a) { return vec2::make(a,a); }

#define CONST_VEC2(a, b) ((::battlemints::vec2){ a, b })
#define CONST_VEC2_SPLAT(a) ((::battlemints::vec2){ a, a })

struct vec4 {
    float x, y, z, w;

    inline static vec4 make(float a, float b, float c, float d) { return (vec4){a, b, c, d}; }

    inline vec4 operator-() const { return make(-x, -y, -z, -w); }

    inline vec4 operator+(vec4 o) const { return make(x + o.x, y + o.y, z + o.z, w + o.w); }
    inline vec4 operator-(vec4 o) const { return make(x - o.x, y - o.y, z - o.z, w - o.w); }
    inline vec4 operator*(vec4 o) const { return make(x * o.x, y * o.y, z * o.z, w * o.w); }
    inline vec4 operator/(vec4 o) const { return make(x / o.x, y / o.y, z / o.z, w / o.w); }

    inline vec4 operator*(float o) const { return make(x * o, y * o, z * o, w * o); }
    inline vec4 operator/(float o) const { return make(x / o, y / o, z / o, w / o); }
};

inline vec4 operator*(float n, vec4 v) { return v*n; }
inline vec4 operator/(float n, vec4 v) { return vec4::make(n / v.x, n / v.y, n / v.z, n / v.w); }

inline vec4 make_vec4(float a, float b, float c, float d) { return vec4::make(a, b, c, d); }
inline vec4 make_vec4(float a) { return vec4::make(a, a, a, a); }

#define CONST_VEC4(a, b, c, d) ((::battlemints::vec4){ a, b, c, d })
#define CONST_VEC4_SPLAT(a) ((::battlemints::vec4){ a, a, a, a })

inline vec2 vmin(vec2 a, vec2 b)
{
    return make_vec2(std::min(a.x, b.x), std::min(a.y, b.y));
}
inline vec2 vmax(vec2 a, vec2 b)
{
    return make_vec2(std::max(a.x, b.x), std::max(a.y, b.y));
}
inline vec2 vceil(vec2 a)
{
    return make_vec2(ceilf(a.x), ceilf(a.y));
}

inline float vsum(vec2 a) { return a.x + a.y; }
inline float vproduct(vec2 a) { return a.x * a.y; }
inline float vdot(vec2 a, vec2 b) { return vsum(a * b); }
inline float vnorm2(vec2 a) { return vdot(a, a); }
inline float vnorm(vec2 a) { return sqrtf(vnorm2(a)); }
inline vec2 vnormalize(vec2 a) { return a/vnorm(a); }
inline vec2 vclip(vec2 a, float limit)
{
    float norm2 = vnorm2(a), limit2 = limit*limit;
    return norm2 > limit2 ? limit*a/sqrtf(norm2) : a;
}

inline vec2 vreflect(vec2 normal, vec2 a) { return a - 2.0f*vdot(a, normal)*normal; }
inline vec2 vperp(vec2 a) { return make_vec2(-a.y, a.x); }

struct rect {
    vec2 low, high;
};

inline rect make_rect(vec2 low, vec2 high) { return (rect){low, high}; }
inline rect make_rect(float lx, float ly, float hx, float hy)
    { return (rect){make_vec2(lx, ly), make_vec2(hx, hy)}; }

inline vec2 rect_lr(rect r) { return make_vec2(r.high.x, r.low.y); }
inline vec2 rect_ul(rect r) { return make_vec2(r.low.x, r.high.y); }

inline bool operator==(vec2 a, vec2 b) { return a.x == b.x && a.y == b.y; }
inline bool operator==(rect a, rect b) { return a.low == b.low && a.high == b.high; }

inline bool operator!=(vec2 a, vec2 b) { return a.x != b.x || a.y != b.y; }
inline bool operator!=(rect a, rect b) { return a.low != b.low || a.high != b.high; }

inline std::ostream &operator<<(std::ostream &os, vec2 v)
    { return os << "<" << v.x << "," << v.y << ">"; }
inline std::ostream &operator<<(std::ostream &os, vec4 v)
    { return os << "<" << v.x << "," << v.y << "," << v.z << "," << v.w << ">"; }
inline std::ostream &operator<<(std::ostream &os, rect r)
    { return os << "[" << r.low << "-" << r.high << "]"; }

inline float rand_between(float lo, float hi)
{
    float r = (float)rand()/(float)RAND_MAX;
    float spread = hi - lo;
    float ret = r * spread + lo;

    return ret;
}

inline float rand_near(float center, float epsilon)
    { return rand_between(center - epsilon, center + epsilon); }

inline float signum(float n)
{
    return n <  0.0f ? -1.0f
         : n == 0.0f ?  0.0f
         :              1.0f;

}

inline vec2 polar_vec2(float rho, float theta)
    { return make_vec2(rho*fast_cos_2pi(theta), rho*fast_sin_2pi(theta)); }

}

#endif
