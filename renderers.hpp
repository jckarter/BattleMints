#ifndef __RENDERERS_HPP__
#define __RENDERERS_HPP__

#include <boost/utility.hpp>
#include <boost/unordered_map.hpp>
#include "drawing.hpp"
#include "game.hpp"
#include "geom.hpp"

namespace battlemints {

struct thing;

struct renderer;
struct renders_with_pair { renderer *instance; parameter param; };
typedef boost::iterator_range<renders_with_pair*> renders_with_range;
typedef std::map<renders_with_pair, std::vector<thing*> > renders_with_map;

bool operator<(renders_with_pair const &a, renders_with_pair const &b)
    { return a.instance->z_index(a.param) < b.instance->z_index(b.param); }

struct renderer : boost::noncopyable {
    typedef void* parameter;

    static const renders_with_range null_range;

    renderer() {}
    virtual ~renderer() {}

    virtual void draw(std::vector<thing*> const &things, parameter p) = 0;

    static void global_start();
    static void global_finish();

    template <typename T>
    static T parameter_as(parameter p)
        { union { parameter p; T t; } x = {p}; return x.t; }

    template <typename T>
    static parameter as_parameter(T t)
        { union { T t; parameter p; } x = {t}; return x.p; }

    virtual float z_index(parameter p) = 0;

private:
    static void _prebuild_textures();
};

struct sphere_renderer : renderer {
    static sphere_renderer *instance;

    sphere_renderer() {}
    virtual ~sphere_renderer();

    sphere_texture *make_texture(float radius);

    virtual void draw(std::vector<thing*> const &things, parameter p);

    virtual float z_index(parameter p) { return 100.0f - parameter_as<float>(p); }

private:
    boost::unordered_map<float, sphere_texture*> sphere_texture_cache;
};

struct face_renderer : renderer {
    typedef void const *face_id;
    static face_id PLAYER_FACE, MINI_FACE, MEGA_FACE;

    static face_renderer *instance;

    face_renderer() {}
    virtual ~face_renderer();

    sphere_face *make_face(face_id radius);

    virtual void draw(std::vector<thing*> const &things, parameter p);

    virtual float z_index(parameter p) { return 200.0f; }

private:
    boost::unordered_map<char const *, sphere_face*> sphere_face_cache;
};

struct tile_renderer : renderer {
    static tile_renderer *instance;
    static renders_with_pair instance_null_arg;
    static const renders_with_range instance_null_arg_range;

    tile_renderer() {}

    virtual void draw(std::vector<thing*> const &things, parameter p);

    virtual float z_index(parameter p) { return 300.0f; }
};

struct self_renderer : renderer {
    static self_renderer *instance;
    static renders_with_pair instance_null_arg;
    static const renders_with_range instance_null_arg_range;

    self_renderer() {}

    virtual void draw(std::vector<thing*> const &things, parameter p);

    virtual float z_index(parameter p) { return 0.0f; }
};

}

#endif
