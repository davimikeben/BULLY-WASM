// runtime/arm64/interpreter.hpp
// Interpretador ARM64 (AArch64) — executa codigo do convidado na memoria do host.
#pragma once
#include "cpu.hpp"
#include <cstdint>
#include <cstdio>

namespace bully::arm64 {

// Memoria do convidado: ponteiro para a regiao mapeada e tamanho.
struct GuestMemory {
    uint8_t* base;
    uint64_t size;

    template<typename T>
    T read(uint64_t addr) const {
        if (addr + sizeof(T) > size) {
            // Out-of-bounds: retorna 0 (comportamento de convidado bugado).
            return T{};
        }
        T v;
        memcpy(&v, base + addr, sizeof(T));
        return v;
    }

    template<typename T>
    void write(uint64_t addr, T val) {
        if (addr + sizeof(T) > size) return;
        memcpy(base + addr, &val, sizeof(T));
    }
};

class Interpreter {
public:
    Interpreter(GuestMemory mem, HostTable& host)
        : mem_(mem), host_(host) { cpu_.reset(); }

    CPUState& cpu() { return cpu_; }
    const CPUState& cpu() const { return cpu_; }

    // Executa a partir de `entry` ate RET no nivel mais alto ou `max_steps`.
    // Retorna o numero de passos executados.
    uint64_t run(uint64_t entry, uint64_t max_steps = ~0ULL);

    // Um unico passo.
    void step();

private:
    CPUState cpu_;
    GuestMemory mem_;
    HostTable& host_;

    uint32_t fetch() { return mem_.read<uint32_t>(cpu_.pc); }

    // Decodificacao por grupos principais.
    void exec(uint32_t insn);

    void exec_data_proc_imm(uint32_t insn);
    void exec_branch(uint32_t insn);
    void exec_load_store(uint32_t insn);
    void exec_data_proc_reg(uint32_t insn);
    void exec_simd_fp(uint32_t insn);

    // Helpers.
    void set_nzcv_32(uint32_t result, uint64_t a, uint64_t b, bool is_sub);
    void set_nzcv_64(uint64_t result, uint64_t a, uint64_t b, bool is_sub);
    static uint64_t shift_reg(uint64_t val, uint8_t shift, uint8_t amount, bool is64);
    void call_host_or_jump(uint64_t target);

    // Logging (quando ativado).
    static bool trace_enabled;
};

} // namespace bully::arm64
