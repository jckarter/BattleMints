#include "sound_server.hpp"
#include <OpenAL/al.h>
#include <vector>
#include <stdexcept>
#include <boost/format.hpp>

namespace battlemints {

sound_server *sound_server::_current = NULL;

sound_server::sound_server(unsigned num_voices)
    : _device(NULL), _context(NULL), _sources(num_voices)
{
    _start_openal();

    if (!_context)
        return;

    std::vector<ALuint> sources_v(num_voices, 0);

    alGenSources(num_voices, &sources_v[0]);
    std::copy(sources_v.begin(), sources_v.end(), _sources.begin());

    _current = this;
}

sound_server::~sound_server()
{
    if (!_context)
        return;

    std::vector<ALuint> sources_v(_sources.begin(), _sources.end());

    alDeleteSources(sources_v.size(), &sources_v[0]);

    _finish_openal();
}

inline ALuint
sound_server::_find_available_source()
{
    for (unsigned i = 0; i < _sources.size(); ++i) {
        ALuint source = _next_source();
        ALint state;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state != AL_PLAYING)
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

void
sound_server::play(sound_effect const & sound, vec2 position, vec2 velocity, float gain)
{
    if (!_context)
        return;

    ALuint source = _find_available_source();

    if (source != 0) {
        alSourcei(source, AL_BUFFER, sound.buffer);
        alSource3f(source, AL_POSITION, position.x, position.y, 0.0f);
        alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, 0.0f);
        alSourcef(source, AL_GAIN, gain);
        alSourcePlay(source);
    }
}

void
sound_server::listener(vec2 position, vec2 velocity, float gain)
{
    if (!_context)
        return;

    alListener3f(AL_POSITION, position.x, position.y, 0.0f);
    alListener3f(AL_VELOCITY, velocity.x, velocity.y, 0.0f);
    alListenerf(AL_GAIN, gain);
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
