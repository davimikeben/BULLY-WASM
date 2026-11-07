# Relatorio ELF — `libc++_shared.so`

## 1. Cabecalho ELF

- Arquitetura: **EM_AARCH64
- Classe: ELFCLASS64
- Endianness: ELFDATA2LSB
- Tipo: ET_DYN
- ABI: ELFOSABI_SYSV
- Entry point: `0x0`
- Tamanho: 1,330,832 bytes (1.3 MB)

## 2. Segmentos (Program Headers)

| Tipo | Offset | VAddr | FileSz | MemSz | Flags | Align |
|------|--------|-------|-------|-------|-------|-------|
| PT_PHDR | 0x40 | 0x40 | 672 | 672 | 4 | 8 |
| PT_LOAD | 0x0 | 0x0 | 636984 | 636984 | 4 | 4096 |
| PT_LOAD | 0x9b840 | 0x9c840 | 652032 | 652032 | 5 | 4096 |
| PT_LOAD | 0x13ab40 | 0x13cb40 | 38912 | 38912 | 6 | 4096 |
| PT_LOAD | 0x144340 | 0x147340 | 456 | 32128 | 6 | 4096 |
| PT_DYNAMIC | 0x142c20 | 0x144c20 | 464 | 464 | 6 | 8 |
| PT_GNU_RELRO | 0x13ab40 | 0x13cb40 | 38912 | 42176 | 4 | 1 |
| PT_GNU_EH_FRAME | 0x7a0d8 | 0x7a0d8 | 19028 | 19028 | 4 | 4 |
| PT_GNU_STACK | 0x0 | 0x0 | 0 | 0 | 6 | 0 |
| PT_GNU_PROPERTY | 0x3a0 | 0x3a0 | 32 | 32 | 4 | 8 |
| PT_NOTE | 0x2e0 | 0x2e0 | 188 | 188 | 4 | 4 |
| PT_NOTE | 0x3a0 | 0x3a0 | 32 | 32 | 4 | 8 |

## 3. Dependencias dinamicas (DT_NEEDED)

- `libc.so`
- `libm.so`
- `libdl.so`

SONAME: `libc++_shared.so`

## 4. Simbolos

- **Exports**: 2358
- **Imports**: 164
- **JNI exports**: 0

### 4.1 Imports agrupados por API

#### libc/libdl/pthread (72)

```
__assert2
__ctype_get_mb_cur_max
__cxa_atexit
__cxa_finalize
__cxa_thread_atexit_impl
__errno
__sF
__system_property_get
close
closedir
closelog
fclose
fopen
fread
free
freelocale
fwrite
malloc
memchr
memcmp
memcpy
memmove
memset
open
openat
opendir
openlog
pthread_cond_broadcast
pthread_cond_destroy
pthread_cond_signal
pthread_cond_timedwait
pthread_cond_wait
pthread_detach
pthread_getspecific
pthread_join
pthread_key_create
pthread_key_delete
pthread_mutex_destroy
pthread_mutex_init
pthread_mutex_lock
pthread_mutex_trylock
pthread_mutex_unlock
pthread_mutexattr_destroy
pthread_mutexattr_init
pthread_mutexattr_settype
pthread_once
pthread_rwlock_rdlock
pthread_rwlock_unlock
pthread_rwlock_wrlock
pthread_self
pthread_setspecific
read
readdir
readlink
realloc
strcmp
strcoll_l
strerror_r
strftime_l
strlen
strncmp
strtod
strtof
strtol
strtold
strtold_l
strtoll
strtoll_l
strtoul
strtoull
strtoull_l
strxfrm_l
```

#### Outros (92)

