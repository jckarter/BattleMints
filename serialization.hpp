#ifndef __SERIALIZATION_HPP__
#define __SERIALIZATION_HPP__

#include <map>
#include <string>
#include <vector>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <cstdio>
#include "geom.hpp"

namespace battlemints {

struct thing;

namespace serialization {
    enum flags {
        SPAWN = 1
    };
}

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type);

thing *thing_from_bin(FILE *bin);

struct invalid_board : std::runtime_error {
    invalid_board(std::string const &s) : std::runtime_error(s) {}
};

typename<typename T>
inline void safe_fread(T *p, size_t size, size_t nmemb, FILE *stream)
{
    void *vp = (void*)p;
    size_t count = fread(vp, size, nmemb, stream);
    while (count != nmemb) {
        if (ferror(stream) && errno == EINTR) {
            clearerr(stream);
            nmemb -= count;
            vp = (void*)((char*)vp + size * nmemb);
            count = fread(vp, size, nmemb, stream);
        }
        else
            throw invalid_board(
                "fread got " + boost::lexical_cast<std::string>(count) + " records but expected "
                    + boost::lexical_cast<std::string>(size * nmemb) + " records"
            );
    }
}

inline std::string pascal_string_from_bin(FILE *bin)
{
    char length = getc(bin);
    std::string r(length, '\0');
    safe_fread(&r[0], length, 1, bin);

    return r;
}

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
    safe_fread(&r, n*sizeof(r), 1, bin);
}

}

#endif
