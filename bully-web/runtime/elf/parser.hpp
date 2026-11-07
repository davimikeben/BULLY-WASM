// runtime/elf/parser.hpp
// Parser ELF64/AArch64 — mapeia o binario em memoria virtual do convidado.
#pragma once
#include "elf_types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace bully::elf {

struct Segment {
    uint32_t type;
    uint32_t flags;
    uint64_t offset;   // offset no arquivo
    uint64_t vaddr;    // endereco virtual (base + vaddr)
    uint64_t filesz;
    uint64_t memsz;
    uint64_t align;
    const uint8_t* data; // ponteiro para os dados no arquivo carregado
};

struct Symbol {
    std::string name;
    uint64_t value;   // endereco virtual (base + st_value)
    uint64_t size;
    unsigned char bind;
    unsigned char type;
    uint16_t shndx;
    bool is_import;    // SHN_UNDEF
};

struct Relocation {
    uint64_t offset;  // endereco virtual onde aplicar
    uint32_t type;
    uint32_t sym_idx;
    int64_t addend;
    std::string sym_name;
};

struct DynamicInfo {
    std::vector<std::string> needed;
    std::string soname;
    uint64_t init = 0;
    uint64_t fini = 0;
    uint64_t init_array = 0;
    uint64_t init_array_size = 0;
    uint64_t fini_array = 0;
    uint64_t fini_array_size = 0;
    uint64_t strtab = 0;
    uint64_t symtab = 0;
    uint64_t strsz = 0;
    uint64_t syment = 0;
    uint64_t rela = 0;
    uint64_t relasz = 0;
    uint64_t relaent = 0;
    uint64_t jmprel = 0;
    uint64_t pltrelsz = 0;
    uint32_t pltrel = 0;
    uint32_t flags = 0;
    uint64_t pltgot = 0;
};

// Resultado do parse de um ELF (ainda nao carregado em memoria)
struct ElfImage {
    std::string path;
    uint64_t base_addr = 0;       // endereco base de carga
    Elf64_Ehdr header;
    std::vector<Segment> segments;
    DynamicInfo dyn;
    std::vector<Symbol> exports;
    std::vector<Symbol> imports;
    std::vector<Relocation> relocations;
    std::vector<uint64_t> init_array;
    std::vector<uint64_t> fini_array;
    std::vector<uint8_t> file_data;  // todo o arquivo em memoria

    // Procura um export por nome. Retorna endereco virtual ou 0.
    uint64_t find_export(const std::string& name) const;
};

// Faz o parse de um arquivo ELF64. Retorna false em caso de erro.
bool parse_elf(const std::string& path, ElfImage& out);

// Calcula o tamanho total de memoria necessario para carregar os PT_LOAD.
uint64_t compute_load_size(const ElfImage& img);

} // namespace bully::elf
