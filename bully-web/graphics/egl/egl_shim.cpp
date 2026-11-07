// graphics/egl/egl_shim.cpp
#include "egl_shim.h"
#include "../../runtime/elf/linker.hpp"
#include <cstdio>

#ifdef __EMSCRIPTEN__
#include <EGL/egl.h>
#endif

namespace bully::egl {

// Em Emscripten: usa o EGL real que aponta para o canvas WebGL2.
// Em nativo: stub que simula um contexto valido (para testes do linker).

static EGLDisplay g_display = (EGLDisplay)1;
static EGLContext g_context = (EGLContext)1;
static EGLSurface g_surface = (EGLSurface)1;
static EGLConfig g_config = (EGLConfig)1;

EGLDisplay eglGetDisplay(EGLNativeWindowType display) {
#ifdef __EMSCRIPTEN__ return ::eglGetDisplay(display); #else (void)display; return g_display; #endif
}
EGLBoolean eglInitialize(EGLDisplay display, EGLint* major, EGLint* minor) {
#ifdef __EMSCRIPTEN__ return ::eglInitialize(display, major, minor); #else
    (void)display; if(major)*major=3; if(minor)*minor=0; return 1;
#endif
}
EGLBoolean eglChooseConfig(EGLDisplay display, const EGLint* attribs, EGLConfig* configs, EGLint config_size, EGLint* num_configs) {
#ifdef __EMSCRIPTEN__ return ::eglChooseConfig(display, attribs, configs, config_size, num_configs); #else
    (void)display;(void)attribs;
    if(configs && config_size>0) configs[0]=g_config;
    if(num_configs) *num_configs=1;
    return 1;
#endif
}
EGLBoolean eglGetConfigAttrib(EGLDisplay display, EGLConfig config, EGLint attribute, EGLint* value) {
#ifdef __EMSCRIPTEN__ return ::eglGetConfigAttrib(display, config, attribute, value); #else
    (void)display;(void)config;(void)attribute; if(value)*value=0; return 1;
#endif
}
EGLContext eglCreateContext(EGLDisplay display, EGLConfig config, EGLContext share_context, const EGLint* attribs) {
#ifdef __EMSCRIPTEN__ return ::eglCreateContext(display, config, share_context, attribs); #else
    (void)display;(void)config;(void)share_context;(void)attribs; return g_context;
#endif
}
EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig config, EGLNativeWindowType win, const EGLint* attribs) {
#ifdef __EMSCRIPTEN__ return ::eglCreateWindowSurface(display, config, win, attribs); #else
    (void)display;(void)config;(void)win;(void)attribs; return g_surface;
#endif
}
EGLBoolean eglMakeCurrent(EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext context) {
#ifdef __EMSCRIPTEN__ return ::eglMakeCurrent(display, draw, read, context); #else
    (void)display;(void)draw;(void)read;(void)context; return 1;
#endif
}
EGLBoolean eglSwapBuffers(EGLDisplay display, EGLSurface surface) {
#ifdef __EMSCRIPTEN__ return ::eglSwapBuffers(display, surface); #else
    (void)display;(void)surface; return 1;
#endif
}
EGLBoolean eglSwapInterval(EGLDisplay display, EGLint interval) {
#ifdef __EMSCRIPTEN__ return ::eglSwapInterval(display, interval); #else
    (void)display;(void)interval; return 1;
#endif
}
EGLBoolean eglDestroySurface(EGLDisplay display, EGLSurface surface) {
#ifdef __EMSCRIPTEN__ return ::eglDestroySurface(display, surface); #else
    (void)display;(void)surface; return 1;
#endif
}
EGLint eglGetError() {
#ifdef __EMSCRIPTEN__ return ::eglGetError(); #else return 0; #endif
}
void* eglGetProcAddress(const char* name) {
#ifdef __EMSCRIPTEN__ return ::eglGetProcAddress(name); #else (void)name; return nullptr; #endif
}

void register_egl_symbols(bully::elf::Linker& linker) {
    linker.add_host_symbol("eglChooseConfig", (uint64_t)&eglChooseConfig);
    linker.add_host_symbol("eglCreateContext", (uint64_t)&eglCreateContext);
    linker.add_host_symbol("eglCreateWindowSurface", (uint64_t)&eglCreateWindowSurface);
    linker.add_host_symbol("eglDestroySurface", (uint64_t)&eglDestroySurface);
    linker.add_host_symbol("eglGetConfigAttrib", (uint64_t)&eglGetConfigAttrib);
    linker.add_host_symbol("eglGetDisplay", (uint64_t)&eglGetDisplay);
    linker.add_host_symbol("eglGetError", (uint64_t)&eglGetError);
    linker.add_host_symbol("eglGetProcAddress", (uint64_t)&eglGetProcAddress);
    linker.add_host_symbol("eglInitialize", (uint64_t)&eglInitialize);
    linker.add_host_symbol("eglMakeCurrent", (uint64_t)&eglMakeCurrent);
    linker.add_host_symbol("eglSwapBuffers", (uint64_t)&eglSwapBuffers);
    linker.add_host_symbol("eglSwapInterval", (uint64_t)&eglSwapInterval);
}

} // namespace bully::egl
