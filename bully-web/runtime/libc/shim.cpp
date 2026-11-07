// runtime/libc/shim.cpp
#include "shim.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <ctime>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <errno.h>
#include <pthread.h>

namespace bully::libc {

using arm64::CPUState;

// ===========================================================================
// libc - memoria
// ===========================================================================
static uint64_t h_malloc(CPUState& c)  { return (uint64_t)malloc((size_t)c.x[0]); }
static uint64_t h_calloc(CPUState& c)  { return (uint64_t)calloc((size_t)c.x[0], (size_t)c.x[1]); }
static uint64_t h_realloc(CPUState& c) { return (uint64_t)realloc((void*)c.x[0], (size_t)c.x[1]); }
static uint64_t h_free(CPUState& c)    { free((void*)c.x[0]); return 0; }

// ===========================================================================
// libc - string/memory
// ===========================================================================
static uint64_t h_memcpy(CPUState& c) {
    return (uint64_t)memcpy((void*)c.x[0], (const void*)c.x[1], (size_t)c.x[2]);
}
static uint64_t h_memset(CPUState& c) {
    return (uint64_t)memset((void*)c.x[0], (int)c.x[1], (size_t)c.x[2]);
}
static uint64_t h_memcmp(CPUState& c) {
    return (uint64_t)memcmp((const void*)c.x[0], (const void*)c.x[1], (size_t)c.x[2]);
}
static uint64_t h_memmove(CPUState& c) {
    return (uint64_t)memmove((void*)c.x[0], (const void*)c.x[1], (size_t)c.x[2]);
}
static uint64_t h_memchr(CPUState& c) {
    return (uint64_t)memchr((const void*)c.x[0], (int)c.x[1], (size_t)c.x[2]);
}
static uint64_t h_strlen(CPUState& c) { return (uint64_t)strlen((const char*)c.x[0]); }
static uint64_t h_strnlen(CPUState& c) { return (uint64_t)strnlen((const char*)c.x[0], (size_t)c.x[1]); }
static uint64_t h_strcpy(CPUState& c) { return (uint64_t)strcpy((char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strncpy(CPUState& c) { return (uint64_t)strncpy((char*)c.x[0], (const char*)c.x[1], (size_t)c.x[2]); }
static uint64_t h_strcmp(CPUState& c) { return (uint64_t)strcmp((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strncmp(CPUState& c) { return (uint64_t)strncmp((const char*)c.x[0], (const char*)c.x[1], (size_t)c.x[2]); }
static uint64_t h_strchr(CPUState& c) { return (uint64_t)strchr((const char*)c.x[0], (int)c.x[1]); }
static uint64_t h_strrchr(CPUState& c) { return (uint64_t)strrchr((const char*)c.x[0], (int)c.x[1]); }
static uint64_t h_strstr(CPUState& c) { return (uint64_t)strstr((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strdup(CPUState& c) { return (uint64_t)strdup((const char*)c.x[0]); }
static uint64_t h_strndup(CPUState& c) { return (uint64_t)strndup((const char*)c.x[0], (size_t)c.x[1]); }
static uint64_t h_strcat(CPUState& c) { return (uint64_t)strcat((char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strncat(CPUState& c) { return (uint64_t)strncat((char*)c.x[0], (const char*)c.x[1], (size_t)c.x[2]); }
static uint64_t h_strcasecmp(CPUState& c) { return (uint64_t)strcasecmp((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strncasecmp(CPUState& c) { return (uint64_t)strncasecmp((const char*)c.x[0], (const char*)c.x[1], (size_t)c.x[2]); }
static uint64_t h_strtok(CPUState& c) { return (uint64_t)strtok((char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strtok_r(CPUState& c) { return (uint64_t)strtok_r((char*)c.x[0], (const char*)c.x[1], (char**)c.x[2]); }
static uint64_t h_strerror(CPUState& c) { return (uint64_t)strerror((int)c.x[0]); }
static uint64_t h_strcspn(CPUState& c) { return (uint64_t)strcspn((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strspn(CPUState& c) { return (uint64_t)strspn((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_strpbrk(CPUState& c) { return (uint64_t)strpbrk((const char*)c.x[0], (const char*)c.x[1]); }

// ===========================================================================
// libc - stdio
// ===========================================================================
static uint64_t h_sprintf(CPUState& c) {
    return (uint64_t)sprintf((char*)c.x[0], (const char*)c.x[1],
        (void*)c.x[2], (void*)c.x[3], (void*)c.x[4], (void*)c.x[5],
        (void*)c.x[6], (void*)c.x[7]));
}
static uint64_t h_snprintf(CPUState& c) {
    return (uint64_t)snprintf((char*)c.x[0], (size_t)c.x[1], (const char*)c.x[2],
        (void*)c.x[3], (void*)c.x[4], (void*)c.x[5], (void*)c.x[6],
        (void*)c.x[7]);
}
static uint64_t h_printf(CPUState& c) {
    return (uint64_t)printf((const char*)c.x[0],
        (void*)c.x[1], (void*)c.x[2], (void*)c.x[3], (void*)c.x[4],
        (void*)c.x[5], (void*)c.x[6], (void*)c.x[7]);
}
static uint64_t h_fprintf(CPUState& c) {
    return (uint64_t)fprintf((FILE*)c.x[0], (const char*)c.x[1],
        (void*)c.x[2], (void*)c.x[3], (void*)c.x[4], (void*)c.x[5],
        (void*)c.x[6], (void*)c.x[7]);
}
static uint64_t h_fopen(CPUState& c) { return (uint64_t)fopen((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_fclose(CPUState& c) { return (uint64_t)fclose((FILE*)c.x[0]); }
static uint64_t h_fread(CPUState& c) { return (uint64_t)fread((void*)c.x[0], (size_t)c.x[1], (size_t)c.x[2], (FILE*)c.x[3]); }
static uint64_t h_fwrite(CPUState& c) { return (uint64_t)fwrite((const void*)c.x[0], (size_t)c.x[1], (size_t)c.x[2], (FILE*)c.x[3]); }
static uint64_t h_fseek(CPUState& c) { return (uint64_t)fseek((FILE*)c.x[0], (long)c.x[1], (int)c.x[2]); }
static uint64_t h_ftell(CPUState& c) { return (uint64_t)ftell((FILE*)c.x[0]); }
static uint64_t h_fflush(CPUState& c) { return (uint64_t)fflush((FILE*)c.x[0]); }
static uint64_t h_fgets(CPUState& c) { return (uint64_t)fgets((char*)c.x[0], (int)c.x[1], (FILE*)c.x[2]); }
static uint64_t h_fputs(CPUState& c) { return (uint64_t)fputs((const char*)c.x[0], (FILE*)c.x[1]); }
static uint64_t h_fputc(CPUState& c) { return (uint64_t)fputc((int)c.x[0], (FILE*)c.x[1]); }
static uint64_t h_rewind(CPUState& c) { rewind((FILE*)c.x[0]); return 0; }
static uint64_t h_feof(CPUState& c) { return (uint64_t)feof((FILE*)c.x[0]); }
static uint64_t h_ferror(CPUState& c) { return (uint64_t)ferror((FILE*)c.x[0]); }
static uint64_t h_setvbuf(CPUState& c) { return (uint64_t)setvbuf((FILE*)c.x[0], (char*)c.x[1], (int)c.x[2], (size_t)c.x[3]); }
static uint64_t h_fgetpos(CPUState& c) { return (uint64_t)fgetpos((FILE*)c.x[0], (fpos_t*)c.x[1]); }
static uint64_t h_fsetpos(CPUState& c) { return (uint64_t)fsetpos((FILE*)c.x[0], (const fpos_t*)c.x[1]); }
static uint64_t h_clearerr(CPUState& c) { clearerr((FILE*)c.x[0]); return 0; }
static uint64_t h_ungetc(CPUState& c) { return (uint64_t)ungetc((int)c.x[0], (FILE*)c.x[1]); }
static uint64_t h_vfprintf(CPUState& c) {
    va_list ap;
    va_start(ap, c.x[2]);
    int r = vfprintf((FILE*)c.x[0], (const char*)c.x[1], ap);
    va_end(ap);
    return (uint64_t)r;
}
static uint64_t h_vsnprintf(CPUState& c) {
    va_list ap;
    va_start(ap, c.x[2]);
    int r = vsnprintf((char*)c.x[0], (size_t)c.x[1], (const char*)c.x[2], ap);
    va_end(ap);
    return (uint64_t)r;
}

// ===========================================================================
// libc - filesystem
// ===========================================================================
static uint64_t h_unlink(CPUState& c) { return (uint64_t)unlink((const char*)c.x[0]); }
static uint64_t h_remove(CPUState& c) { return (uint64_t)remove((const char*)c.x[0]); }
static uint64_t h_rename(CPUState& c) { return (uint64_t)rename((const char*)c.x[0], (const char*)c.x[1]); }
static uint64_t h_access(CPUState& c) { return (uint64_t)access((const char*)c.x[0], (int)c.x[1]); }
static uint64_t h_stat(CPUState& c) { return (uint64_t)stat((const char*)c.x[0], (struct stat*)c.x[1]); }
static uint64_t h_open(CPUState& c) { return (uint64_t)open((const char*)c.x[0], (int)c.x[1], (int)c.x[2]); }
static uint64_t h_read(CPUState& c) { return (uint64_t)read((int)c.x[0], (void*)c.x[1], (size_t)c.x[2]); }
static uint64_t h_write(CPUState& c) { return (uint64_t)write((int)c.x[0], (const void*)c.x[1], (size_t)c.x[2]); }
static uint64_t h_close(CPUState& c) { return (uint64_t)close((int)c.x[0]); }
static uint64_t h_lseek(CPUState& c) { return (uint64_t)lseek((int)c.x[0], (off_t)c.x[1], (int)c.x[2]); }
static uint64_t h_mkdir(CPUState& c) { return (uint64_t)mkdir((const char*)c.x[0], (mode_t)c.x[1]); }
static uint64_t h_rmdir(CPUState& c) { return (uint64_t)rmdir((const char*)c.x[0]); }

// ===========================================================================
// libc - mmap
// ===========================================================================
static uint64_t h_mmap(CPUState& c) {
    void* addr = (void*)c.x[0];
    size_t len = (size_t)c.x[1];
    int prot = (int)c.x[2];
    int flags = (int)c.x[3];
    int fd = (int)c.x[4];
    off_t off = (off_t)c.x[5];
    return (uint64_t)mmap(addr, len, prot, flags, fd, off);
}
static uint64_t h_munmap(CPUState& c) { return (uint64_t)munmap((void*)c.x[0], (size_t)c.x[1]); }
static uint64_t h_mprotect(CPUState& c) { return (uint64_t)mprotect((void*)c.x[0], (size_t)c.x[1], (int)c.x[2]); }

// ===========================================================================
// libc - time/rand
// ===========================================================================
static uint64_t h_rand(CPUState& c) { return (uint64_t)rand(); }
static uint64_t h_srand(CPUState& c) { srand((unsigned int)c.x[0]); return 0; }
static uint64_t h_time(CPUState& c) { return (uint64_t)time((time_t*)c.x[0]); }
static uint64_t h_clock(CPUState& c) { return (uint64_t)clock(); }
static uint64_t h_gettimeofday(CPUState& c) {
    return (uint64_t)gettimeofday((struct timeval*)c.x[0], (void*)c.x[1]); }
static uint64_t h_usleep(CPUState& c) { return (uint64_t)usleep((useconds_t)c.x[0]); }
static uint64_t h_sleep(CPUState& c) { return (uint64_t)sleep((unsigned int)c.x[0]); }

// ===========================================================================
// libc - misc
// ===========================================================================
static uint64_t h_abort(CPUState& c) { abort(); return 0; }
static uint64_t h_exit(CPUState& c) { exit((int)c.x[0]); return 0; }
static uint64_t h__Exit(CPUState& c) { _exit((int)c.x[0]); return 0; }
static uint64_t h_atexit(CPUState& c) { return (uint64_t)atexit((void(*)())c.x[0]); }
static uint64_t h_getenv(CPUState& c) { return (uint64_t)getenv((const char*)c.x[0]); }
static uint64_t h_setenv(CPUState& c) { return (uint64_t)setenv((const char*)c.x[0], (const char*)c.x[1], (int)c.x[2]); }
static uint64_t h_unsetenv(CPUState& c) { return (uint64_t)unsetenv((const char*)c.x[0]); }
static uint64_t h_system(CPUState& c) { return (uint64_t)system((const char*)c.x[0]); }
static uint64_t h_abs(CPUState& c) { return (uint64_t)abs((int)c.x[0]); }
static uint64_t h_labs(CPUState& c) { return (uint64_t)labs((long)c.x[0]); }
static uint64_t h_atoi(CPUState& c) { return (uint64_t)atoi((const char*)c.x[0]); }
static uint64_t h_atol(CPUState& c) { return (uint64_t)atol((const char*)c.x[0]); }
static uint64_t h_atof(CPUState& c) {
    union { double d; uint64_t u; } u;
    u.d = atof((const char*)c.x[0]);
    return u.u;
}
static uint64_t h_strtol(CPUState& c) { return (uint64_t)strtol((const char*)c.x[0], (char**)c.x[1], (int)c.x[2]); }
static uint64_t h_strtoll(CPUState& c) { return (uint64_t)strtoll((const char*)c.x[0], (char**)c.x[1], (int)c.x[2]); }
static uint64_t h_strtoul(CPUState& c) { return (uint64_t)strtoul((const char*)c.x[0], (char**)c.x[1], (int)c.x[2]); }
static uint64_t h_strtoull(CPUState& c) { return (uint64_t)strtoull((const char*)c.x[0], (char**)c.x[1], (int)c.x[2]); }
static uint64_t h_strtod(CPUState& c) {
    union { double d; uint64_t u; } u;
    u.d = strtod((const char*)c.x[0], (char**)c.x[1]);
    return u.u;
}
static uint64_t h_qsort(CPUState& c) {
    qsort((void*)c.x[0], (size_t)c.x[1], (size_t)c.x[2], (int(*)(const void*,const void*))c.x[3]);
    return 0;
}
static uint64_t h_bsearch(CPUState& c) {
    return (uint64_t)bsearch((const void*)c.x[0], (const void*)c.x[1], (size_t)c.x[2],
        (size_t)c.x[3], (int(*)(const void*,const void*))c.x[4]);
}
static uint64_t h_memmem(CPUState& c) {
    return (uint64_t)memmem((const void*)c.x[0], (size_t)c.x[1], (const void*)c.x[2], (size_t)c.x[3]);
}

// errno
static int g_errno = 0;
static uint64_t h___errno(CPUState& c) { return (uint64_t)&g_errno; }

// __sF: array of FILE* stdin/stdout/stderr
static FILE* g_sF[3] = { stdin, stdout, stderr };
static uint64_t h___sF(CPUState& c) { return (uint64_t)g_sF; }

// ===========================================================================
// libm
// ===========================================================================
#define LIBM_FN(name, fn) \
    static uint64_t h_##name(CPUState& c) { \
        union { double d; uint64_t u; } u; u.d = fn((double)c.x[0]); return u.u; }
#define LIBM_FN2(name, fn) \
    static uint64_t h_##name(CPUState& c) { \
        union { double d; uint64_t u; } u; \
        u.d = fn((double)c.x[0], (double)c.x[1]); return u.u; }
#define LIBM_FNF(name, fn) \
    static uint64_t h_##name(CPUState& c) { \
        union { float f; uint32_t u; } u; u.f = fn((float)c.x[0]); return u.u; }
#define LIBM_FNF2(name, fn) \
    static uint64_t h_##name(CPUState& c) { \
        union { float f; uint32_t u; } u; \
        u.f = fn((float)c.x[0], (float)c.x[1]); return u.u; }

LIBM_FN(sin, sin)
LIBM_FN(cos, cos)
LIBM_FN(tan, tan)
LIBM_FN(asin, asin)
LIBM_FN(acos, acos)
LIBM_FN(atan, atan)
LIBM_FN2(atan2, atan2)
LIBM_FN(sinh, sinh)
LIBM_FN(cosh, cosh)
LIBM_FN(tanh, tanh)
LIBM_FN(exp, exp)
LIBM_FN(exp2, exp2)
LIBM_FN(log, log)
LIBM_FN(log2, log2)
LIBM_FN(log10, log10)
LIBM_FN2(pow, pow)
LIBM_FN(sqrt, sqrt)
LIBM_FN(cbrt, cbrt)
LIBM_FN(fabs, fabs)
LIBM_FN(floor, floor)
LIBM_FN(ceil, ceil)
LIBM_FN(round, round)
LIBM_FN2(fmod, fmod)
LIBM_FN2(ldexp, ldexp)
LIBM_FN2(frexp, frexp) // not quite - need ptr arg
LIBM_FN2(modf, modf)     // not quite
LIBM_FN(trunc, trunc)
LIBM_FN(nearbyint, nearbyint)
LIBM_FN(roundeven, roundeven)
LIBM_FN2(scalbn, scalbn)
LIBM_FN2(copysign, copysign)
LIBM_FN2(nextafter, nextafter)
LIBM_FN2(fdim, fdim)
LIBM_FN2(fmax, fmax)
LIBM_FN2(fmin, fmin)
LIBM_FN2(hypot, hypot)
LIBM_FN2(remainder, remainder)
LIBM_FN(tgamma, tgamma)
LIBM_FN(lgamma, lgamma)
LIBM_FN(erf, erf)
LIBM_FN(erfc, erfc)

LIBM_FNF(sinf, sinf)
LIBM_FNF(cosf, cosf)
LIBM_FNF(tanf, tanf)
LIBM_FNF(asinf, asinf)
LIBM_FNF(acosf, acosf)
LIBM_FNF(atanf, atanf)
LIBM_FNF2(atan2f, atan2f)
LIBM_FNF(sinhf, sinhf)
LIBM_FNF(coshf, coshf)
LIBM_FNF(tanhf, tanhf)
LIBM_FNF(expf, expf)
LIBM_FNF(logf, logf)
LIBM_FNF(log10f, log10f)
LIBM_FNF2(powf, powf)
LIBM_FNF(sqrtf, sqrtf)
LIBM_FNF(cbrtf, cbrtf)
LIBM_FNF(fabsf, fabsf)
LIBM_FNF(floorf, floorf)
LIBM_FNF(ceilf, ceilf)
LIBM_FNF(roundf, roundf)
LIBM_FNF2(fmodf, fmodf)
LIBM_FNF(truncf, truncf)
LIBM_FNF2(fminf, fminf)
LIBM_FNF2(fmaxf, fmaxf)
LIBM_FNF2(hypotf, hypotf)

// ===========================================================================
// libdl
// ===========================================================================
static uint64_t h_dl_iterate_phdr(CPUState& c) {
    // dl_iterate_phdr(callback, data) - retorna 0 (no phdrs do convidado
    return 0;
}
static uint64_t h_dlopen(CPUState& c) { return 0; }
static uint64_t h_dlsym(CPUState& c) { return 0; }
static uint64_t h_dlclose(CPUState& c) { return 0; }
static uint64_t h_dlerror(CPUState& c) { return 0; }

// ===========================================================================
// pthread (stubs - single-threaded for now
// ===========================================================================
static uint64_t h_pthread_create(CPUState& c) { return 0; }
static uint64_t h_pthread_join(CPUState& c) { return 0; }
static uint64_t h_pthread_detach(CPUState& c) { return 0; }
static uint64_t h_pthread_self(CPUState& c) { return 1; }
static uint64_t h_pthread_exit(CPUState& c) { return 0; }
static uint64_t h_pthread_kill(CPUState& c) { return 0; }
static uint64_t h_pthread_once(CPUState& c) {
    int* once = (int*)c.x[0];
    if (!*once) { *once = 1; ((void(*)())c.x[1](); }
    return 0;
}
static uint64_t h_pthread_mutex_init(CPUState& c) { return 0; }
static uint64_t h_pthread_mutex_destroy(CPUState& c) { return 0; }
static uint64_t h_pthread_mutex_lock(CPUState& c) { return 0; }
static uint64_t h_pthread_mutex_unlock(CPUState& c) { return 0; }
static uint64_t h_pthread_mutex_trylock(CPUState& c) { return 0; }
static uint64_t h_pthread_rwlock_init(CPUState& c) { return 0; }
static uint64_t h_pthread_rwlock_destroy(CPUState& c) { return 0; }
static uint64_t h_pthread_rwlock_rdlock(CPUState& c) { return 0; }
static uint64_t h_pthread_rwlock_wrlock(CPUState& c) { return 0; }
static uint64_t h_pthread_rwlock_unlock(CPUState& c) { return 0; }
static uint64_t h_pthread_cond_init(CPUState& c) { return 0; }
static uint64_t h_pthread_cond_destroy(CPUState& c) { return 0; }
static uint64_t h_pthread_cond_wait(CPUState& c) { return 0; }
static uint64_t h_pthread_cond_signal(CPUState& c) { return 0; }
static uint64_t h_pthread_cond_broadcast(CPUState& c) { return 0; }
static uint64_t h_pthread_cond_timedwait(CPUState& c) { return 0; }
static uint64_t h_pthread_key_create(CPUState& c) {
    static int next_key = 1;
    *(int*)c.x[0] = next_key++;
    return 0;
}
static uint64_t h_pthread_key_delete(CPUState& c) { return 0; }
static uint64_t h_pthread_getspecific(CPUState& c) { return 0; }
static uint64_t h_pthread_setspecific(CPUState& c) { return 0; }
static uint64_t h_pthread_attr_init(CPUState& c) { return 0; }
static uint64_t h_pthread_attr_destroy(CPUState& c) { return 0; }
static uint64_t h_pthread_attr_setdetachstate(CPUState& c) { return 0; }
static uint64_t h_pthread_sigmask(CPUState& c) { return 0; }
static uint64_t h_pthread_setname_np(CPUState& c) { return 0; }
static uint64_t h_pthread_getname_np(CPUState& c) { return 0; }

// ===========================================================================
// C++ ABI
// ===========================================================================
static uint64_t h___cxa_finalize(CPUState& c) { return 0; }
static uint64_t h___cxa_atexit(CPUState& c) { return 0; }
static uint64_t h___register_atfork(CPUState& c) { return 0; }
static uint64_t h___stack_chk_fail(CPUState& c) {
    fprintf(stderr, "[cxa] __stack_chk_fail called!\n");
    abort();
    return 0;
}
static uint64_t h___cxa_pure_virtual(CPUState& c) {
    fprintf(stderr, "[cxa] pure virtual function called!\n");
    abort();
    return 0;
}
static uint64_t h___cxa_allocate_exception(CPUState& c) { return (uint64_t)malloc(256); }
static uint64_t h___cxa_free_exception(CPUState& c) { free((void*)c.x[0]); return 0; }
static uint64_t h___cxa_throw(CPUState& c) {
    fprintf(stderr, "[cxa] exception thrown (not supported)\n");
    abort();
    return 0;
}
static uint64_t h___cxa_begin_catch(CPUState& c) { return c.x[0]; }
static uint64_t h___cxa_end_catch(CPUState& c) { return 0; }
static uint64_t h___cxa_rethrow(CPUState& c) {
    fprintf(stderr, "[cxa] rethrow (not supported)\n");
    abort();
    return 0;
}
static uint64_t h___gxx_personality_v0(CPUState& c) { return 0; }
static uint64_t h___cxa_guard_acquire(CPUState& c) {
    // guard object at c.x[0]: bit 0 = initialized, bit 1 = in-progress
    uint8_t* g = (uint8_t*)c.x[0];
    if (*g & 1) return 0; // already initialized
    *g |= 2; // mark in-progress
    return 1;
}
static uint64_t h___cxa_guard_release(CPUState& c) {
    uint8_t* g = (uint8_t*)c.x[0];
    *g = (*g & ~2) | 1; // clear in-progress, set initialized
    return 0;
}
static uint64_t h___cxa_guard_abort(CPUState& c) {
    uint8_t* g = (uint8_t*)c.x[0];
    *g &= ~2;
    return 0;
}
static uint64_t h___emutls_get_address(CPUState& c) {
    // emutls: retorna um endereco de uma variavel thread-local.
    // Para single-thread, alocamos uma estrutura por simbolo.
    static std::unordered_map<uint64_t, void*> tls_vars;
    uint64_t key = c.x[0]; // control object
    auto it = tls_vars.find(key);
    if (it == tls_vars.end()) {
        void* p = calloc(1, 256);
        tls_vars[key] = p;
        return (uint64_t)p;
    }
    return (uint64_t)it->second;
}
static uint64_t h___cxa_thread_atexit(CPUState& c) { return 0; }

// ===========================================================================
// Registro
// ===========================================================================
void register_shims(bully::elf::Linker& linker) {
#define REG(name) linker.add_host_symbol(#name, h_##name)

    // memoria
    REG(malloc); REG(calloc); REG(realloc); REG(free);
    // string/mem
    REG(memcpy); REG(memset); REG(memcmp); REG(memmove); REG(memchr); REG(memmem);
    REG(strlen); REG(strnlen); REG(strcpy); REG(strncpy); REG(strcmp); REG(strncmp);
    REG(strchr); REG(strrchr); REG(strstr); REG(strdup); REG(strndup);
    REG(strcat); REG(strncat); REG(strcasecmp); REG(strncasecmp);
    REG(strtok); REG(strtok_r); REG(strerror); REG(strcspn); REG(strspn); REG(strpbrk);
    // stdio
    REG(sprintf); REG(snprintf); REG(printf); REG(fprintf);
    REG(fopen); REG(fclose); REG(fread); REG(fwrite); REG(fseek); REG(ftell);
    REG(fflush); REG(fgets); REG(fputs); REG(fputc); REG(rewind); REG(feof);
    REG(ferror); REG(setvbuf); REG(fgetpos); REG(fsetpos); REG(clearerr); REG(ungetc);
    REG(vfprintf); REG(vsnprintf);
    // filesystem
    REG(unlink); REG(remove); REG(rename); REG(access); REG(stat);
    REG(open); REG(read); REG(write); REG(close); REG(lseek);
    REG(mkdir); REG(rmdir);
    // mmap
    REG(mmap); REG(munmap); REG(mprotect);
    // time/rand
    REG(rand); REG(srand); REG(time); REG(clock); REG(gettimeofday);
    REG(usleep); REG(sleep);
    // misc
    REG(abort); REG(exit); REG(atexit); REG(getenv); REG(setenv); REG(unsetenv);
    REG(system); REG(abs); REG(labs); REG(atoi); REG(atol); REG(atof);
    REG(strtol); REG(strtoll); REG(strtoul); REG(strtoull); REG(strtod);
    REG(qsort); REG(bsearch);
    // errno / sF
    REG(__errno); REG(__sF);
    // libm
    REG(sin); REG(cos); REG(tan); REG(asin); REG(acos); REG(atan); REG(atan2);
    REG(sinh); REG(cosh); REG(tanh); REG(exp); REG(exp2); REG(log); REG(log2); REG(log10);
    REG(pow); REG(sqrt); REG(cbrt); REG(fabs); REG(floor); REG(ceil); REG(round);
    REG(fmod); REG(ldexp); REG(frexp); REG(modf); REG(trunc); REG(nearbyint);
    REG(roundeven); REG(scalbn); REG(copysign); REG(nextafter); REG(fdim);
    REG(fmax); REG(fmin); REG(hypot); REG(remainder); REG(tgamma); REG(lgamma);
    REG(erf); REG(erfc);
    REG(sinf); REG(cosf); REG(tanf); REG(asinf); REG(acosf); REG(atanf); REG(atan2f);
    REG(sinhf); REG(coshf); REG(tanhf); REG(expf); REG(logf); REG(log10f);
    REG(powf); REG(sqrtf); REG(cbrtf); REG(fabsf); REG(floorf); REG(ceilf); REG(roundf);
    REG(fmodf); REG(truncf); REG(fminf); REG(fmaxf); REG(hypotf);
    // libdl
    REG(dl_iterate_phdr); REG(dlopen); REG(dlsym); REG(dlclose); REG(dlerror);
    // pthread
    REG(pthread_create); REG(pthread_join); REG(pthread_detach); REG(pthread_self);
    REG(pthread_exit); REG(pthread_kill); REG(pthread_once);
    REG(pthread_mutex_init); REG(pthread_mutex_destroy); REG(pthread_mutex_lock);
    REG(pthread_mutex_unlock); REG(pthread_mutex_trylock);
    REG(pthread_rwlock_init); REG(pthread_rwlock_destroy); REG(pthread_rwlock_rdlock);
    REG(pthread_rwlock_wrlock); REG(pthread_rwlock_unlock);
    REG(pthread_cond_init); REG(pthread_cond_destroy); REG(pthread_cond_wait);
    REG(pthread_cond_signal); REG(pthread_cond_broadcast); REG(pthread_cond_timedwait);
    REG(pthread_key_create); REG(pthread_key_delete); REG(pthread_getspecific);
    REG(pthread_setspecific);
    REG(pthread_attr_init); REG(pthread_attr_destroy); REG(pthread_attr_setdetachstate);
    REG(pthread_sigmask); REG(pthread_setname_np); REG(pthread_getname_np);
    // cxxabi
    REG(__cxa_finalize); REG(__cxa_atexit); REG(__register_atfork);
    REG(__stack_chk_fail); REG(__cxa_pure_virtual);
    REG(__cxa_allocate_exception); REG(__cxa_free_exception); REG(__cxa_throw);
    REG(__cxa_begin_catch); REG(__cxa_end_catch); REG(__cxa_rethrow);
    REG(__gxx_personality_v0);
    REG(__cxa_guard_acquire); REG(__cxa_guard_release); REG(__cxa_guard_abort);
    REG(__emutls_get_address); REG(__cxa_thread_atexit);
#undef REG
}

} // namespace bully::libc
