// runtime/elf/elf_types.hpp
// Tipos ELF64 para AArch64 — usados pelo loader do runtime bully-web.
#pragma once
#include <cstdint>
#include <cstddef>

namespace bully::elf {

// ELF64 header
struct Elf64_Ehdr {
    unsigned char e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint64_t e_entry;
    uint64_t e_phoff;
    uint64_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

// ELF64 program header
struct Elf64_Phdr {
    uint32_t p_type;
    uint32_t p_flags;
    uint64_t p_offset;
    uint64_t p_vaddr;
    uint64_t p_paddr;
    uint64_t p_filesz;
    uint64_t p_memsz;
    uint64_t p_align;
};

// ELF64 section header
struct Elf64_Shdr {
    uint32_t sh_name;
    uint32_t sh_type;
    uint64_t sh_flags;
    uint64_t sh_addr;
    uint64_t sh_offset;
    uint64_t sh_size;
    uint32_t sh_link;
    uint32_t sh_info;
    uint64_t sh_addralign;
    uint64_t sh_entsize;
};

// ELF64 dynamic symbol
struct Elf64_Sym {
    uint32_t st_name;
    unsigned char st_info;
    unsigned char st_other;
    uint16_t st_shndx;
    uint64_t st_value;
    uint64_t st_size;
};

// ELF64 relocation (with addend)
struct Elf64_Rela {
    uint64_t r_offset;
    uint64_t r_info;
    int64_t r_addend;
};

// Dynamic section entry
struct Elf64_Dyn {
    int64_t d_tag;
    union {
        uint64_t d_val;
        uint64_t d_ptr;
    } d_un;
};

// ---- Constantes ----
static constexpr uint16_t ET_DYN  = 3;
static constexpr uint16_t EM_AARCH64 = 183;

// Program header types
static constexpr uint32_t PT_LOAD    = 1;
static constexpr uint32_t PT_DYNAMIC = 2;
static constexpr uint32_t PT_TLS     = 7;
static constexpr uint32_t PT_GNU_RELRO = 0x6474e552;

// Dynamic tags
static constexpr int64_t DT_NULL     = 0;
static constexpr int64_t DT_NEEDED   = 1;
static constexpr int64_t DT_STRTAB   = 5;
static constexpr int64_t DT_SYMTAB   = 6;
static constexpr int64_t DT_STRSZ    = 10;
static constexpr int64_t DT_INIT     = 12;
static constexpr int64_t DT_FINI     = 13;
static constexpr int64_t DT_SONAME   = 14;
static constexpr int64_t DT_INIT_ARRAY  = 25;
static constexpr int64_t DT_FINI_ARRAY  = 26;
static constexpr int64_t DT_INIT_ARRAYSZ = 27;
static constexpr int64_t DT_FINI_ARRAYSZ = 28;
static constexpr int64_t DT_RELA     = 7;
static constexpr int64_t DT_RELASZ   = 8;
static constexpr int64_t DT_RELAENT  = 9;
static constexpr int64_t DT_PLTGOT   = 3;
static constexpr int64_t DT_PLTREL   = 20;
static constexpr int64_t DT_JMPREL   = 23;
static constexpr int64_t DT_SYMENT   = 11;
static constexpr int64_t DT_FLAGS    = 30;
static constexpr int64_t DT_BIND_NOW = 24;
static constexpr int64_t DT_ANDROID_REL = 0x6000000f;
static constexpr int64_t DT_ANDROID_RELSZ = 0x60000010;

// Section types
static constexpr uint32_t SHT_DYNAMIC = 6;
static constexpr uint32_t SHT_DYNSYM  = 11;
static constexpr uint32_t SHT_RELA    = 4;
static constexpr uint32_t SHT_INIT_ARRAY = 14;
static constexpr uint32_t SHT_FINI_ARRAY = 15;
static constexpr uint32_t SHT_STRTAB  = 3;

// Symbol binding/type
static constexpr unsigned char STB_GLOBAL = 1;
static constexpr unsigned char STB_WEAK   = 2;
static constexpr unsigned char STT_FUNC   = 2;
static constexpr unsigned char STT_OBJECT = 1;
static constexpr unsigned char STT_NOTYPE = 0;

inline unsigned char ELF64_ST_BIND(unsigned char i) { return i >> 4; }
inline unsigned char ELF64_ST_TYPE(unsigned char i) { return i & 0xf; }

// Relocation macros for AArch64
inline uint32_t ELF64_R_SYM(uint64_t i)  { return (uint32_t)(i >> 32); }
inline uint32_t ELF64_R_TYPE(uint64_t i) { return (uint32_t)(i & 0xffffffff); }

// AArch64 relocation types
namespace R_AARCH64 {
    static constexpr uint32_t NONE       = 0;
    static constexpr uint32_t ABS64      = 257;
    static constexpr uint32_t GLOB_DAT   = 1025;
    static constexpr uint32_t JUMP_SLOT  = 1026;
    static constexpr uint32_t RELATIVE   = 1027;
    static constexpr uint32_t IRELATIVE  = 1032;
}

} // namespace bully::elf
