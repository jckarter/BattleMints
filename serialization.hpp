#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <map>
#include <string>
#include <json/json.h>
#include "geom.hpp"

namespace battlemints {

struct thing;

rect rect_from_json(Json::Value const &v);
vec2 vec2_from_json(Json::Value const &v);
vec4 vec4_from_json(Json::Value const &v);

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type);

thing *thing_from_json(Json::Value const &v);

struct invalid_board_json : std::runtime_error {
    invalid_board_json(std::string const &s) : std::runtime_error(s) {}
};

}

#endif
