// graphics/egl/egl_shim.h
// EGL shim — gerencia o contexto WebGL2 do navegador.
#pragma once
#include <cstdint>

typedef void* EGLDisplay;
typedef void* EGLConfig;
typedef void* EGLContext;
typedef void* EGLSurface;
typedef void* EGLNativeWindowType;
typedef int32_t EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;

namespace bully::egl {

EGLDisplay eglGetDisplay(EGLNativeWindowType display);
EGLBoolean eglInitialize(EGLDisplay display, EGLint* major, EGLint* minor);
EGLBoolean eglChooseConfig(EGLDisplay display, const EGLint* attribs, EGLConfig* configs, EGLint config_size, EGLint* num_configs);
EGLBoolean eglGetConfigAttrib(EGLDisplay display, EGLConfig config, EGLint attribute, EGLint* value);
EGLContext eglCreateContext(EGLDisplay display, EGLConfig config, EGLContext share_context, const EGLint* attribs);
EGLSurface eglCreateWindowSurface(EGLDisplay display, EGLConfig config, EGLNativeWindowType win, const EGLint* attribs);
EGLBoolean eglMakeCurrent(EGLDisplay display, EGLSurface draw, EGLSurface read, EGLContext context);
EGLBoolean eglSwapBuffers(EGLDisplay display, EGLSurface surface);
EGLBoolean eglSwapInterval(EGLDisplay display, EGLint interval);
EGLBoolean eglDestroySurface(EGLDisplay display, EGLSurface surface);
EGLint eglGetError();
void* eglGetProcAddress(const char* name);

void register_egl_symbols(class bully::elf::Linker& linker);

} // namespace bully::egl
