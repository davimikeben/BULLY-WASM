# Relatorio ELF — `libGame.so`

## 1. Cabecalho ELF

- Arquitetura: **EM_AARCH64
- Classe: ELFCLASS64
- Endianness: ELFDATA2LSB
- Tipo: ET_DYN
- ABI: ELFOSABI_SYSV
- Entry point: `0x0`
- Tamanho: 19,252,456 bytes (18.4 MB)

## 2. Segmentos (Program Headers)

| Tipo | Offset | VAddr | FileSz | MemSz | Flags | Align |
|------|--------|-------|-------|-------|-------|-------|
| PT_PHDR | 0x40 | 0x40 | 504 | 504 | 4 | 8 |
| PT_LOAD | 0x0 | 0x0 | 18518672 | 18518672 | 5 | 4096 |
| PT_LOAD | 0x11a9290 | 0x11aa290 | 592384 | 592384 | 6 | 4096 |
| PT_LOAD | 0x1239c90 | 0x123bc90 | 139272 | 3477208 | 6 | 4096 |
| PT_DYNAMIC | 0x1211fa8 | 0x1212fa8 | 544 | 544 | 6 | 8 |
| PT_GNU_RELRO | 0x11a9290 | 0x11aa290 | 592384 | 593264 | 4 | 1 |
| PT_GNU_EH_FRAME | 0x5ea954 | 0x5ea954 | 336932 | 336932 | 4 | 4 |
| PT_GNU_STACK | 0x0 | 0x0 | 0 | 0 | 6 | 0 |
| PT_NOTE | 0x238 | 0x238 | 188 | 188 | 4 | 4 |

## 3. Dependencias dinamicas (DT_NEEDED)

- `libandroid.so`
- `liblog.so`
- `libEGL.so`
- `libGLESv3.so`
- `libVendor_mpg123.so`
- `libopenal.so`
- `libm.so`
- `libc++_shared.so`
- `libdl.so`
- `libc.so`

SONAME: `libGame.so`

## 4. Simbolos

- **Exports**: 49938
- **Imports**: 417
- **JNI exports**: 38

### 4.1 Imports agrupados por API

#### libc/libdl/pthread (92)

```
__android_log_print
__cxa_allocate_exception
__cxa_atexit
__cxa_begin_catch
__cxa_end_catch
__cxa_finalize
__cxa_free_exception
__cxa_guard_abort
__cxa_guard_acquire
__cxa_guard_release
__cxa_pure_virtual
__cxa_rethrow
__cxa_thread_atexit
__cxa_throw
__dynamic_cast
__emutls_get_address
__errno
__fread_chk
__gxx_personality_v0
__memcpy_chk
__memmove_chk
__memset_chk
__register_atfork
__sF
__stack_chk_fail
__strcat_chk
__strchr_chk
__strcpy_chk
__strlen_chk
__strncpy_chk
__strncpy_chk2
__strrchr_chk
__system_property_get
__vsnprintf_chk
__vsprintf_chk
calloc
fclose
fopen
fread
free
fwrite
malloc
memalign
memchr
memcmp
memcpy
memmove
memset
printf
pthread_attr_destroy
pthread_attr_getschedparam
pthread_attr_getstacksize
pthread_attr_init
pthread_attr_setschedparam
pthread_create
pthread_getspecific
pthread_join
pthread_key_create
pthread_key_delete
pthread_mutex_destroy
pthread_mutex_init
pthread_mutex_lock
pthread_mutex_unlock
pthread_mutexattr_destroy
pthread_mutexattr_init
pthread_mutexattr_settype
pthread_rwlock_rdlock
pthread_rwlock_unlock
pthread_rwlock_wrlock
pthread_setname_np
pthread_setschedparam
pthread_setspecific
realloc
strcasecmp
strcat
strchr
strcmp
strcoll
strcpy
strcspn
strerror
strlen
strncasecmp
strncat
strncmp
strncpy
strrchr
strstr
strtod
strtok
strtok_r
strtoul
```

#### Outros (193)

