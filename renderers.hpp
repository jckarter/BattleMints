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
typedef void const* renderer_parameter;
struct renders_with_pair { renderer *instance; renderer_parameter param; };
typedef boost::iterator_range<renders_with_pair*> renders_with_range;
typedef std::map<renders_with_pair, std::vector<thing*> > renders_with_map;

struct renderer : boost::noncopyable {
    static const renders_with_range null_range;

    renderer() {}
    virtual ~renderer() {}

    virtual void draw(std::vector<thing*> const &things, renderer_parameter p) = 0;

    static void global_start();
    static void global_finish();

    template <typename T>
    static T parameter_as(renderer_parameter p)
        { union { renderer_parameter p; T t; } x = {p}; return x.t; }

    template <typename T>
    static renderer_parameter as_parameter(T t)
        { union { T t; renderer_parameter p; } x = {t}; return x.p; }

    virtual float z_index(renderer_parameter p) = 0;

private:
    static void _prebuild_textures();
};

template <typename RenderedTexture>
struct rendered_renderer : renderer {

    rendered_renderer() {}
    virtual ~rendered_renderer();

    RenderedTexture *make_texture(float radius);

    virtual void draw(std::vector<thing*> const &things, renderer_parameter p);

private:
    typedef boost::unordered_map<float, RenderedTexture*> texture_cache_map;
    texture_cache_map texture_cache;
};

struct sphere_renderer : rendered_renderer<sphere_texture> {
    static sphere_renderer *instance;

    virtual float z_index(renderer_parameter p) { return 100.0f - parameter_as<float>(p); }
};

struct spike_renderer : rendered_renderer<spike_texture> {
    static spike_renderer *instance;

    virtual float z_index(renderer_parameter p) { return 100.0f - parameter_as<float>(p) + 0.01f; }

private:
    typedef boost::unordered_map<float, sphere_texture*> sphere_texture_cache_map;
    sphere_texture_cache_map sphere_texture_cache;
};

struct face_renderer : renderer {
    typedef void const *face_id;
    static face_id PLAYER_FACE, PLAYER_INVULN_FACE, MINI_FACE, MEGA_FACE;

    static face_renderer *instance;

    face_renderer() {}
    virtual ~face_renderer();

    sphere_face *make_face(face_id f);

    virtual void draw(std::vector<thing*> const &things, renderer_parameter p);

    virtual float z_index(renderer_parameter p) { return 200.0f; }

private:
    typedef boost::unordered_map<face_id, sphere_face*> sphere_face_cache_map;
    sphere_face_cache_map sphere_face_cache;
};

struct tile_renderer : renderer {
    static tile_renderer *instance;
    static renders_with_pair instance_null_arg;
    static const renders_with_range instance_null_arg_range;

    tile_renderer() {}

    virtual void draw(std::vector<thing*> const &things, renderer_parameter p);

    virtual float z_index(renderer_parameter p) { return 300.0f; }
};

struct decoration_renderer : renderer {
    typedef void const *decoration_id;
    static decoration_id SIGN_DECORATION, BATTLEMINTS_FLAG_DECORATION, FLAGPOST_DECORATION;
    static decoration_renderer *instance;

    decoration_renderer() {}

    virtual void draw(std::vector<thing*> const &things, renderer_parameter p);

    virtual float z_index(renderer_parameter p) { return -100.0f; }

    image_texture *make_texture(decoration_id d);

private:
    typedef boost::unordered_map<decoration_id, image_texture*> decoration_cache_map;
    decoration_cache_map decoration_cache;
};

struct self_renderer : renderer {
    static self_renderer *instance;

    self_renderer() {}

    virtual void draw(std::vector<thing*> const &things, renderer_parameter p);

    virtual float z_index(renderer_parameter p) { return 0.0f; }
};

inline bool operator<(renders_with_pair const &a, renders_with_pair const &b)
{
    float a_z = a.instance->z_index(a.param), b_z = b.instance->z_index(b.param);

    return a_z == b_z ? a.param < b.param : a_z < b_z;
}

}

#endif
