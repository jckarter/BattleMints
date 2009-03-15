#include "serialization.hpp"
#include "dramatis_personae.hpp"
#include "walls.hpp"
#include "tiles.hpp"
#include "tripwire.hpp"
#include "decorations.hpp"
#include <UIKit/UIKit.h>

namespace battlemints {

typedef thing *(*thing_reader)(FILE *bin);
typedef std::pair<std::string, thing_reader> thing_reader_pair;

static const boost::array<thing_reader_pair, 15> _thing_reader_pairs = {
    thing_reader_pair("player",          &thing::from_bin<player>),
    thing_reader_pair("pellet",          &thing::from_bin<pellet>)
    thing_reader_pair("mini",            &thing::from_bin<mini>),
    thing_reader_pair("mega",            &thing::from_bin<mega>),
    thing_reader_pair("bumper",          &thing::from_bin<bumper>),
    thing_reader_pair("powerup",         &thing::from_bin<powerup>),
    thing_reader_pair("wall",            &thing::from_bin<wall>),
    thing_reader_pair("wallpost",        &thing::from_bin<wallpost>),
    thing_reader_pair("goal",            &thing::from_bin<goal>),
    thing_reader_pair("alarm",           &thing::from_bin<alarm>),
    thing_reader_pair("door",            &thing::from_bin<door>),
    thing_reader_pair("tile",            &thing::from_bin<tile>),
    thing_reader_pair("tile_vertices",   &thing::from_bin<tile_vertices>),
    thing_reader_pair("sign",            &thing::from_bin<sign>),
    thing_reader_pair("switch",          &thing::from_bin<switch_spring>)
};

static const std::map<std::string, thing_reader> _thing_readers
    (_thing_reader_pairs.begin(), _thing_reader_pairs.end());

thing *thing_from_bin(FILE *bin)
{
    std::string kind = pascal_string_from_bin(bin);
    uint32_t flags = data_from_bin<uint32_t>(bin);

    std::map<std::string, thing_reader>::const_iterator reader
        = _thing_readers.find(kind);

    thing *th;
    if (reader != _thing_readers.end())
        th = (reader->second)(bin);
    else
        throw invalid_board("unknown kind of thing \"" + kind + "\"");

    if (flags & serialization::SPAWN)
        th = new spawn(th);

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
