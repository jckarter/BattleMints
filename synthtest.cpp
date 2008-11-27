#include "synth.hpp"
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <cstdio>
#include <cstring>
#include <unistd.h>

float attack() { return 0.02; }
float decay() { return 0.03; }

void
save_wav_file(FILE *f, void const *data, unsigned size)
{
    struct riff_wave_header {
        char magic[4];
        uint32_t size;
        char kind[4];

        char fmt_magic[4];
        uint32_t fmt_size;
        uint16_t fmt_compression_code;
        uint16_t fmt_channels;
        uint32_t fmt_sample_rate;
        uint32_t fmt_bytes_sec;
        uint16_t fmt_block_align;
        uint16_t fmt_significant_bits;

        char data_magic[4];
        uint32_t data_size;
    } header = {
        { 'R', 'I', 'F', 'F' },
        size + sizeof(riff_wave_header) - 8,
        { 'W', 'A', 'V', 'E' },

        { 'f', 'm', 't', ' ' },
        16,
        1,
        1,
        44100,
        88100,
        2,
        16,

        { 'd', 'a', 't', 'a' },
        size
    };

    fwrite((void*)&header, 1, sizeof(header), f);
    fwrite(data, 1, size, f);
}

int
main(int argc, char *argv[])
{
    sranddev();

    using namespace battlemints;
    
    int16_t buffer_data[44100];
    memset((void*)buffer_data, 0, sizeof(buffer_data));

    synth<
        sine_wave_synth<fraction<1,7> >,
        sine_wave_synth<fraction<1,7>, fraction<30,9> >,
        sine_wave_synth<fraction<9,70>, fraction<16,3> >,
        sine_wave_synth<fraction<8,70>, fraction<70,9> >,
        sine_wave_synth<fraction<6,70>, fraction<80,9> >,
        sine_wave_synth<fraction<5,70>, fraction<12,1> >,
        sine_wave_synth<fraction<5,70>, fraction<40,3> >,
        envelope<attack, decay>
    >(buffer_data, 0, 7056, 1.0f, 450.0f, 0.0f);

    if (argc > 1) {
        FILE *file = fopen(argv[1], "w");
        if (file) {
            save_wav_file(file, (void*)buffer_data, sizeof(buffer_data));
            fclose(file);
        }
    }

    ALCdevice *d = alcOpenDevice(NULL);
    ALCcontext *c = NULL;
    if (d) {
        c = alcCreateContext(d, NULL);
        alcMakeContextCurrent(c);
    }

    ALCenum er = alcGetError(d);
    if (er != ALC_NO_ERROR) {
        fprintf(stderr, "Error 0x%X setting up context\n", er);
        return 1;
    }

    alListener3f(AL_POSITION, 0.0, 0.0, 0.0);
    alListener3f(AL_VELOCITY, 0.0, 0.0, 0.0);
    alListenerf(AL_GAIN, 1.0);

    ALuint buffer;
    alGenBuffers(1, &buffer);
    alBufferData(buffer, AL_FORMAT_MONO16, (ALvoid*)buffer_data, sizeof(buffer_data), 44100);

    ALuint source;
    alGenSources(1, &source);
    alSource3f(source, AL_POSITION, 0.0, 0.0, 0.0);
    alSource3f(source, AL_VELOCITY, 0.0, 0.0, 0.0);
    alSourcef(source, AL_GAIN, 1.0);
    alSourcei(source, AL_BUFFER, buffer);
    fprintf(stderr, "GO!\n");
    alSourcePlay(source);

    ALint state;
    do {
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        sleep(0);
    } while (state == AL_PLAYING);

    fprintf(stderr, "All Done!\n");
    return 0;
}
