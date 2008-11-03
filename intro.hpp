#ifndef __INTRO_HPP__
#define __INTRO_HPP__

#include "controller.hpp"

namespace battlemints {

struct intro : controller {
    static const boost::array<float, 8> texcoords;
    static const boost::array<float, 8> vertices;
    
    intro();
    virtual ~intro();

    virtual void setup();
    virtual void tick();
    virtual void draw();

private:
    GLuint _load_splash_texture();

    float _intensity() { return (float)_countdown/(float)_peak; }

    GLuint _splash_texture;
    int _countdown, _peak;
};

}

#endif