```
abort
android_set_abort_message
btowc
chdir
clock_gettime
dl_iterate_phdr
fchmod
fchmodat
fdopendir
fflush
fprintf
fputc
fseek
fseeko
fstat
ftello
ftruncate
getauxval
getc
getcwd
getenv
getpid
ioctl
isdigit_l
islower_l
isupper_l
iswalpha_l
iswblank_l
iswcntrl_l
iswdigit_l
iswlower_l
iswprint_l
iswpunct_l
iswspace_l
iswupper_l
iswxdigit_l
isxdigit_l
link
localeconv
lstat
mbrlen
mbrtowc
mbsnrtowcs
mbsrtowcs
mbtowc
mkdir
nanosleep
newlocale
pathconf
posix_memalign
realpath
remove
rename
sendfile
setlocale
snprintf
sscanf
stat
statvfs
swprintf
symlink
syscall
sysconf
syslog
tolower_l
toupper_l
towlower_l
towupper_l
truncate
ungetc
unlinkat
uselocale
utimensat
vasprintf
vfprintf
vsnprintf
vsscanf
wcrtomb
wcscoll_l
wcslen
wcsnrtombs
wcstod
wcstof
wcstol
wcstold
wcstoll
wcstoul
wcstoull
wcsxfrm_l
wctob
wmemchr
wmemcmp
```

### 4.2 Exports JNI

```
```

## 5. Relocations

| Tipo | Contagem |
|------|----------|
| R_AARCH64_ABS64 | 2003 |
| R_AARCH64_GLOB_DAT | 189 |
| R_AARCH64_JUMP_SLOT | 490 |
| R_AARCH64_RELATIVE | 1477 |

### 5.1 Amostras de relocations

#### R_AARCH64_RELATIVE

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x13cb40 | 1297216 |  |
| 0x13cfd0 | 647360 |  |
| 0x13cfd8 | 647368 |  |
| 0x13cfe0 | 647376 |  |
| 0x13cfe8 | 647384 |  |

#### R_AARCH64_GLOB_DAT

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x144e40 | 0 | free |
| 0x144f10 | 0 | __sF |
| 0x144f58 | 0 | __cxa_thread_atexit_impl |
| 0x145000 | 0 | _ZTINSt6__ndk18ios_base7failureE |
| 0x144ff8 | 0 | _ZTVNSt6__ndk18ios_baseE |

#### R_AARCH64_ABS64

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x13d260 | 0 | _ZTSPKa |
| 0x13d120 | 0 | _ZTSPKb |
| 0x140d40 | 0 | _ZTSNSt6__ndk113basic_istreamIcNS_11char_traitsIcEEEE |
| 0x142388 | 0 | _ZTINSt6__ndk115__codecvt_utf16IDsLb1EEE |
| 0x143bd8 | 0 | _ZTINSt6__ndk114codecvt_bynameIDsDu9mbstate_tEE |

#### R_AARCH64_JUMP_SLOT

| Offset | Addend | Simbolo |
|--------|--------|---------|
| 0x1453f0 | 0 | __cxa_finalize |
| 0x1453f8 | 0 | __cxa_atexit |
| 0x145400 | 0 | __cxa_allocate_exception |
| 0x145408 | 0 | _ZNSt8bad_castC1Ev |
| 0x145410 | 0 | _ZNSt8bad_castD1Ev |

## 6. Inicializacao

- .init_array entries: 3

```
0x0
0x0
0x0
```

- .init: ausente
- .fini: ausente

## 7. TLS

- Nenhuma secao TLS encontrada

## 8. Tags dinamicas

| Tag | Valor |
|-----|-------|
| DT_NEEDED | libc.so |
| DT_NEEDED | libm.so |
| DT_NEEDED | libdl.so |
| DT_SONAME | libc++_shared.so |
| DT_FLAGS | 8 |
| DT_FLAGS_1 | 1 |
| DT_RELA | 222976 |
| DT_RELASZ | 88056 |
| DT_RELAENT | 24 |
| DT_RELACOUNT | 1477 |
| DT_JMPREL | 311032 |
| DT_PLTRELSZ | 11760 |
| DT_PLTGOT | 1332184 |
| DT_PLTREL | 7 |
| DT_AARCH64_BTI_PLT | 0 |
| DT_SYMTAB | 960 |
| DT_SYMENT | 24 |
| DT_STRTAB | 102716 |
| DT_STRSZ | 120259 |
| DT_GNU_HASH | 66624 |
| DT_HASH | 82524 |
| DT_INIT_ARRAY | 1330184 |
| DT_INIT_ARRAYSZ | 24 |
| DT_FINI_ARRAY | 1330168 |
| DT_FINI_ARRAYSZ | 16 |
| DT_VERSYM | 61512 |
| DT_VERNEED | 66560 |
| DT_VERNEEDNUM | 2 |
| DT_NULL | 0 |
