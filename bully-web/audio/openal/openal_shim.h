// audio/openal/openal_shim.h
// OpenAL shim — mapeia para Web Audio API (AudioWorklet) via Emscripten.
#pragma once
#include <cstdint>

typedef int32_t ALint;
typedef uint32_t ALuint;
typedef float ALfloat;
typedef int32_t ALenum;
typedef char ALboolean;
typedef int32_t ALsizei;
typedef int32_t ALbitfield;
typedef void ALvoid;

typedef struct ALCdevice_struct ALCdevice;
typedef struct ALCcontext_struct ALCcontext;

namespace bully::openal {

// Sources
void alGenSources(ALsizei n, ALuint* sources);
void alDeleteSources(ALsizei n, const ALuint* sources);
void alSourcePlay(ALuint source);
void alSourceStop(ALuint source);
void alSourcePause(ALuint source);
void alSourceRewind(ALuint source);
void alSourcei(ALuint source, ALenum param, ALint value);
void alSourcef(ALuint source, ALenum param, ALfloat value);
void alSource3f(ALuint source, ALenum param, ALfloat x, ALfloat y, ALfloat z);
void alGetSourcei(ALuint source, ALenum param, ALint* value);
void alGetSourcef(ALuint source, ALenum param, ALfloat* value);
void alGetSource3f(ALuint source, ALenum param, ALfloat* x, ALfloat* y, ALfloat* z);
void alSourceQueueBuffers(ALuint source, ALsizei n, const ALuint* buffers);
void alSourceUnqueueBuffers(ALuint source, ALsizei n, ALuint* buffers);

// Buffers
void alGenBuffers(ALsizei n, ALuint* buffers);
void alDeleteBuffers(ALsizei n, const ALuint* buffers);
void alBufferData(ALuint buffer, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq);
void alGetBufferi(ALuint buffer, ALenum param, ALint* value);

// Listener
void alListener3f(ALenum param, ALfloat x, ALfloat y, ALfloat z);
void alListenerfv(ALenum param, const ALfloat* values);

// Filters
void alGenFilters(ALsizei n, ALuint* filters);
void alDeleteFilters(ALsizei n, const ALuint* filters);
void alFilteri(ALuint filter, ALenum param, ALint value);
void alFilterf(ALuint filter, ALenum param, ALfloat value);
ALboolean alIsFilter(ALuint filter);

// State
ALenum alGetError();

// ALC
ALCdevice* alcOpenDevice(const char* devicename);
ALCcontext* alcCreateContext(ALCdevice* device, const ALint* attrlist);
ALCboolean alcMakeContextCurrent(ALCcontext* context);
ALCboolean alcCloseDevice(ALCdevice* device);
ALCboolean alcIsExtensionPresent(ALCdevice* device, const char* extname);

void register_openal_symbols(class bully::elf::Linker& linker);

} // namespace bully::openal