```
_ZNKSt11logic_error4whatEv
_ZNKSt13runtime_error4whatEv
_ZNKSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE4findEcm
_ZNKSt6__ndk119__shared_weak_count13__get_deleterERKSt9type_info
_ZNKSt6__ndk123__match_any_but_newlineIcE6__execERNS_7__stateIcEE
_ZNKSt6__ndk16locale4nameEv
_ZNKSt6__ndk16locale9use_facetERNS0_2idE
_ZNKSt6__ndk18ios_base6getlocEv
_ZNSt11logic_errorC1EPKc
_ZNSt11logic_errorC2EPKc
_ZNSt11logic_errorC2ERKS_
_ZNSt11logic_errorD1Ev
_ZNSt11logic_errorD2Ev
_ZNSt12length_errorD1Ev
_ZNSt12out_of_rangeD1Ev
_ZNSt13runtime_errorC2ERKS_
_ZNSt20bad_array_new_lengthC1Ev
_ZNSt20bad_array_new_lengthD1Ev
_ZNSt6__ndk111regex_errorC1ENS_15regex_constants10error_typeE
_ZNSt6__ndk111regex_errorD1Ev
_ZNSt6__ndk112__next_primeEm
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE5eraseEmm
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKc
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEPKcm
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6appendEmc
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKc
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6assignEPKcm
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE6resizeEmc
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE7reserveEm
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9__grow_byEmmmmmm
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEE9push_backEc
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEC1ERKS5_
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEED2Ev
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEaSERKS5_
_ZNSt6__ndk112basic_stringIcNS_11char_traitsIcEENS_9allocatorIcEEEaSEc
_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEE4readEPcl
_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEE5seekgENS_4fposI9mbstate_tEE
_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEE5tellgEv
_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED0Ev
_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED1Ev
_ZNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED2Ev
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEE6sentryC1ERS3_
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEE6sentryD1Ev
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED0Ev
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED1Ev
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED2Ev
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEf
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEi
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEj
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEl
_ZNSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEElsEm
_ZNSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED0Ev
_ZNSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED1Ev
_ZNSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED2Ev
_ZNSt6__ndk115__get_classnameEPKcb
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE4syncEv
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE5imbueERKNS_6localeE
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE5uflowEv
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE6setbufEPcl
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE6xsgetnEPcl
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE6xsputnEPKcl
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEE9showmanycEv
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEEC2Ev
_ZNSt6__ndk115basic_streambufIcNS_11char_traitsIcEEED2Ev
_ZNSt6__ndk117iostream_categoryEv
_ZNSt6__ndk119__shared_weak_count14__release_weakEv
_ZNSt6__ndk119__shared_weak_countD2Ev
_ZNSt6__ndk120__get_collation_nameEPKc
_ZNSt6__ndk15alignEmmRPvRm
_ZNSt6__ndk15ctypeIcE2idE
_ZNSt6__ndk15mutex4lockEv
_ZNSt6__ndk15mutex6unlockEv
_ZNSt6__ndk15mutexD1Ev
_ZNSt6__ndk16localeC1ERKS0_
_ZNSt6__ndk16localeC1Ev
_ZNSt6__ndk16localeD1Ev
_ZNSt6__ndk16localeaSERKS0_
_ZNSt6__ndk17collateIcE2idE
_ZNSt6__ndk18ios_base33__set_badbit_and_consider_rethrowEv
_ZNSt6__ndk18ios_base4initEPv
_ZNSt6__ndk18ios_base5clearEj
_ZNSt6__ndk18ios_base5imbueERKNS_6localeE
_ZNSt6__ndk18ios_base7failureC1EPKcRKNS_10error_codeE
_ZNSt6__ndk18ios_base7failureD1Ev
_ZNSt6__ndk18ios_base7failureD2Ev
_ZNSt6__ndk19basic_iosIcNS_11char_traitsIcEEED2Ev
_ZNSt6__ndk19to_stringEf
_ZNSt9exceptionD2Ev
_ZSt9terminatev
_ZTH7gString
_ZTH8gString2
_ZTINSt6__ndk111regex_errorE
_ZTINSt6__ndk113basic_istreamIcNS_11char_traitsIcEEEE
_ZTINSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEEE
_ZTINSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEEE
_ZTINSt6__ndk115basic_streambufIcNS_11char_traitsIcEEEE
_ZTINSt6__ndk119__shared_weak_countE
_ZTINSt6__ndk18ios_base7failureE
_ZTISt11logic_error
_ZTISt12length_error
_ZTISt12out_of_range
_ZTISt20bad_array_new_length
_ZTISt9exception
_ZTVN10__cxxabiv117__class_type_infoE
_ZTVN10__cxxabiv120__si_class_type_infoE
_ZTVN10__cxxabiv121__vmi_class_type_infoE
_ZTVNSt6__ndk112system_errorE
_ZTVNSt6__ndk18ios_base7failureE
_ZTVNSt6__ndk19basic_iosIcNS_11char_traitsIcEEEE
_ZTVSt12length_error
_ZTVSt12out_of_range
_ZThn16_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED0Ev
_ZThn16_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED1Ev
_ZTv0_n24_NSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED0Ev
_ZTv0_n24_NSt6__ndk113basic_istreamIcNS_11char_traitsIcEEED1Ev
_ZTv0_n24_NSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED0Ev
_ZTv0_n24_NSt6__ndk113basic_ostreamIcNS_11char_traitsIcEEED1Ev
_ZTv0_n24_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED0Ev
_ZTv0_n24_NSt6__ndk114basic_iostreamIcNS_11char_traitsIcEEED1Ev
_ZdaPv
_ZdlPv
_Znam
_Znwm
abort
acosf
asinf
atan2f
atanf
atof
atoi
bsearch
cos
cosf
dl_iterate_phdr
exit
exp
exp2f
expf
feof
ferror
fflush
fgetc
fgets
fmodf
fprintf
fseek
ftell
getauxval
getc
getenv
getpid
gettid
gettimeofday
ldexp
localtime_r
log
log10f
longjmp
mkdir
nanosleep
pow
powf
putchar
puts
qsort
rand
rename
sched_get_priority_max
sched_get_priority_min
sem_destroy
sem_getvalue
sem_init
sem_post
sem_trywait
sem_wait
setjmp
sin
sincos
sincosf
sinf
sprintf
srand
sscanf
stat
stderr
stdin
syscall
tan
tanf
ungetc
unlink
usleep
vsnprintf
```

