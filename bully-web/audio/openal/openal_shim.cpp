// audio/openal/openal_shim.cpp
#include "openal_shim.h"
#include "../../runtime/elf/linker.hpp"
#include <cstdio>
#include <unordered_map>
#include <cstring>

// Em Emscripten: implementar sobre Web Audio (AudioWorklet).
// Em nativo: stubs que simulam estado (para testes do linker).

namespace bully::openal {

static ALCdevice g_device;
static ALCcontext g_context;
static std::unordered_map<ALuint, bool> g_sources;
static std::unordered_map<ALuint, bool> g_buffers;
static std::unordered_map<ALuint, bool> g_filters;
static ALuint g_next_id = 1;

void alGenSources(ALsizei n, ALuint* sources) {
    for (ALsizei i = 0; i < n; i++) {
        ALuint id = g_next_id++;
        g_sources[id] = true;
        if (sources) sources[i] = id;
    }
}
void alDeleteSources(ALsizei n, const ALuint* sources) {
    for (ALsizei i = 0; i < n; i++) if (sources) g_sources.erase(sources[i]);
}
void alSourcePlay(ALuint source) { (void)source; }
void alSourceStop(ALuint source) { (void)source; }
void alSourcePause(ALuint source) { (void)source; }
void alSourceRewind(ALuint source) { (void)source; }
void alSourcei(ALuint source, ALenum param, ALint value) { (void)source;(void)param;(void)value; }
void alSourcef(ALuint source, ALenum param, ALfloat value) { (void)source;(void)param;(void)value; }
void alSource3f(ALuint source, ALenum param, ALfloat x, ALfloat y, ALfloat z) { (void)source;(void)param;(void)x;(void)y;(void)z; }
void alGetSourcei(ALuint source, ALenum param, ALint* value) { (void)source;(void)param; if(value)*value=0; }
void alGetSourcef(ALuint source, ALenum param, ALfloat* value) { (void)source;(void)param; if(value)*value=0.0f; }
void alGetSource3f(ALuint source, ALenum param, ALfloat* x, ALfloat* y, ALfloat* z) {
    (void)source;(void)param; if(x)*x=0; if(y)*y=0; if(z)*z=0;
}
void alSourceQueueBuffers(ALuint source, ALsizei n, const ALuint* buffers) { (void)source;(void)n;(void)buffers; }
void alSourceUnqueueBuffers(ALuint source, ALsizei n, ALuint* buffers) {
    (void)source;
    if (buffers) for (ALsizei i = 0; i < n; i++) buffers[i] = 0;
}

void alGenBuffers(ALsizei n, ALuint* buffers) {
    for (ALsizei i = 0; i < n; i++) {
        ALuint id = g_next_id++;
        g_buffers[id] = true;
        if (buffers) buffers[i] = id;
    }
}
void alDeleteBuffers(ALsizei n, const ALuint* buffers) {
    for (ALsizei i = 0; i < n; i++) if (buffers) g_buffers.erase(buffers[i]);
}
void alBufferData(ALuint buffer, ALenum format, const ALvoid* data, ALsizei size, ALsizei freq) {
    (void)buffer;(void)format;(void)data;(void)size;(void)freq;
}
void alGetBufferi(ALuint buffer, ALenum param, ALint* value) { (void)buffer;(void)param; if(value)*value=0; }

void alListener3f(ALenum param, ALfloat x, ALfloat y, ALfloat z) { (void)param;(void)x;(void)y;(void)z; }
void alListenerfv(ALenum param, const ALfloat* values) { (void)param;(void)values; }

void alGenFilters(ALsizei n, ALuint* filters) {
    for (ALsizei i = 0; i < n; i++) {
        ALuint id = g_next_id++;
        g_filters[id] = true;
        if (filters) filters[i] = id;
    }
}
void alDeleteFilters(ALsizei n, const ALuint* filters) {
    for (ALsizei i = 0; i < n; i++) if (filters) g_filters.erase(filters[i]);
}
void alFilteri(ALuint filter, ALenum param, ALint value) { (void)filter;(void)param;(void)value; }
void alFilterf(ALuint filter, ALenum param, ALfloat value) { (void)filter;(void)param;(void)value; }
ALboolean alIsFilter(ALuint filter) { return g_filters.count(filter) ? 1 : 0; }

ALenum alGetError() { return 0; }

ALCdevice* alcOpenDevice(const char* devicename) { (void)devicename; return &g_device; }
ALCcontext* alcCreateContext(ALCdevice* device, const ALint* attrlist) { (void)device;(void)attrlist; return &g_context; }
ALCboolean alcMakeContextCurrent(ALCcontext* context) { (void)context; return 1; }
ALCboolean alcCloseDevice(ALCdevice* device) { (void)device; return 1; }
ALCboolean alcIsExtensionPresent(ALCdevice* device, const char* extname) { (void)device;(void)extname; return 0; }

void register_openal_symbols(bully::elf::Linker& linker) {
    linker.add_host_symbol("alBufferData", (uint64_t)&alBufferData);
    linker.add_host_symbol("alDeleteBuffers", (uint64_t)&alDeleteBuffers);
    linker.add_host_symbol("alDeleteFilters", (uint64_t)&alDeleteFilters);
    linker.add_host_symbol("alDeleteSources", (uint64_t)&alDeleteSources);
    linker.add_host_symbol("alFilterf", (uint64_t)&alFilterf);
    linker.add_host_symbol("alFilteri", (uint64_t)&alFilteri);
    linker.add_host_symbol("alGenBuffers", (uint64_t)&alGenBuffers);
    linker.add_host_symbol("alGenFilters", (uint64_t)&alGenFilters);
    linker.add_host_symbol("alGenSources", (uint64_t)&alGenSources);
    linker.add_host_symbol("alGetBufferi", (uint64_t)&alGetBufferi);
    linker.add_host_symbol("alGetError", (uint64_t)&alGetError);
    linker.add_host_symbol("alGetSource3f", (uint64_t)&alGetSource3f);
    linker.add_host_symbol("alGetSourcef", (uint64_t)&alGetSourcef);
    linker.add_host_symbol("alGetSourcei", (uint64_t)&alGetSourcei);
    linker.add_host_symbol("alIsFilter", (uint64_t)&alIsFilter);
    linker.add_host_symbol("alListener3f", (uint64_t)&alListener3f);
    linker.add_host_symbol("alListenerfv", (uint64_t)&alListenerfv);
    linker.add_host_symbol("alSource3f", (uint64_t)&alSource3f);
    linker.add_host_symbol("alSourcePause", (uint64_t)&alSourcePause);
    linker.add_host_symbol("alSourcePlay", (uint64_t)&alSourcePlay);
    linker.add_host_symbol("alSourceQueueBuffers", (uint64_t)&alSourceQueueBuffers);
    linker.add_host_symbol("alSourceRewind", (uint64_t)&alSourceRewind);
    linker.add_host_symbol("alSourceStop", (uint64_t)&alSourceStop);
    linker.add_host_symbol("alSourceUnqueueBuffers", (uint64_t)&alSourceUnqueueBuffers);
    linker.add_host_symbol("alSourcef", (uint64_t)&alSourcef);
    linker.add_host_symbol("alSourcei", (uint64_t)&alSourcei);
    linker.add_host_symbol("alcCloseDevice", (uint64_t)&alcCloseDevice);
    linker.add_host_symbol("alcCreateContext", (uint64_t)&alcCreateContext);
    linker.add_host_symbol("alcIsExtensionPresent", (uint64_t)&alcIsExtensionPresent);
    linker.add_host_symbol("alcMakeContextCurrent", (uint64_t)&alcMakeContextCurrent);
    linker.add_host_symbol("alcOpenDevice", (uint64_t)&alcOpenDevice);
}

} // namespace bully::openal
