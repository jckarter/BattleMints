#ifndef SOUND_SERVER_HPP
#define SOUND_SERVER_HPP

#include <boost/utility.hpp>
#include <boost/cstdint.hpp>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <deque>
#include <utility>
#include "geom.hpp"

namespace battlemints {

struct thing;

struct sound_effect : boost::noncopyable {

    sound_effect(unsigned num_buffers, unsigned buffer_size);
    ~sound_effect();

    virtual bool fill_buffer(int16_t *buffer, unsigned start, unsigned length) = 0;

protected:
    unsigned _buffer_size, _buffer_n;

private:
    friend class sound_server;

    std::vector<ALuint> _buffers;
};

struct sound_server : boost::noncopyable {

    sound_server(unsigned num_voices = 16);
    ~sound_server();

    void play(sound_effect *sound, thing *thing);
    void tick();

private:
    static sound_server *_current;

    struct playing_sound {
        ALuint source;
        sound_effect *effect;
        thing *origin;
        bool buffers_left;
    };

    ALCdevice *_device;
    ALCcontext *_context;

    std::deque<ALuint> _sources;
    std::vector<playing_sound> _sounds;

    ALuint _find_available_source();
    ALuint _next_source();
    bool _fill_buffer(sound_effect *effect, ALuint buffer);
    void _start_sound(playing_sound &ps);
    void _stop_sound(playing_sound &ps);
    bool _tick_ps(playing_sound &ps);

    void _start_openal();
    void _finish_openal();
};

}

#endif
