#include "board_loader.hpp"
#include "board.hpp"

namespace battlemints {

void board_loader::setup()
{
}

void board_loader::tick()
{
    if (finished)
        controller::set_current(b);
}

void board_loader::draw()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void *board_loader::_loader_thread(void *void_thys)
{
    board_loader *thys = (board_loader*)void_thys;

    thys->b = board::from_file(thys->name);
    thys->  finished = true;

    return NULL;
}

}
