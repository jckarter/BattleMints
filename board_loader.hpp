#ifndef __LOADER_HPP__
#define __LOADER_HPP__

#include <pthread.h>
#include "controller.hpp"
#include "board.hpp"

namespace battlemints {

struct board_loader : controller {
    bool finished;
    pthread_t thread;
    std::string name;
    board *b;

    board_loader(std::string const &n)
        : finished(false), thread(NULL), name(n), b(NULL)
        { pthread_create(&thread, NULL, &_loader_thread, (void*)this); }

    virtual void setup();
    virtual void tick();
    virtual void draw();

private:
    static void *_loader_thread(void *void_thys);
};

}

#endif