#### OpenGL ES (77)

```
glActiveTexture
glAttachShader
glBindAttribLocation
glBindBuffer
glBindFramebuffer
glBindRenderbuffer
glBindTexture
glBindVertexArray
glBlendFunc
glBlendFuncSeparate
glBufferData
glClear
glClearColor
glClearDepthf
glClearStencil
glColorMask
glCompileShader
glCompressedTexImage2D
glCompressedTexSubImage2D
glCreateProgram
glCreateShader
glCullFace
glDeleteBuffers
glDeleteFramebuffers
glDeleteProgram
glDeleteRenderbuffers
glDeleteShader
glDeleteTextures
glDeleteVertexArrays
glDepthFunc
glDepthMask
glDisable
glDisableVertexAttribArray
glDrawBuffers
glDrawElements
glEnable
glEnableVertexAttribArray
glFinish
glFramebufferRenderbuffer
glFramebufferTexture2D
glFrontFace
glGenBuffers
glGenFramebuffers
glGenRenderbuffers
glGenTextures
glGenVertexArrays
glGetBooleanv
glGetError
glGetIntegerv
glGetProgramInfoLog
glGetProgramiv
glGetShaderInfoLog
glGetShaderiv
glGetString
glGetUniformLocation
glLineWidth
glLinkProgram
glPixelStorei
glPolygonOffset
glReadPixels
glRenderbufferStorage
glScissor
glShaderSource
glStencilFunc
glStencilMask
glStencilOp
glTexImage2D
glTexParameterf
glTexParameteri
glTexStorage2D
glTexSubImage2D
glUniform1i
glUniform4fv
glUniformMatrix4fv
glUseProgram
glVertexAttribPointer
glViewport
```

