#ifndef SOUND_SERVER_HPP
#define SOUND_SERVER_HPP

#include <boost/utility.hpp>
#include <boost/cstdint.hpp>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <deque>
#include "geom.hpp"

namespace battlemints {

struct sound_effect : boost::noncopyable {

    sound_effect(unsigned num_buffers, );
    ~sound_effect();

    void tick();

protected:
    virtual bool fill_buffer(int16_t *buffer, unsigned start, unsigned length);

private:
    std::deque<ALuint> _buffers;
};

struct sound_server : boost::noncopyable {

    sound_server(unsigned num_voices = 16);
    ~sound_server();

    void play(sound_effect const & sound, vec2 position, vec2 velocity, float gain);

    static sound_server *current() { return _current; }

private:
    static sound_server *_current;

    ALCdevice *_device;
    ALCcontext *_context;
    std::deque<ALuint> _sources;

    ALuint _find_available_source();
    ALuint _next_source();

    void _start_openal();
    void _finish_openal();
};

}

#endif
