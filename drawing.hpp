#ifndef __DRAWING_HPP__
#define __DRAWING_HPP__

#include <CoreGraphics/CoreGraphics.h>
#include <boost/optional.hpp>
#include <string>

namespace battlemints {

CGContextRef make_bitmap_context(unsigned w, unsigned h, void *data);
CGImageRef   make_image(char const *filename);

boost::optional<std::string> resource_filename(std::string const &name, std::string const &type);

}

#endif
