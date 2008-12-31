#include <rply.h>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <boost/array.hpp>
#include <boost/utility.hpp>
#include "ply_file.hpp"
#ifndef NO_GRAPHICS
# include "serialization.hpp"
#endif

namespace battlemints {

struct read_state {
    std::vector<GLfloat> &vertices;
    std::vector<GLushort> &elements;

    long current_vertex_index, current_face_index;
    boost::array<GLfloat, 3> current_vertex;
    boost::array<GLushort,  3> current_face;

    std::exception *error;

    read_state(std::vector<GLfloat> &vv, std::vector<GLushort> &ee)
        : vertices(vv), elements(ee),
          current_vertex_index(0), current_face_index(0),
          error(NULL)
    {
        std::fill(current_vertex.begin(), current_vertex.end(), 0.0f);
        std::fill(current_face.begin(),   current_face.end(),   0);
    }

    void push_vertex()
    {
        std::copy(current_vertex.begin(), current_vertex.end(), std::back_inserter(vertices));
        std::fill(current_vertex.begin(), current_vertex.end(), 0.0f);
    }

    void push_face()
    {
        std::copy(current_face.begin(), current_face.end(), std::back_inserter(elements));
        std::fill(current_face.begin(), current_face.end(), 0);
    }
};

struct ply_handle : boost::noncopyable {
    p_ply handle;

    ply_handle(p_ply h) : handle(h) { if (!handle) throw std::runtime_error("ply_open failed"); }

    ~ply_handle() { ply_close(handle); }
};

namespace {

    int _read_vertex(p_ply_argument arg)
    {
        read_state * state;
        long vertex_index, vertex_n;

        ply_get_argument_user_data(arg, (void**)&state, &vertex_n);
        ply_get_argument_element(arg, NULL, &vertex_index);

        if (vertex_index != state->current_vertex_index) {
            state->current_vertex_index = vertex_index;
            state->push_vertex();
        }
        state->current_vertex[vertex_n] = ply_get_argument_value(arg);

        return 1;
    }

    int _read_face(p_ply_argument arg)
    {
        read_state * state;
        long face_index, face_length, face_n;
        
        ply_get_argument_user_data(arg, (void**)&state, NULL);
        ply_get_argument_element(arg, NULL, &face_index);
        ply_get_argument_property(arg, NULL, &face_length, &face_n);

        if (face_length != 3) {
            state->error = new std::runtime_error("Only triangular faces supported");
            return 0;
        }

        if (face_index != state->current_face_index) {
            state->current_face_index = face_index;
            state->push_face();
        }

        if (face_n >= 0)
            state->current_face[face_n] = ply_get_argument_value(arg);

        return 1;
    }

}

#ifndef NO_GRAPHICS
bool read_ply(
    std::string const &name,
    std::vector<GLfloat> &out_vertices,
    std::vector<GLushort> &out_elements
)
{
    try {
        boost::optional<std::string> path = resource_filename(name, "ply");
        return read_ply_file(*path, out_vertices, out_elements);
    } catch (std::exception const &x) {
        std::cerr << "Reading ply " << name << " failed: " << x.what() << "\n";
        return false;
    }
}
#endif

bool
read_ply_file(
    std::string const &filename,
    std::vector<GLfloat> &out_vertices,
    std::vector<GLushort> &out_elements
)
{
    try {
        ply_handle ply(ply_open(filename.c_str(), NULL));

        if (!ply_read_header(ply.handle))
            throw std::runtime_error("ply_read_header failed");

        read_state state(out_vertices, out_elements);
        long vertices_count, faces_count;

        vertices_count = ply_set_read_cb(ply.handle, "vertex", "x", &_read_vertex, &state, 0);
        ply_set_read_cb(ply.handle, "vertex", "y", &_read_vertex, &state, 1);
        ply_set_read_cb(ply.handle, "vertex", "z", &_read_vertex, &state, 2);

        faces_count = ply_set_read_cb(ply.handle, "face", "vertex_indices", &_read_face, &state, 0);

        if (!ply_read(ply.handle))
            // XXX exception leaks!
            throw state.error ? *state.error : std::runtime_error("ply_read failed");
        state.push_vertex();
        state.push_face();

        if (out_vertices.size() != vertices_count * 3)
            throw std::runtime_error("Wrong number of vertices extracted");
        if (out_elements.size() != faces_count * 3)
            throw std::runtime_error("Wrong number of faces extracted");

        return true;

    } catch (std::exception const &x) {
        std::cerr << "Reading ply file " << filename << " failed: " << x.what() << "\n";
        return false;
    } catch (...) {
        std::cerr << "Reading ply file " << filename << " failed: ...\n";
        return false;
    }
}

}
