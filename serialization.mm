#include "serialization.hpp"
#include "dramatis_personae.hpp"
#include "walls.hpp"
#include "tiles.hpp"
#include "tripwire.hpp"
#include <UIKit/UIKit.h>

namespace battlemints {

typedef thing *(*thing_reader)(Json::Value const &v);
typedef std::pair<std::string, thing_reader> thing_reader_pair;

static const boost::array<thing_reader_pair, 12> _thing_reader_pairs = {
    thing_reader_pair("wall",            &wall::from_json),
    thing_reader_pair("wallpost",        &wallpost::from_json),
    thing_reader_pair("player",          &player::from_json),
    thing_reader_pair("goal",            &goal::from_json),
    thing_reader_pair("alarm",           &alarm::from_json),
    thing_reader_pair("mini",            &mini::from_json),
    thing_reader_pair("mega",            &mega::from_json),
    thing_reader_pair("bumper",          &bumper::from_json),
    thing_reader_pair("powerup",         &powerup::from_json),
    thing_reader_pair("tile",            &tile::from_json),
    thing_reader_pair("tile_vertices",   &tile_vertices::from_json),
    thing_reader_pair("sign",            &sign::from_json)
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

    thing *th;
    if (reader != _thing_readers.end())
        th = (reader->second)(params);
    else
        throw invalid_board_json(std::string("unknown kind of thing \"" + kind + "\""));

    if (params["spawn"].asBool())
        th = new spawn(th);

    if (params.isMember("label"))
        th.label = intern(params.asString());

    return th;
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
