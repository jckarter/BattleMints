#include "ply_file.hpp"
#include <iostream>
#include <boost/foreach.hpp>

using namespace battlemints;

template <typename Container>
void
three_out(std::ostream &os, Container const &c)
{
    int n = 0;
    BOOST_FOREACH (typename Container::const_reference elt, c) {
        os << elt << (++n % 3 == 0 ? "\n" : " ");
    }
}

int
main(int argc, char *argv[])
{
    if (argc < 2)
        std::cerr << "Usage: " << argv[0] << " ply_file\n";

    std::vector<GLfloat> vertices;
    std::vector<GLushort>  elements;

    if (!read_ply_file(argv[1], vertices, elements))
        return 1;

    std::cout << "Vertices:\n";
    three_out(std::cout, vertices);
    std::cout << "Elements:\n";
    three_out(std::cout, elements);
    
    return 0;
}
