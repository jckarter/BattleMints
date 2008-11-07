#include "drawing.hpp"
#include <UIKit/UIKit.h>
#include <CoreGraphics/CoreGraphics.h>

namespace battlemints {

CGContextRef make_bitmap_context(unsigned w, unsigned h, void *data)
{
    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    CGContextRef context = CGBitmapContextCreate(
        data, w, h, 8, w*4,
        colorspace, kCGImageAlphaPremultipliedLast
    );
    CGColorSpaceRelease(colorspace);
    return context;
}

CGImageRef make_image(char const *filename)
{
    UIImage *ui_image = [UIImage imageNamed:[NSString stringWithUTF8String:filename]];
    CGImageRef ret = [ui_image CGImage];
    CGImageRetain(ret);
    return ret;
}

}
