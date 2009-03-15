#ifndef __FINALLY_HPP__
#define __FINALLY_HPP__

#include <boost/function.hpp>

namespace battlemints {

template<typename T>
struct finally {
    typedef T value_type;
    typedef boost::function<void (T)> destructor_type;

    T object;
    destructor_type destructor;

    finally(T o, destructor_type d) : object(o), destructor(d) {}
    ~finally() { destructor(object); }
};

}

#endif
