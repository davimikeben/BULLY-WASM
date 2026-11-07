// graphics/gles3/gles3_shim.cpp
#include "gles3_shim.h"
#include "../../runtime/elf/linker.hpp"
#include <cstdio>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#endif

namespace bully::gles3 {

// Para Emscripten: encaminha para WebGL2 real. Para nativo: no-op (testes do linker).

void glActiveTexture(GLenum texture) {
#ifdef __EMSCRIPTEN__ ::glActiveTexture(texture); #else (void)texture; #endif
}
void glAttachShader(GLuint program, GLuint shader) {
#ifdef __EMSCRIPTEN__ ::glAttachShader(program, shader); #else (void)program;(void)shader; #endif
}
void glBindAttribLocation(GLuint program, GLuint index, const GLchar* name) {
#ifdef __EMSCRIPTEN__ ::glBindAttribLocation(program, index, name); #else (void)program;(void)index;(void)name; #endif
}
void glBindBuffer(GLenum target, GLuint buffer) {
#ifdef __EMSCRIPTEN__ ::glBindBuffer(target, buffer); #else (void)target;(void)buffer; #endif
}
void glBindFramebuffer(GLenum target, GLuint framebuffer) {
#ifdef __EMSCRIPTEN__ ::glBindFramebuffer(target, framebuffer); #else (void)target;(void)framebuffer; #endif
}
void glBindRenderbuffer(GLenum target, GLuint renderbuffer) {
#ifdef __EMSCRIPTEN__ ::glBindRenderbuffer(target, renderbuffer); #else (void)target;(void)renderbuffer; #endif
}
void glBindTexture(GLenum target, GLuint texture) {
#ifdef __EMSCRIPTEN__ ::glBindTexture(target, texture); #else (void)target;(void)texture; #endif
}
void glBindVertexArray(GLuint array) {
#ifdef __EMSCRIPTEN__ ::glBindVertexArray(array); #else (void)array; #endif
}
void glBlendFunc(GLenum sfactor, GLenum dfactor) {
#ifdef __EMSCRIPTEN__ ::glBlendFunc(sfactor, dfactor); #else (void)sfactor;(void)dfactor; #endif
}
void glBlendFuncSeparate(GLenum a,GLenum b, GLenum c, GLenum d) {
#ifdef __EMSCRIPTEN__ ::glBlendFuncSeparate(a,b,c,d); #else (void)a;(void)b;(void)c;(void)d; #endif
}
void glBufferData(GLenum t, GLsizeiptr s, const void* d, GLenum u) {
#ifdef __EMSCRIPTEN__ ::glBufferData(t,s,d,u); #else (void)t;(void)s;(void)d;(void)u; #endif
}
void glClear(GLbitfield mask) {
#ifdef __EMSCRIPTEN__ ::glClear(mask); #else (void)mask; #endif
}
void glClearColor(GLclampf r, GLclampf g, GLclampf b, GLclampf a) {
#ifdef __EMSCRIPTEN__ ::glClearColor(r,g,b,a); #else (void)r;(void)g;(void)b;(void)a; #endif
}
void glClearDepthf(GLclampf d) {
#ifdef __EMSCRIPTEN__ ::glClearDepthf(d); #else (void)d; #endif
}
void glClearStencil(GLint s) {
#ifdef __EMSCRIPTEN__ ::glClearStencil(s); #else (void)s; #endif
}
void glColorMask(GLboolean r, GLboolean g, GLboolean b, GLboolean a) {
#ifdef __EMSCRIPTEN__ ::glColorMask(r,g,b,a); #else (void)r;(void)g;(void)b;(void)a; #endif
}
void glCompileShader(GLuint shader) {
#ifdef __EMSCRIPTEN__ ::glCompileShader(shader); #else (void)shader; #endif
}
void glCompressedTexImage2D(GLenum a,GLint b,GLenum c,GLsizei d,GLsizei e,GLint f,GLsizei g,const void* h){
#ifdef __EMSCRIPTEN__ ::glCompressedTexImage2D(a,b,c,d,e,f,g,h); #else (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h; #endif
}
void glCompressedTexSubImage2D(GLenum a,GLint b,GLint c,GLint d,GLsizei e,GLsizei f,GLenum g,GLsizei h,const void* i){
#ifdef __EMSCRIPTEN__ ::glCompressedTexSubImage2D(a,b,c,d,e,f,g,h,i); #else (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i; #endif
}
GLuint glCreateProgram() {
#ifdef __EMSCRIPTEN__ return ::glCreateProgram(); #else return 1; #endif
}
GLuint glCreateShader(GLenum t) {
#ifdef __EMSCRIPTEN__ return ::glCreateShader(t); #else (void)t; return 1; #endif
}
void glCullFace(GLenum mode) {
#ifdef __EMSCRIPTEN__ ::glCullFace(mode); #else (void)mode; #endif
}
void glDeleteBuffers(GLsizei n, const GLuint* b) {
#ifdef __EMSCRIPTEN__ ::glDeleteBuffers(n,b); #else (void)n;(void)b; #endif
}
void glDeleteFramebuffers(GLsizei n, const GLuint* f) {
#ifdef __EMSCRIPTEN__ ::glDeleteFramebuffers(n,f); #else (void)n;(void)f; #endif
}
void glDeleteProgram(GLuint p) {
#ifdef __EMSCRIPTEN__ ::glDeleteProgram(p); #else (void)p; #endif
}
void glDeleteRenderbuffers(GLsizei n, const GLuint* r) {
#ifdef __EMSCRIPTEN__ ::glDeleteRenderbuffers(n,r); #else (void)n;(void)r; #endif
}
void glDeleteShader(GLuint s) {
#ifdef __EMSCRIPTEN__ ::glDeleteShader(s); #else (void)s; #endif
}
void glDeleteTextures(GLsizei n, const GLuint* t) {
#ifdef __EMSCRIPTEN__ ::glDeleteTextures(n,t); #else (void)n;(void)t; #endif
}
void glDeleteVertexArrays(GLsizei n, const GLuint* a) {
#ifdef __EMSCRIPTEN__ ::glDeleteVertexArrays(n,a); #else (void)n;(void)a; #endif
}
void glDepthFunc(GLenum f) {
#ifdef __EMSCRIPTEN__ ::glDepthFunc(f); #else (void)f; #endif
}
void glDepthMask(GLboolean f) {
#ifdef __EMSCRIPTEN__ ::glDepthMask(f); #else (void)f; #endif
}
void glDisable(GLenum cap) {
#ifdef __EMSCRIPTEN__ ::glDisable(cap); #else (void)cap; #endif
}
void glDisableVertexAttribArray(GLuint i) {
#ifdef __EMSCRIPTEN__ ::glDisableVertexAttribArray(i); #else (void)i; #endif
}
void glDrawBuffers(GLsizei n, const GLenum* b) {
#ifdef __EMSCRIPTEN__ ::glDrawBuffers(n,b); #else (void)n;(void)b; #endif
}
void glDrawElements(GLenum m, GLsizei c, GLenum t, const void* i) {
#ifdef __EMSCRIPTEN__ ::glDrawElements(m,c,t,i); #else (void)m;(void)c;(void)t;(void)i; #endif
}
void glEnable(GLenum cap) {
#ifdef __EMSCRIPTEN__ ::glEnable(cap); #else (void)cap; #endif
}
void glEnableVertexAttribArray(GLuint i) {
#ifdef __EMSCRIPTEN__ ::glEnableVertexAttribArray(i); #else (void)i; #endif
}
void glFinish() {
#ifdef __EMSCRIPTEN__ ::glFinish(); #endif
}
void glFramebufferRenderbuffer(GLenum a,GLenum b,GLenum c,GLuint r){
#ifdef __EMSCRIPTEN__ ::glFramebufferRenderbuffer(a,b,c,r); #else (void)a;(void)b;(void)c;(void)r; #endif
}
void glFramebufferTexture2D(GLenum a,GLenum b,GLenum c,GLuint d,GLint e){
#ifdef __EMSCRIPTEN__ ::glFramebufferTexture2D(a,b,c,d,e); #else (void)a;(void)b;(void)c;(void)d;(void)e; #endif
}
void glFrontFace(GLenum m) {
#ifdef __EMSCRIPTEN__ ::glFrontFace(m); #else (void)m; #endif
}
void glGenBuffers(GLsizei n, GLuint* b) {
#ifdef __EMSCRIPTEN__ ::glGenBuffers(n,b); #else if(b) for(GLsizei i=0;i<n;i++) b[i]=i+1; #endif
}
void glGenFramebuffers(GLsizei n, GLuint* f) {
#ifdef __EMSCRIPTEN__ ::glGenFramebuffers(n,f); #else if(f) for(GLsizei i=0;i<n;i++) f[i]=i+1; #endif
}
void glGenRenderbuffers(GLsizei n, GLuint* r) {
#ifdef __EMSCRIPTEN__ ::glGenRenderbuffers(n,r); #else if(r) for(GLsizei i=0;i<n;i++) r[i]=i+1; #endif
}
void glGenTextures(GLsizei n, GLuint* t) {
#ifdef __EMSCRIPTEN__ ::glGenTextures(n,t); #else if(t) for(GLsizei i=0;i<n;i++) t[i]=i+1; #endif
}
void glGenVertexArrays(GLsizei n, GLuint* a) {
#ifdef __EMSCRIPTEN__ ::glGenVertexArrays(n,a); #else if(a) for(GLsizei i=0;i<n;i++) a[i]=i+1; #endif
}
void glGetBooleanv(GLenum p, GLboolean* v) {
#ifdef __EMSCRIPTEN__ ::glGetBooleanv(p,v); #else (void)p; if(v) *v=0; #endif
}
GLenum glGetError() {
#ifdef __EMSCRIPTEN__ return ::glGetError(); #else return 0; #endif
}
void glGetIntegerv(GLenum p, GLint* v) {
#ifdef __EMSCRIPTEN__ ::glGetIntegerv(p,v); #else (void)p; if(v) *v=0; #endif
}
void glGetProgramInfoLog(GLuint p, GLsizei b, GLsizei* l, GLchar* i) {
#ifdef __EMSCRIPTEN__ ::glGetProgramInfoLog(p,b,l,i); #else (void)p;(void)b; if(l)*l=0; if(i&&b) i[0]=0; #endif
}
void glGetProgramiv(GLuint p, GLenum n, GLint* v) {
#ifdef __EMSCRIPTEN__ ::glGetProgramiv(p,n,v); #else (void)p;(void)n; if(v)*v=1; #endif
}
void glGetShaderInfoLog(GLuint s, GLsizei b, GLsizei* l, GLchar* i) {
#ifdef __EMSCRIPTEN__ ::glGetShaderInfoLog(s,b,l,i); #else (void)s;(void)b; if(l)*l=0; if(i&&b) i[0]=0; #endif
}
void glGetShaderiv(GLuint s, GLenum p, GLint* v) {
#ifdef __EMSCRIPTEN__ ::glGetShaderiv(s,p,v); #else (void)s;(void)p; if(v)*v=1; #endif
}
const GLubyte* glGetString(GLenum name) {
#ifdef __EMSCRIPTEN__ return ::glGetString(name); #else
    static const GLubyte vendor[]="WebGL"; static const GLubyte empty[]="";
    return (name==0x1F00||name==0x1F01||name==0x1F02) ? vendor : empty;
#endif
}
GLint glGetUniformLocation(GLuint p, const GLchar* n) {
#ifdef __EMSCRIPTEN__ return ::glGetUniformLocation(p,n); #else (void)p;(void)n; return 0; #endif
}
void glLineWidth(GLfloat w) {
#ifdef __EMSCRIPTEN__ ::glLineWidth(w); #else (void)w; #endif
}
void glLinkProgram(GLuint p) {
#ifdef __EMSCRIPTEN__ ::glLinkProgram(p); #else (void)p; #endif
}
void glPixelStorei(GLenum p, GLint v) {
#ifdef __EMSCRIPTEN__ ::glPixelStorei(p,v); #else (void)p;(void)v; #endif
}
void glPolygonOffset(GLfloat f, GLfloat u) {
#ifdef __EMSCRIPTEN__ ::glPolygonOffset(f,u); #else (void)f;(void)u; #endif
}
void glReadPixels(GLint x,GLint y,GLsizei w,GLsizei h,GLenum f,GLenum t,void* p){
#ifdef __EMSCRIPTEN__ ::glReadPixels(x,y,w,h,f,t,p); #else (void)x;(void)y;(void)w;(void)h;(void)f;(void)t;(void)p; #endif
}
void glRenderbufferStorage(GLenum t,GLenum i,GLsizei w,GLsizei h){
#ifdef __EMSCRIPTEN__ ::glRenderbufferStorage(t,i,w,h); #else (void)t;(void)i;(void)w;(void)h; #endif
}
void glScissor(GLint x,GLint y,GLsizei w,GLsizei h){
#ifdef __EMSCRIPTEN__ ::glScissor(x,y,w,h); #else (void)x;(void)y;(void)w;(void)h; #endif
}
void glShaderSource(GLuint s,GLsizei c,const GLchar* const* str,const GLint* l){
#ifdef __EMSCRIPTEN__ ::glShaderSource(s,c,str,l); #else (void)s;(void)c;(void)str;(void)l; #endif
}
void glStencilFunc(GLenum f,GLint r,GLuint m){
#ifdef __EMSCRIPTEN__ ::glStencilFunc(f,r,m); #else (void)f;(void)r;(void)m; #endif
}
void glStencilMask(GLuint m){
#ifdef __EMSCRIPTEN__ ::glStencilMask(m); #else (void)m; #endif
}
void glStencilOp(GLenum f,GLenum zf,GLenum zp){
#ifdef __EMSCRIPTEN__ ::glStencilOp(f,zf,zp); #else (void)f;(void)zf;(void)zp; #endif
}
void glTexImage2D(GLenum a,GLint b,GLint c,GLsizei d,GLsizei e,GLint f,GLenum g,GLenum h,const void* i){
#ifdef __EMSCRIPTEN__ ::glTexImage2D(a,b,c,d,e,f,g,h,i); #else (void)a;(void)b;(void)c;(void)d;(void)e;(void)f;(void)g;(void)h;(void)i; #endif
}
void glTexParameterf(GLenum t,GLenum p,GLfloat v){
#ifdef __EMSCRIPTEN__ ::glTexParameterf(t,p,v); #else (void)t;(void)p;(void)v; #endif
}
void glTexParameteri(GLenum t,GLenum p,GLint v){
#ifdef __EMSCRIPTEN__ ::glTexParameteri(t,p,v); #else (void)t;(void)p;(void)v; #endif
}
void glTexStorage2D(GLenum t,GLsizei l,GLenum i,GLsizei w,GLsizei h){
#ifdef __EMSCRIPTEN__ ::glTexStorage2D(t,l,i,w,h); #else (void)t;(void)l;(void)i;(void)w;(void)h; #endif
}
void glTexSubImage2D(GLenum t,GLint l,GLint x,GLint y,GLsizei w,GLsizei h,GLenum f,GLenum ty,const void* p){
#ifdef __EMSCRIPTEN__ ::glTexSubImage2D(t,l,x,y,w,h,f,ty,p); #else (void)t;(void)l;(void)x;(void)y;(void)w;(void)h;(void)f;(void)ty;(void)p; #endif
}
void glUniform1i(GLint loc,GLint v){
#ifdef __EMSCRIPTEN__ ::glUniform1i(loc,v); #else (void)loc;(void)v; #endif
}
void glUniform4fv(GLint loc,GLsizei c,const GLfloat* v){
#ifdef __EMSCRIPTEN__ ::glUniform4fv(loc,c,v); #else (void)loc;(void)c;(void)v; #endif
}
void glUniformMatrix4fv(GLint loc,GLsizei c,GLboolean t,const GLfloat* v){
#ifdef __EMSCRIPTEN__ ::glUniformMatrix4fv(loc,c,t,v); #else (void)loc;(void)c;(void)t;(void)v; #endif
}
void glUseProgram(GLuint p){
#ifdef __EMSCRIPTEN__ ::glUseProgram(p); #else (void)p; #endif
}
void glVertexAttribPointer(GLuint i,GLint s,GLenum t,GLboolean n,GLsizei st,const void* p){
#ifdef __EMSCRIPTEN__ ::glVertexAttribPointer(i,s,t,n,st,p); #else (void)i;(void)s;(void)t;(void)n;(void)st;(void)p; #endif
}
void glViewport(GLint x,GLint y,GLsizei w,GLsizei h){
#ifdef __EMSCRIPTEN__ ::glViewport(x,y,w,h); #else (void)x;(void)y;(void)w;(void)h; #endif
}

// HostFn stub: registra todas as funcoes GL. Para builds nativas (teste do
// linker) sao no-ops. No Emscripten, as funcoes reais sao chamadas.
static arm64::HostFn gl_stub() {
    return [](arm64::CPUState&) -> uint64_t { return 0; };
}

void register_gl_symbols(bully::elf::Linker& linker) {
#define GL_STUB(name) linker.add_host_symbol(#name, gl_stub())
    GL_STUB(glActiveTexture); GL_STUB(glAttachShader); GL_STUB(glBindAttribLocation);
    GL_STUB(glBindBuffer); GL_STUB(glBindFramebuffer); GL_STUB(glBindRenderbuffer);
    GL_STUB(glBindTexture); GL_STUB(glBindVertexArray); GL_STUB(glBlendFunc);
    GL_STUB(glBlendFuncSeparate); GL_STUB(glBufferData); GL_STUB(glClear);
    GL_STUB(glClearColor); GL_STUB(glClearDepthf); GL_STUB(glClearStencil);
    GL_STUB(glColorMask); GL_STUB(glCompileShader); GL_STUB(glCompressedTexImage2D);
    GL_STUB(glCompressedTexSubImage2D); GL_STUB(glCreateProgram); GL_STUB(glCreateShader);
    GL_STUB(glCullFace); GL_STUB(glDeleteBuffers); GL_STUB(glDeleteFramebuffers);
    GL_STUB(glDeleteProgram); GL_STUB(glDeleteRenderbuffers); GL_STUB(glDeleteShader);
    GL_STUB(glDeleteTextures); GL_STUB(glDeleteVertexArrays); GL_STUB(glDepthFunc);
    GL_STUB(glDepthMask); GL_STUB(glDisable); GL_STUB(glDisableVertexAttribArray);
    GL_STUB(glDrawBuffers); GL_STUB(glDrawElements); GL_STUB(glEnable);
    GL_STUB(glEnableVertexAttribArray); GL_STUB(glFinish); GL_STUB(glFramebufferRenderbuffer);
    GL_STUB(glFramebufferTexture2D); GL_STUB(glFrontFace); GL_STUB(glGenBuffers);
    GL_STUB(glGenFramebuffers); GL_STUB(glGenRenderbuffers); GL_STUB(glGenTextures);
    GL_STUB(glGenVertexArrays); GL_STUB(glGetBooleanv); GL_STUB(glGetError);
    GL_STUB(glGetIntegerv); GL_STUB(glGetProgramInfoLog); GL_STUB(glGetProgramiv);
    GL_STUB(glGetShaderInfoLog); GL_STUB(glGetShaderiv); GL_STUB(glGetString);
    GL_STUB(glGetUniformLocation); GL_STUB(glLineWidth); GL_STUB(glLinkProgram);
    GL_STUB(glPixelStorei); GL_STUB(glPolygonOffset); GL_STUB(glReadPixels);
    GL_STUB(glRenderbufferStorage); GL_STUB(glScissor); GL_STUB(glShaderSource);
    GL_STUB(glStencilFunc); GL_STUB(glStencilMask); GL_STUB(glStencilOp);
    GL_STUB(glTexImage2D); GL_STUB(glTexParameterf); GL_STUB(glTexParameteri);
    GL_STUB(glTexStorage2D); GL_STUB(glTexSubImage2D); GL_STUB(glUniform1i);
    GL_STUB(glUniform4fv); GL_STUB(glUniformMatrix4fv); GL_STUB(glUseProgram);
    GL_STUB(glVertexAttribPointer); GL_STUB(glViewport);
#undef GL_STUB
}

} // namespace bully::gles3