#### EGL (12)

```
eglChooseConfig
eglCreateContext
eglCreateWindowSurface
eglDestroySurface
eglGetConfigAttrib
eglGetDisplay
eglGetError
eglGetProcAddress
eglInitialize
eglMakeCurrent
eglSwapBuffers
eglSwapInterval
```

#### OpenAL (31)

```
alBufferData
alDeleteBuffers
alDeleteFilters
alDeleteSources
alFilterf
alFilteri
alGenBuffers
alGenFilters
alGenSources
alGetBufferi
alGetError
alGetSource3f
alGetSourcef
alGetSourcei
alIsFilter
alListener3f
alListenerfv
alSource3f
alSourcePause
alSourcePlay
alSourceQueueBuffers
alSourceRewind
alSourceStop
alSourceUnqueueBuffers
alSourcef
alSourcei
alcCloseDevice
alcCreateContext
alcIsExtensionPresent
alcMakeContextCurrent
alcOpenDevice
```

#### Android (libandroid) (12)

```
AAssetManager_fromJava
AAssetManager_open
AAsset_close
AAsset_getLength64
AAsset_getRemainingLength64
AAsset_read
AAsset_seek64
ANativeWindow_fromSurface
ANativeWindow_getHeight
ANativeWindow_getWidth
ANativeWindow_release
ANativeWindow_setBuffersGeometry
```

### 4.2 Exports JNI

```
Java_com_rockstargames_oswrapper_GameNative_implOnTouchMove
Java_com_rockstargames_oswrapper_GameNative_implOnHttpRequestData
Java_com_rockstargames_oswrapper_GameNative_implOnSurfaceChanged
Java_com_rockstargames_oswrapper_GameNative_implOnPlaylistOpenComplete
Java_com_rockstargames_oswrapper_GameNative_implOnGamepadButtonUp
Java_com_rockstargames_oswrapper_GameNative_implOnGamepadButtonDown
Java_com_rockstargames_oswrapper_GameNative_implOnGamepadCountChanged
Java_com_rockstargames_oswrapper_GameNative_implOnKeyboardButtonUp
Java_com_rockstargames_oswrapper_GameNative_implOnSurfaceCreated
Java_com_rockstargames_oswrapper_GameNative_implOnResume
Java_com_rockstargames_oswrapper_GameNative_implOnHttpRequestFinished
Java_com_rockstargames_oswrapper_GameNative_implOnHttpRequestResponse
Java_com_rockstargames_oswrapper_GameNative_implOnKeyboardButtonDown
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarGateComplete
Java_com_rockstargames_oswrapper_GameNative_implOnPause
Java_com_rockstargames_oswrapper_GameNative_implOnActivityDestroyed
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarTicketChanged
Java_com_rockstargames_oswrapper_GameNative_implOnNetworkChanged
Java_com_rockstargames_oswrapper_GameNative_implOnTouchEnd
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarDeleteAccountComplete
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarIdChanged
Java_com_rockstargames_oswrapper_GameNative_implOnLowMemory
Java_com_rockstargames_oswrapper_GameNative_implOnBackButtonPressed
Java_com_rockstargames_oswrapper_GameNative_implOnGamepadAxesChanged
Java_com_rockstargames_oswrapper_GameNative_implIsInitialized
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarCloudDisabledComplete
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarInitialComplete
Java_com_rockstargames_oswrapper_GameNative_implOnHttpRequestError
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarSignInComplete
Java_com_rockstargames_oswrapper_GameNative_implOnInitialSetup
Java_com_rockstargames_oswrapper_GameNative_implOnActivityCreated
Java_com_rockstargames_oswrapper_GameNative_implOnTouchStart
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarSetup
Java_com_rockstargames_oswrapper_GameNative_implOnSurfaceDestroyed
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarSignOutComplete
Java_com_rockstargames_oswrapper_GameNative_implOnAccelerometerChanged
Java_com_rockstargames_oswrapper_GameNative_implOnDrawFrame
Java_com_rockstargames_oswrapper_GameNative_implOnRockstarStateChanged
```

