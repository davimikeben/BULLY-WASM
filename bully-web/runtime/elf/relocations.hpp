// runtime/elf/relocations.hpp
// Aplicacao de relocations AArch64 para o runtime.
#pragma once
#include "parser.hpp"
#include <cstdint>

namespace bully::elf {

// Contexto de relocacao: mapa de simbolos resolvidos (nome -> endereco virtual).
struct SymbolResolver {
    // Resolve um simbolo importado. Retorna endereco virtual ou 0 se nao encontrado.
    // implementado pelo linker.
    uint64_t (*resolve)(void* ctx, const std::string& name);
    void* ctx;
};

// Aplica todas as relocations de uma imagem carregada.
// `base` e o endereco base onde a imagem foi mapeada.
// `mem` e a memoria do convidado (ponteiro real para a regiao mapeada).
bool apply_relocations(ElfImage& img, uint8_t* mem, const SymbolResolver& resolver);

// Aplica uma unica relocation. Usada internamente e para testes.
bool apply_one(uint32_t type, uint8_t* mem, uint64_t offset, uint64_t sym_addr, int64_t addend, uint64_t base);

} // namespace bully::elf
