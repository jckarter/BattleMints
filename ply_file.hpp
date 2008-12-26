#ifndef __PLY_FILE_HPP__
#define __PLY_FILE_HPP__

#ifndef NO_GRAPHICS
#include <OpenGLES/ES1/gl.h>
#include <OpenGLES/ES1/glext.h>
#else
typedef float GLfloat;
typedef unsigned short GLushort;
#endif
#include <string>
#include <vector>

namespace battlemints {

#ifndef NO_GRAPHICS
bool read_ply(
    std::string const &name,
    std::vector<GLfloat> &out_vertices,
    std::vector<GLushort> &out_elements
);
#endif

bool read_ply_file(
    std::string const &filename,
    std::vector<GLfloat> &out_vertices,
    std::vector<GLushort> &out_elements
);

}

#endif

