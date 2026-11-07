// runtime/elf/linker.hpp
// Linker dinamico: carrega libGame.so + dependencias, resolve simbolos,
// aplica relocations e chama inicializadores via interpretador ARM64.
#pragma once
#include "parser.hpp"
#include "../arm64/cpu.hpp"
#include "../arm64/interpreter.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <cstdint>

namespace bully::elf {

// Uma imagem carregada em memoria.
struct LoadedImage {
    ElfImage elf;
    uint8_t* mem = nullptr;  // ponteiro para a regiao alocada (base real)
    uint64_t base = 0;       // endereco virtual base
    bool initialized = false;
};

// Linker: gerencia varias imagens e resolve simbolos entre elas.
class Linker {
public:
    Linker();
    ~Linker();

    // Carrega uma biblioteca e suas dependencias recursivamente.
    bool load(const std::string& path, const std::vector<std::string>& search_paths);

    // Resolve um simbolo global em todas as imagens carregadas.
    uint64_t resolve(const std::string& name);

    // Registra um symbolo fornecido pelo host (shims: libc, GLES, etc).
    void add_host_symbol(const std::string& name, arm64::HostFn fn);

    // Chama todos os init_array em ordem topologica.
    bool run_init();

    // Chama uma funcao exportada pelo nome com argumentos.
    uint64_t call(const std::string& name, uint64_t a0=0, uint64_t a1=0,
                  uint64_t a2=0, uint64_t a3=0);

    // Obtem a imagem principal (primeira carregada).
    LoadedImage* main_image() { return images_.empty() ? nullptr : images_[0].get(); }

    // Acesso a memoria do convidado.
    uint8_t* guest_mem() { return guest_base_; }
    uint64_t guest_size() const { return guest_size_; }

    arm64::HostTable& host_table() { return host_table_; }
    arm64::Interpreter& interpreter() { return *interp_; }

    // Converte endereco virtual do convidado para ponteiro real.
    template<typename T = uint8_t>
    T* vptr(uint64_t vaddr) {
        return reinterpret_cast<T*>(guest_base_ + vaddr);
    }

    // Aloca memoria no espaco do convidado (para pilha, heap, etc).
    uint64_t guest_alloc(uint64_t size, uint64_t align = 16);

private:
    std::vector<std::unique_ptr<LoadedImage>> images_;
    std::unordered_map<std::string, arm64::HostFn> host_symbols_;
    std::unordered_map<std::string, uint64_t> global_cache_;

    arm64::HostTable host_table_;
    std::unique_ptr<arm64::Interpreter> interp_;

    // Regiao unica de memoria do convidado (enderecamento flat).
    uint8_t* guest_base_ = nullptr;
    uint64_t guest_size_ = 0;
    uint64_t guest_cursor_ = 0;
    uint64_t host_cursor_ = 0; // enderecos para funcoes do host

    bool ensure_guest_memory(uint64_t size);
    uint64_t allocate_region(uint64_t size, uint64_t align);
    bool map_image(LoadedImage& img);
    bool resolve_all();
    uint64_t resolve_in(const std::string& name);
};

} // namespace bully::elf
