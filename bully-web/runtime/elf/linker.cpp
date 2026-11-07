// runtime/elf/linker.cpp
#include "linker.hpp"
#include "relocations.hpp"
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/mman.h>

namespace bully::elf {

// Endereco base para funcoes do host (acima de 4GB do convidado nao cabe em 32-bit,
// mas como usamos 64-bit, colocamos em 0xF0000000).
static constexpr uint64_t HOST_ADDR_BASE = 0xF0000000ULL;

Linker::Linker() {
    // Aloca 4 GB de espaco de enderecos do convidado (reserva, nao comita tudo).
    guest_size_ = 4ULL * 1024 * 1024 * 1024;
    guest_base_ = static_cast<uint8_t*>(mmap(nullptr, guest_size_,
        PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (guest_base_ == MAP_FAILED) {
        fprintf(stderr, "[linker] failed to mmap guest memory (%llu)\n",
                (unsigned long long)guest_size_);
        guest_base_ = nullptr;
    }
    guest_cursor_ = 0x10000; // deixa espaco baixo para NULL
    host_cursor_ = HOST_ADDR_BASE;

    // Cria o interpretador apontando para a memoria do convidado.
    arm64::GuestMemory mem{guest_base_, guest_size_};
    interp_ = std::make_unique<arm64::Interpreter>(mem, host_table_);
}

Linker::~Linker() {
    if (guest_base_) munmap(guest_base_, guest_size_);
}

bool Linker::ensure_guest_memory(uint64_t size) {
    if (!guest_base_) return false;
    if (size > guest_size_) {
        fprintf(stderr, "[linker] needs %llu bytes, only %llu mapped\n",
                (unsigned long long)size, (unsigned long long)guest_size_);
        return false;
    }
    return true;
}

uint64_t Linker::allocate_region(uint64_t size, uint64_t align) {
    if (!align) align = 4096;
    uint64_t addr = (guest_cursor_ + align - 1) & ~(align - 1);
    guest_cursor_ = addr + size;
    if (guest_cursor_ > HOST_ADDR_BASE) {
        fprintf(stderr, "[linker] guest memory exhausted (cursor=%llu)\n",
                (unsigned long long)guest_cursor_);
        return 0;
    }
    return addr;
}

uint64_t Linker::guest_alloc(uint64_t size, uint64_t align) {
    return allocate_region(size, align);
}

bool Linker::map_image(LoadedImage& img) {
    uint64_t load_sz = compute_load_size(img.elf);
    uint64_t base = allocate_region(load_sz, 0x10000);
    if (!base) return false;
    img.base = base;
    img.elf.base_addr = base;

    // Copia segmentos PT_LOAD para a memoria do convidado.
    for (const auto& seg : img.elf.segments) {
        if (seg.type != PT_LOAD) continue;
        uint8_t* dst = guest_base_ + base + seg.vaddr;
        if (seg.filesz) memcpy(dst, seg.data, seg.filesz);
        if (seg.memsz > seg.filesz) memset(dst + seg.filesz, 0, seg.memsz - seg.filesz);
    }
    img.mem = guest_base_ + base;
    return true;
}

uint64_t Linker::resolve_in(const std::string& name) {
    // Host symbols tem prioridade (shims).
    auto hit = host_symbols_.find(name);
    if (hit != host_symbols_.end()) {
        // Retorna o endereco virtual atribuido ao host symbol.
        // Precisamos de um mapa nome->endereco; construimos aqui.
        // (Os enderecos sao alocados em add_host_symbol e armazenados em global_cache_.)
        auto cit = global_cache_.find(name);
        if (cit != global_cache_.end()) return cit->second;
    }
    // Depois procura em todas as imagens.
    for (auto& img : images_) {
        uint64_t addr = img->elf.find_export(name);
        if (addr) return addr;
    }
    return 0;
}

uint64_t Linker::resolve(const std::string& name) {
    auto it = global_cache_.find(name);
    if (it != global_cache_.end()) return it->second;
    uint64_t addr = resolve_in(name);
    global_cache_[name] = addr;
    return addr;
}

static uint64_t resolver_trampoline(void* ctx, const std::string& name) {
    auto* self = static_cast<Linker*>(ctx);
    return self->resolve(name);
}

bool Linker::load(const std::string& path, const std::vector<std::string>& search_paths) {
    if (!guest_base_) return false;

    // 1. Parse
    auto img = std::make_unique<LoadedImage>();
    if (!parse_elf(path, img->elf)) return false;

    // 2. Carregar dependencias primeiro (ordem topologica simples).
    for (const auto& dep : img->elf.dyn.needed) {
        if (dep == "libc.so" || dep == "libm.so" || dep == "libdl.so" ||
            dep == "liblog.so" || dep == "libandroid.so" ||
            dep == "libEGL.so" || dep == "libGLESv3.so" ||
            dep == "libopenal.so" || dep == "libVendor_mpg123.so") {
            continue; // shims
        }
        std::string found;
        for (const auto& sp : search_paths) {
            std::string p = sp + "/" + dep;
            FILE* f = fopen(p.c_str(), "rb");
            if (f) { fclose(f); found = p; break; }
        }
        if (!found.empty()) {
            if (!load(found, search_paths)) {
                fprintf(stderr, "[linker] failed to load dependency %s\n", dep.c_str());
                return false;
            }
        } else {
            fprintf(stderr, "[linker] dependency not found: %s (will use host shim)\n", dep.c_str());
        }
    }

    // 3. Mapear em memoria.
    if (!map_image(*img)) return false;
    images_.push_back(std::move(img));
    return true;
}

bool Linker::resolve_all() {
    SymbolResolver resolver{resolver_trampoline, this};
    for (auto& img : images_) {
        if (!apply_relocations(img->elf, guest_base_, resolver)) {
            fprintf(stderr, "[linker] relocations failed for %s\n", img->elf.path.c_str());
            return false;
        }
    }
    return true;
}

bool Linker::run_init() {
    if (!resolve_all()) return false;

    for (auto& img : images_) {
        if (img->initialized) continue;
        for (uint64_t fn_ptr : img->elf.init_array) {
            if (!fn_ptr) continue;
            // fn_ptr ja contem o endereco virtual final (preenchido por RELATIVE).
            // Executa via interpretador.
            uint64_t steps = interp_->run(fn_ptr, 1000000);
            if (steps >= 1000000) {
                fprintf(stderr, "[linker] init function 0x%lx exceeded step limit\n",
                        (unsigned long)fn_ptr);
            }
        }
        img->initialized = true;
    }
    return true;
}

uint64_t Linker::call(const std::string& name, uint64_t a0, uint64_t a1,
                      uint64_t a2, uint64_t a3) {
    uint64_t addr = resolve(name);
    if (!addr) {
        fprintf(stderr, "[linker] symbol not found: %s\n", name.c_str());
        return 0;
    }
    auto& cpu = interp_->cpu();
    cpu.x[0] = a0;
    cpu.x[1] = a1;
    cpu.x[2] = a2;
    cpu.x[3] = a3;
    cpu.x[30] = 0; // LR=0 -> halt no RET
    interp_->run(addr, 100000000ULL);
    return cpu.x[0];
}

void Linker::add_host_symbol(const std::string& name, arm64::HostFn fn) {
    uint64_t addr = host_cursor_;
    host_cursor_ += 4; // cada funcao do host ocupa 4 bytes virtuais
    host_symbols_[name] = std::move(fn);
    host_table_.add(addr, host_symbols_[name]);
    global_cache_[name] = addr;
}

} // namespace bully::elf
