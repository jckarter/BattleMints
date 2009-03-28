#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <map>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include <sys/errno.h>
#include "geom.hpp"

namespace battlemints {

struct thing;

namespace serialization {
    enum flags {
        SPAWN = 1
    };
}

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type);
std::string data_filename(std::string const &name);

thing *thing_from_bin(FILE *bin);

struct invalid_board : std::runtime_error {
    invalid_board(std::string const &s) : std::runtime_error(s) {}
};

void _safe_fread(void *p, size_t size, size_t nmemb, FILE *stream);
void _safe_fwrite(void const *p, size_t size, size_t nmemb, FILE *stream);

template<typename T>
inline void safe_fread(T *p, size_t size, size_t nmemb, FILE *stream)
    { _safe_fread((void*)p, size, nmemb, stream); }

template<typename T>
inline void safe_fwrite(T const *p, size_t size, size_t nmemb, FILE *stream)
    { _safe_fread((void const *)p, size, nmemb, stream); }

#define BATTLEMINTS_READ_SLOTS(object, from_slot, through_slot, stream) \
    (::battlemints::safe_fread(&((object).from_slot), (char*)(&((object).through_slot)+1) - (char*)&((object).from_slot), 1, stream))

boost::optional<std::string> pascal_string_from_bin(FILE *bin);

template<typename T>
inline T data_from_bin(FILE *bin)
{
    T r;
    safe_fread(&r, sizeof(r), 1, bin);

    return r;
}

template<typename T>
inline void n_from_bin(FILE *bin, T *into, int n)
{
    safe_fread((void*)into, sizeof(*into), n, bin);
}

}

#endif
