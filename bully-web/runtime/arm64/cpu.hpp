// runtime/arm64/cpu.hpp
// Estado do processador ARM64 (AArch64) e interface de funcoes do host.
#pragma once
#include <cstdint>
#include <cstring>
#include <functional>
#include <unordered_map>

namespace bully::arm64 {

// Estado completo da CPU AArch64.
struct CPUState {
    // Registradores inteiros X0-X30. X30 = LR (link register).
    uint64_t x[31];
    uint64_t sp;
    uint64_t pc;

    // PSTATE: flags NZCV nos bits 31,30,29,28.
    uint32_t pstate;

    // Registradores FP/SIMD V0-V31 (128 bits cada, acessiveis como Q/D/S/H/B).
    uint8_t v[32][16];

    uint32_t fpsr;
    uint32_t fpcr;

    // Sistema: contador de passos (para limite de execucao).
    uint64_t step_count;

    // Flag de retorno: quando true, o interpretador para.
    bool halt;

    void reset() {
        memset(x, 0, sizeof(x));
        sp = 0;
        pc = 0;
        pstate = 0;
        memset(v, 0, sizeof(v));
        fpsr = 0;
        fpcr = 0;
        step_count = 0;
        halt = false;
    }

    // Acesso aos registradores inteiros (Xn / Wn).
    uint64_t rx(int n) const { return n == 31 ? 0 : x[n]; }
    void set_rx(int n, uint64_t v) { if (n != 31) x[n] = v; }

    uint32_t rw(int n) const { return n == 31 ? 0 : (uint32_t)x[n]; }
    void set_rw(int n, uint32_t v) { if (n != 31) { x[n] = (uint64_t)(int32_t)v; } }

    // Flags NZCV.
    bool n() const { return (pstate >> 31) & 1; }
    bool z() const { return (pstate >> 30) & 1; }
    bool c() const { return (pstate >> 29) & 1; }
    bool v() const { return (pstate >> 28) & 1; }
    void set_nzcv(bool n, bool z, bool c, bool v) {
        pstate &= ~0xf0000000u;
        if (n) pstate |= 0x80000000u;
        if (z) pstate |= 0x40000000u;
        if (c) pstate |= 0x20000000u;
        if (v) pstate |= 0x10000000u;
    }

    // Acesso a registradores FP.
    float f32(int n) const { float f; memcpy(&f, &v[n][0], 4); return f; }
    void set_f32(int n, float f) { memcpy(&v[n][0], &f, 4); }
    double f64(int n) const { double d; memcpy(&d, &v[n][0], 8); return d; }
    void set_f64(int n, double d) { memcpy(&v[n][0], &d, 8); }
};

// Uma funcao fornecida pelo host (shim). Recebe o estado da CPU e retorna o
// valor para X0. A funcao eh responsavel por ler os argumentos de X0-X7 e
// escrever o retorno em X0 (o retorno desta funcao tambem vai para X0).
using HostFn = std::function<uint64_t(CPUState&)>;

// Tabela de funcoes do host indexadas por endereco virtual.
class HostTable {
public:
    // Registra uma funcao do host em um endereco virtual.
    void add(uint64_t addr, HostFn fn) {
        fns_[addr] = std::move(fn);
    }

    bool has(uint64_t addr) const {
        return fns_.find(addr) != fns_.end();
    }

    // Chama uma funcao do host. Retorna false se nao existir.
    bool call(uint64_t addr, CPUState& cpu) const {
        auto it = fns_.find(addr);
        if (it == fns_.end()) return false;
        uint64_t ret = it->second(cpu);
        cpu.x[0] = ret;
        return true;
    }

private:
    std::unordered_map<uint64_t, HostFn> fns_;
};

} // namespace bully::arm64
