// runtime/elf/relocations.cpp
#include "relocations.hpp"
#include <cstdio>
#include <cstring>

namespace bully::elf {

// Escreve um valor de 64 bits em endereco na memoria do convidado.
static inline void write64(uint8_t* mem, uint64_t addr, uint64_t val) {
    memcpy(mem + addr, &val, 8);
}

bool apply_one(uint32_t type, uint8_t* mem, uint64_t offset, uint64_t sym_addr, int64_t addend, uint64_t base) {
    switch (type) {
        case R_AARCH64::NONE:
            return true;

        case R_AARCH64::ABS64:
            // *ptr = sym_addr + addend
            write64(mem, offset, sym_addr + (uint64_t)addend);
            return true;

        case R_AARCH64::RELATIVE:
            // *ptr = base + addend
            write64(mem, offset, base + (uint64_t)addend);
            return true;

        case R_AARCH64::GLOB_DAT:
            // *ptr = sym_addr + addend
            write64(mem, offset, sym_addr + (uint64_t)addend);
            return true;

        case R_AARCH64::JUMP_SLOT:
            // *ptr = sym_addr (addend normalmente 0)
            write64(mem, offset, sym_addr + (uint64_t)addend);
            return true;

        default:
            fprintf(stderr, "[reloc] unsupported type %u at offset 0x%lx\n", type, offset);
            return false;
    }
}

bool apply_relocations(ElfImage& img, uint8_t* mem, const SymbolResolver& resolver) {
    uint64_t base = img.base_addr;
    size_t ok = 0, fail = 0;

    // Mapa nome->simbolo dos exports locais
    std::unordered_map<std::string, uint64_t> local_exports;
    for (const auto& s : img.exports) {
        local_exports[s.name] = base + s.value;
    }

    for (auto& rel : img.relocations) {
        uint64_t sym_addr = 0;
        if (rel.sym_idx != 0) {
            // Resolve: primeiro procura local (para casos de simbolos definidos aqui),
            // depois externo via resolver.
            // Nota: o indice do simbolo nao corresponde diretamente a nossa lista;
            // para robustez, tentamos resolver por nome.
            if (!rel.sym_name.empty()) {
                auto it = local_exports.find(rel.sym_name);
                if (it != local_exports.end()) {
                    sym_addr = it->second;
                } else if (resolver.resolve && resolver.ctx) {
                    sym_addr = resolver.resolve(resolver.ctx, rel.sym_name);
                }
            }
        }
        bool r = apply_one(rel.type, mem, rel.offset, sym_addr, rel.addend, base);
        if (r) ok++; else fail++;
    }

    fprintf(stderr, "[reloc] applied %zu relocations (%zu failed)\n", ok, fail);
    return fail == 0;
}

} // namespace bully::elf
