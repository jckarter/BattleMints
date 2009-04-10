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

static const boost::array<thing_reader_pair, 24> _thing_reader_pairs = {
    thing_reader_pair("bomb",             &thing::from_bin<bomb>),
    thing_reader_pair("player",           &thing::from_bin<player>),
    thing_reader_pair("pellet",           &thing::from_bin<pellet>),
    thing_reader_pair("mini",             &thing::from_bin<mini>),
    thing_reader_pair("shield_mini",      &thing::from_bin<shield_mini>),
    thing_reader_pair("mega",             &thing::from_bin<mega>),
    thing_reader_pair("bumper",           &thing::from_bin<bumper>),
    thing_reader_pair("powerup",          &thing::from_bin<powerup>),
    thing_reader_pair("wall",             &thing::from_bin<wall>),
    thing_reader_pair("wallpost",         &thing::from_bin<wallpost>),
    thing_reader_pair("goal",             &thing::from_bin<goal>),
    thing_reader_pair("alarm",            &thing::from_bin<alarm>),
    thing_reader_pair("door",             &thing::from_bin<door>),
    thing_reader_pair("tile",             &thing::from_bin<tile>),
    thing_reader_pair("tile_vertices",    &thing::from_bin<tile_vertices>),
    thing_reader_pair("sign",             &thing::from_bin<sign>),
    thing_reader_pair("switch",           &thing::from_bin<trigger_switch>),
    thing_reader_pair("heavy_switch",     &thing::from_bin<heavy_switch>),
    thing_reader_pair("loader",           &thing::from_bin<loader>),
    thing_reader_pair("eraser",           &thing::from_bin<eraser_switch>),
    thing_reader_pair("durian",           &thing::from_bin<durian>),
    thing_reader_pair("battlemints_flag", &thing::from_bin<battlemints_flag>),
    thing_reader_pair("start_banner",     &thing::from_bin<start_banner>),
    thing_reader_pair("protip",           &thing::from_bin<protip>)
};

static const std::map<std::string, thing_reader> _thing_readers
    (_thing_reader_pairs.begin(), _thing_reader_pairs.end());

bool _safe_fread(void *p, size_t size, size_t nmemb, FILE *stream)
{
    size_t count = fread(p, size, nmemb, stream);
    while (count != nmemb) {
        if (ferror(stream) && errno == EINTR) {
            clearerr(stream);
            nmemb -= count;
            p = (void*)((char*)p + size * nmemb);
            count = fread(p, size, nmemb, stream);
        }
        else if (feof(stream))
            return false;
        else
            throw invalid_board(
                "fread got " + boost::lexical_cast<std::string>(count) + " records but expected "
                    + boost::lexical_cast<std::string>(size * nmemb) + " records"
            );
    }
    return true;
}

bool _safe_fwrite(void const *p, size_t size, size_t nmemb, FILE *stream)
{
    size_t count = fwrite(p, size, nmemb, stream);
    while (count != nmemb) {
        if (ferror(stream) && errno == EINTR) {
            clearerr(stream);
            nmemb -= count;
            p = (void const *)((char const *)p + size * nmemb);
            count = fwrite(p, size, nmemb, stream);
        }
        else if (feof(stream))
            return false;
        else
            throw invalid_board(
                "fwrite got " + boost::lexical_cast<std::string>(count) + " records but expected "
                    + boost::lexical_cast<std::string>(size * nmemb) + " records"
            );
    }
    return true;
}

template<typename LengthType>
boost::optional<std::string> _pascal_string_from_bin(FILE *bin)
{
    LengthType length;
    if (!safe_fread(&length, sizeof(length), 1, bin))
        return boost::optional<std::string>();

    std::string r(length, '\0');

    if (length > 0)
        safe_fread(&r[0], length, 1, bin);

    return r;
}

boost::optional<std::string> pascal_string_from_bin(FILE *bin)
{
    return _pascal_string_from_bin<uint8_t>(bin);
}

boost::optional<std::string> long_pascal_string_from_bin(FILE *bin)
{
    return _pascal_string_from_bin<uint16_t>(bin);
}

thing *thing_from_bin(FILE *bin)
{
    boost::optional<std::string> kind = pascal_string_from_bin(bin);
    if (!kind)
        return NULL;

    uint32_t flags = data_from_bin<uint32_t>(bin);

    std::map<std::string, thing_reader>::const_iterator reader
        = _thing_readers.find(*kind);

    thing *th;
    if (reader != _thing_readers.end())
        th = (reader->second)(bin);
    else
        throw invalid_board("unknown kind of thing \"" + *kind + "\"");

    if (flags & serialization::SPAWN)
        th = new spawn(th);

    return th;
}

static inline NSString *objc_str(std::string const &s)
{
    return [NSString stringWithUTF8String:s.c_str()];
}

std::string data_filename(std::string const &name)
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documents = [paths objectAtIndex:0];

    return std::string([[documents stringByAppendingPathComponent:objc_str(name)] UTF8String]);
}

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type)
{
    NSString *s = [[NSBundle mainBundle] pathForResource:objc_str(name) ofType:objc_str(type)];

    return s ? boost::optional<std::string>([s UTF8String])
             : boost::optional<std::string>();
}

}