## 5. Relocations

| Tipo | Contagem |
|------|----------|
| R_AARCH64_ABS64 | 42596 |
| R_AARCH64_GLOB_DAT | 7844 |
| R_AARCH64_JUMP_SLOT | 12466 |
| R_AARCH64_RELATIVE | 12127 |

### 5.1 Amostras de relocations

#### R_AARCH64_RELATIVE

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x11aa290 | 18522768 |  |
| 0x11b37a0 | 18560984 |  |
| 0x11b37b0 | 9224500 |  |
| 0x11b37c0 | 9224548 |  |
| 0x11b37e0 | 6112947 |  |

#### R_AARCH64_ABS64

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x11ab4b8 | 16 | _ZTVN10__cxxabiv120__si_class_type_infoE |
| 0x11ab588 | 16 | _ZTVN10__cxxabiv120__si_class_type_infoE |
| 0x11ab648 | 16 | _ZTVN10__cxxabiv120__si_class_type_infoE |
| 0x11ab6e8 | 16 | _ZTVN10__cxxabiv120__si_class_type_infoE |
| 0x11ab7d0 | 16 | _ZTVN10__cxxabiv120__si_class_type_infoE |

#### R_AARCH64_GLOB_DAT

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x1215768 | 0 | _ZNSt6__ndk15mutexD1Ev |
| 0x1215dd8 | 0 | alDeleteFilters |
| 0x1215de0 | 0 | alGenFilters |
| 0x1215de8 | 0 | alFilteri |
| 0x1215df0 | 0 | alFilterf |

#### R_AARCH64_JUMP_SLOT

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x1222700 | 0 | __cxa_finalize |
| 0x1222708 | 0 | __cxa_atexit |
| 0x1222710 | 0 | __register_atfork |
| 0x1222718 | 0 | _ZN5name811setWithTextEPKc |
| 0x1222720 | 0 | _ZN11WarLangType10AddNewTypeEPS_ |

## 6. Inicializacao

- .init_array entries: 506

```
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
0x0
... (+486 mais)
```

- .init: ausente
- .fini: ausente

## 7. TLS

- Nenhuma secao TLS encontrada

## 8. Tags dinamicas

| Tag | Valor |
|-----|-------|
| DT_NEEDED | libandroid.so |
| DT_NEEDED | liblog.so |
| DT_NEEDED | libEGL.so |
| DT_NEEDED | libGLESv3.so |
| DT_NEEDED | libVendor_mpg123.so |
| DT_NEEDED | libopenal.so |
| DT_NEEDED | libm.so |
| DT_NEEDED | libc++_shared.so |
| DT_NEEDED | libdl.so |
| DT_NEEDED | libc.so |
| DT_SONAME | libGame.so |
| DT_FLAGS | 8 |
| DT_FLAGS_1 | 1 |
| DT_RELA | 3681696 |
| DT_RELASZ | 1501608 |
| DT_RELAENT | 24 |
| DT_RELACOUNT | 12127 |
| DT_JMPREL | 5183304 |
| DT_PLTRELSZ | 299184 |
| DT_PLTGOT | 19015400 |
| DT_PLTREL | 7 |
| DT_SYMTAB | 760 |
| DT_SYMENT | 24 |
| DT_STRTAB | 1690904 |
| DT_STRSZ | 1990789 |
| DT_GNU_HASH | 1310128 |
| DT_INIT_ARRAY | 18948056 |
| DT_INIT_ARRAYSZ | 4048 |
| DT_FINI_ARRAY | 18948040 |
| DT_FINI_ARRAYSZ | 16 |
| DT_VERSYM | 1209304 |
| DT_VERNEED | 1310016 |
| DT_VERNEEDNUM | 3 |
| DT_NULL | 0 |
