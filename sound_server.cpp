#include "sound_server.hpp"
#include "board.hpp"
#include "thing.hpp"
#include <OpenAL/al.h>
#include <vector>
#include <stdexcept>
#include <boost/format.hpp>
#include <cstdio>

namespace battlemints {

sound_server *sound_server::_current = NULL;

sound_effect::sound_effect(unsigned num_buffers, unsigned buffer_size)
    : _buffers(num_buffers), _buffer_size(buffer_size), _buffer_n(0)
{
    alGenBuffers(num_buffers, &_buffers[0]);
}

sound_effect::~sound_effect()
{
    alDeleteBuffers(_buffers.size(), &_buffers[0]);
}

sound_server::sound_server(unsigned num_voices)
    : _device(NULL), _context(NULL), _sources(num_voices)
{
    _start_openal();

    if (!_context)
        return;

    std::vector<ALuint> sources_v(num_voices, 0);

    alGenSources(num_voices, &sources_v[0]);
    std::copy(sources_v.begin(), sources_v.end(), _sources.begin());
}

sound_server::~sound_server()
{
    if (!_context)
        return;

    BOOST_FOREACH (playing_sound const &s, _sounds)
        delete s.effect;

    std::vector<ALuint> sources_v(_sources.begin(), _sources.end());

    alDeleteSources(sources_v.size(), &sources_v[0]);

    _finish_openal();
}

static inline bool
_is_source_available(ALuint source)
{
    ALint processed, queued, state;
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);
    alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return processed == 0 && queued == 0 && state != AL_PLAYING;
}

inline ALuint
sound_server::_find_available_source()
{
    for (unsigned i = 0; i < _sources.size(); ++i) {
        ALuint source = _next_source();
        if (_is_source_available(source))
            return source;
    }
    return 0;
}

inline ALuint
sound_server::_next_source()
{
    ALuint source = _sources[0];
    _sources.pop_front();
    _sources.push_back(source);
    return source;
}

inline bool
sound_server::_fill_buffer(sound_effect *effect, ALuint buffer)
{
    std::vector<int16_t> buffer_data(effect->_buffer_size);
    bool left = effect->fill_buffer(
        &buffer_data[0],
        effect->_buffer_n++ * effect->_buffer_size,
        effect->_buffer_size
    );
    alBufferData(
        buffer,
        AL_FORMAT_MONO16,
        (void*)&buffer_data[0],
        sizeof(int16_t) * effect->_buffer_size,
        44100
    );
    return left;
}

inline void
sound_server::_start_sound(playing_sound &ps)
{
    BOOST_FOREACH (ALuint buffer, ps.effect->_buffers) {
        ps.buffers_left = _fill_buffer(ps.effect, buffer);
        alSourceQueueBuffers(ps.source, 1, &buffer);
        if (!ps.buffers_left) break;
    }
}

inline void
sound_server::_stop_sound(playing_sound &ps)
{
    alSourceStop(ps.source);
    ALint processed;
    alGetSourcei(ps.source, AL_BUFFERS_PROCESSED, &processed);
    while (processed-- > 0) {
        ALuint buffer;
        alSourceUnqueueBuffers(ps.source, 1, &buffer);
    }
    delete ps.effect;
}

bool
sound_server::_tick_ps(playing_sound &ps)
{
    // XXX when thing dies?
    alSource3f(ps.source, AL_POSITION, ps.origin->center.x, ps.origin->center.y, 0.0f);
    alSource3f(ps.source, AL_VELOCITY, ps.origin->velocity.x, ps.origin->velocity.y, 0.0f);

    if (ps.buffers_left) {
        ALint processed;
        alGetSourcei(ps.source, AL_BUFFERS_PROCESSED, &processed);
        while (processed-- > 0 && ps.buffers_left) {
            ALuint buffer;
            alSourceUnqueueBuffers(ps.source, 1, &buffer);
            ps.buffers_left = _fill_buffer(ps.effect, buffer);
            alSourceQueueBuffers(ps.source, 1, &buffer);
        }
        ALint state;
        alGetSourcei(ps.source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
            alSourcePlay(ps.source);
        return true;
    } else {
        ALint processed;
        alGetSourcei(ps.source, AL_BUFFERS_PROCESSED, &processed);
        while (processed-- > 0) {
            ALuint buffer;
            alSourceUnqueueBuffers(ps.source, 1, &buffer);
        }
        return !_is_source_available(ps.source);
    }
}

void
sound_server::play(sound_effect *sound, thing *thing)
{
    if (!_context)
        return;

    playing_sound ps;
    ps.source = _find_available_source();
    if (ps.source != 0) {
        ps.effect = sound;
        ps.origin = thing;
        ps.buffers_left = true;

        _start_sound(ps);
        _sounds.push_back(ps);
    } else {
        delete sound;
    }
}

void
sound_server::tick()
{
    if (!_context)
        return;

    if (board::current()) {
        vec2 listener_center = board::current()->camera_center();
        vec2 listener_velocity = board::current()->camera_velocity();

        alListener3f(AL_POSITION, listener_center.x, listener_center.y, 0.0f);
        alListener3f(AL_VELOCITY, listener_velocity.x, listener_velocity.y, 0.0f);
    }

    for (std::vector<playing_sound>::iterator i = _sounds.begin(); i != _sounds.end(); ) {
        if (!_tick_ps(*i)) {
            _stop_sound(*i);
            _sounds.erase(i);
        } else
            ++i;
    }
}

void
sound_server::_start_openal()
{
    _device = alcOpenDevice(NULL);
    if (!_device)
        return;

    _context = alcCreateContext(_device, NULL);
    if (!_context) {
        alcCloseDevice(_device);
        _device = NULL;
        return;
    }

    alcMakeContextCurrent(_context);
}

void
sound_server::_finish_openal()
{
    alcDestroyContext(_context);
    alcCloseDevice(_device);
}

}
