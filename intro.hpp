#ifndef __INTRO_HPP__
#define __INTRO_HPP__

#include "controller.hpp"
#include "drawing.hpp"

namespace battlemints {

struct intro : controller {
    static const boost::array<float, 8> texcoords;
    
    intro();
    virtual ~intro();

    virtual void setup();
    virtual void tick();
    virtual void draw();

private:
    float _intensity() { return (float)_countdown/(float)_peak; }

    image_texture *_splash_texture;
    int _countdown, _peak;
};

}

#endif
