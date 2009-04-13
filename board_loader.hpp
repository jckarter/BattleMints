#ifndef __LOADER_HPP__
#define __LOADER_HPP__

#include <pthread.h>
#include "controller.hpp"

namespace battlemints {

struct board;

struct board_name {
    int world;
    int stage;
    
    std::string filename() const
        { return boost::lexical_cast<std::string>(world)
            + "-"
            + boost::lexical_cast<std::string>(stage); }

    std::string description() const
    {
        std::string r =  "World " + boost::lexical_cast<std::string>(world);
        if (stage != 0)
            r += "-" + boost::lexical_cast<std::string>(stage);
        return r;
    }

    static board_name make(int w, int s) { return (board_name){ w, s }; }

    board_name with_stage(int s) const { return (board_name){ world, s }; }

    int goal_number() const { return world*16 + stage; }
};

struct board_loader : controller {
    bool finished;
    pthread_t thread;
    board_name name;
    board *b;

    board_loader(board_name const &n)
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
