#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <map>
#include <string>
#include <json/json.h>
#include <boost/optional.hpp>
#include "geom.hpp"

namespace battlemints {

struct thing;

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type);

thing *thing_from_json(Json::Value const &v);

struct invalid_board_json : std::runtime_error {
    invalid_board_json(std::string const &s) : std::runtime_error(s) {}
};

inline rect rect_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 4)
        throw invalid_board_json("rect value must be an array of 4 elements");
    return make_rect(v[0u].asDouble(), v[1u].asDouble(), v[2u].asDouble(), v[3u].asDouble());
}

inline vec2 vec2_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 2)
        throw invalid_board_json("vec2 value must be an array of 2 elements");
    return make_vec2(v[0u].asDouble(), v[1u].asDouble());
}

inline vec4 vec4_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 4)
        throw invalid_board_json("vec4 value must be an array of 4 elements");
    return make_vec4(v[0u].asDouble(), v[1u].asDouble(), v[2u].asDouble(), v[3u].asDouble());
}

}

#endif
