#include "serialization.hpp"
#include "dramatis_personae.hpp"
#include "walls.hpp"
#include "tiles.hpp"
#include "goal.hpp"
#include <UIKit/UIKit.h>

namespace battlemints {

typedef thing *(*thing_reader)(Json::Value const &v);
typedef std::pair<std::string, thing_reader> thing_reader_pair;

static const boost::array<thing_reader_pair, 18> _thing_reader_pairs = {
    thing_reader_pair("wall",            &wall::from_json),
    thing_reader_pair("wallpost",        &wallpost::from_json),
    thing_reader_pair("player",          &player::from_json),
    thing_reader_pair("goal",            &goal::from_json),
    thing_reader_pair("mini",            &mini::from_json),
    thing_reader_pair("mega",            &mega::from_json),
    thing_reader_pair("bumper",          &bumper::from_json),
    thing_reader_pair("powerup",         &powerup::from_json),
    thing_reader_pair("tile-octagon",    &tile_octagon::from_json),
    thing_reader_pair("tile-hexagon-90", &tile_hexagon_90::from_json),
    thing_reader_pair("tile-hexagon",    &tile_hexagon::from_json),
    thing_reader_pair("tile-trapezoid",  &tile_trapezoid::from_json),
    thing_reader_pair("tile-square",     &tile_square::from_json),
    thing_reader_pair("tile-rhombus-60", &tile_rhombus_60::from_json),
    thing_reader_pair("tile-rhombus-45", &tile_rhombus_45::from_json),
    thing_reader_pair("tile-rhombus-30", &tile_rhombus_30::from_json),
    thing_reader_pair("tile-triangle",   &tile_triangle::from_json),
    thing_reader_pair("arrow",           &arrow::from_json)
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
