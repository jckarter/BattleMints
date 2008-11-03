#ifndef __DRAWING_HPP__
#define __DRAWING_HPP__

#include <CoreGraphics/CoreGraphics.h>

namespace battlemints {

CGContextRef make_bitmap_context(unsigned w, unsigned h, void *data);
CGImageRef   make_image(char const *filename);

}

#endif
