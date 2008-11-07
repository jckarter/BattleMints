#include "serialization.hpp"
#include "player.hpp"
#include "sphere.hpp"
#include "wall.hpp"
#include <UIKit/UIKit.h>

namespace battlemints {

typedef thing *(*thing_reader)(Json::Value const &v);
typedef std::pair<std::string, thing_reader> thing_reader_pair;

static const boost::array<thing_reader_pair, 3> _thing_reader_pairs = {
    thing_reader_pair("sphere", &sphere::from_json),
    thing_reader_pair("wall", &wall::from_json),
    thing_reader_pair("player", &player::from_json)
};

static const std::map<std::string, thing_reader> _thing_readers
    (_thing_reader_pairs.begin(), _thing_reader_pairs.end());

typedef thing *(*thing_reader)(Json::Value const &value);
typedef std::pair<std::string, thing_reader> thing_reader_pair;

thing *thing_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 2)
        throw invalid_board_json("thing value must be an array of 2 elements");

    std::string kind = v[0u].asString();
    Json::Value params = v[1u];

    if (!params.isObject())
        throw invalid_board_json("second element of thing value must be an object");

    std::map<std::string, thing_reader>::const_iterator reader
        = _thing_readers.find(kind);

    if (reader != _thing_readers.end())
        return (reader->second)(params);
    else
        throw invalid_board_json(std::string("unknown kind of thing \"" + kind + "\""));
}

rect rect_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 4)
        throw invalid_board_json("rect value must be an array of 4 elements");
    return make_rect(v[0u].asDouble(), v[1u].asDouble(), v[2u].asDouble(), v[3u].asDouble());
}

vec2 vec2_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 2)
        throw invalid_board_json("vec2 value must be an array of 2 elements");
    return make_vec2(v[0u].asDouble(), v[1u].asDouble());
}

vec4 vec4_from_json(Json::Value const &v)
{
    if (!v.isArray() && v.size() != 4)
        throw invalid_board_json("vec4 value must be an array of 4 elements");
    return make_vec4(v[0u].asDouble(), v[1u].asDouble(), v[2u].asDouble(), v[3u].asDouble());
}

static inline NSString *objc_str(std::string const &s)
{
    return [NSString stringWithUTF8String:s.c_str()];
}

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type)
{
    NSString *s = [[NSBundle mainBundle] pathForResource:objc_str(name) ofType:objc_str(type)];

    return s ? boost::optional<std::string>([s UTF8String])
             : boost::optional<std::string>();
}

}
