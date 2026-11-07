// runtime/arm64/interpreter.cpp
// Interpretador ARM64 (AArch64).
#include "interpreter.hpp"
#include <cmath>
#include <cstring>
#include <cassert>

namespace bully::arm64 {

bool Interpreter::trace_enabled = false;

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

uint64_t Interpreter::shift_reg(uint64_t val, uint8_t shift, uint8_t amount, bool is64) {
    if (!is64) val &= 0xffffffff;
    switch (shift) {
        case 0: return val << amount;                    // LSL
        case 1: return is64 ? val >> amount : (uint64_t)((uint32_t)val >> amount); // LSR
        case 2: return is64 ? (int64_t)val >> amount : (uint64_t)((int32_t)val >> amount); // ASR
        case 3: { // ROR
            uint64_t m = is64 ? 63 : 31;
            amount &= m;
            if (!amount) return val;
            return (val >> amount) | (val << ((is64 ? 64 : 32) - amount));
        }
    }
    return val;
}

void Interpreter::set_nzcv_32(uint32_t result, uint64_t a, uint64_t b, bool is_sub) {
    bool n = (result >> 31) & 1;
    bool z = (result == 0);
    bool c, v;
    if (is_sub) {
        c = (uint32_t)a >= (uint32_t)b;
        v = ((int32_t)a - (int32_t)b < 0) != ((int32_t)a < (int32_t)b);
    } else {
        uint64_t r = (uint32_t)a + (uint32_t)b;
        c = r > 0xffffffff;
        v = (((int32_t)a + (int32_t)b) >> 31) != ((int32_t)a >> 31) &&
            (((int32_t)a + (int32_t)b) >> 31) != ((int32_t)b >> 31);
        // simplificado
        v = (((int32_t)a ^ (int32_t)result) & ((int32_t)b ^ (int32_t)result)) < 0;
    }
    cpu_.set_nzcv(n, z, c, v);
}

void Interpreter::set_nzcv_64(uint64_t result, uint64_t a, uint64_t b, bool is_sub) {
    bool n = (result >> 63) & 1;
    bool z = (result == 0);
    bool c, v;
    if (is_sub) {
        c = a >= b;
        v = ((a ^ b) & (a ^ result)) >> 63;
    } else {
        __uint128_t r = (__uint128_t)a + b;
        c = r > ~0ULL;
        v = ((~(a ^ b)) & (a ^ result)) >> 63;
    }
    cpu_.set_nzcv(n, z, c, v);
}

// ---------------------------------------------------------------------------
// Chamada de funcao: se for host, chama; senao, pula.
// ---------------------------------------------------------------------------
void Interpreter::call_host_or_jump(uint64_t target) {
    if (host_.has(target)) {
        cpu_.pc = target; // marcar para log
        host_.call(target, cpu_);
        // Retorno: PC ja foi salvo em LR antes da chamada.
        cpu_.pc = cpu_.x[30];
        // Se LR for 0, halt.
        if (cpu_.pc == 0) cpu_.halt = true;
    } else {
        cpu_.pc = target;
    }
}

// ---------------------------------------------------------------------------
// Data Processing - Immediate
// ---------------------------------------------------------------------------
void Interpreter::exec_data_proc_imm(uint32_t insn) {
    uint8_t op0 = (insn >> 23) & 0x3; // bits 24:23
    uint8_t op = (insn >> 30) & 0x1;  // bit 30
    bool sf = (insn >> 31) & 1;
    uint8_t rd = insn & 0x1f;
    uint8_t rn = (insn >> 5) & 0x1f;

    switch (op0) {
        case 0: { // PC-rel addressing
            uint8_t opcode = (insn >> 29) & 0x1; // bit 29
            int64_t imm = ((int64_t)(insn & 0x1fffff) << 43) >> 43; // 21-bit signed
            if (opcode == 0) {
                // ADR
                uint64_t result = cpu_.pc + imm;
                cpu_.set_rx(rd, result);
            } else {
                // ADRP
                uint64_t result = (cpu_.pc & ~0xfffULL) + ((uint64_t)imm << 12);
                cpu_.set_rx(rd, result);
            }
            cpu_.pc += 4;
            return;
        }
        case 1: { // Add/sub (immediate)
            uint8_t shift = (insn >> 22) & 0x3;
            uint16_t imm12 = (insn >> 10) & 0xfff;
            uint8_t sh = (insn >> 22) & 0x3;
            uint64_t imm = imm12;
            if (shift == 1) imm <<= 12;
            bool setflags = (insn >> 29) & 1; // bit 29: S
            uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
            uint64_t result;
            if (op == 0) { // ADD
                result = a + imm;
            } else { // SUB
                result = a - imm;
            }
            if (sf) {
                if (setflags) set_nzcv_64((uint64_t)result, a, imm, op);
                cpu_.set_rx(rd, (uint64_t)result);
            } else {
                if (setflags) set_nzcv_32((uint32_t)result, a, imm, op);
                cpu_.set_rw(rd, (uint32_t)result);
            }
            cpu_.pc += 4;
            return;
        }
        case 2: { // Logical (immediate) / Move wide
            uint8_t opc = (insn >> 29) & 0x3; // bits 30:29
            uint16_t imm16 = (insn >> 5) & 0xffff;
            uint8_t hw = (insn >> 21) & 0x3;

            if ((insn & 0x80000000) == 0) {
                // bit 31 = 0: move wide (MOVN, MOVZ, MOVK)
                uint64_t val = (uint64_t)imm16 << (hw * 16);
                switch (opc) {
                    case 0: // MOVN
                        val = ~val;
                        if (!sf) val &= 0xffffffff;
                        cpu_.set_rx(rd, val);
                        break;
                    case 2: // MOVZ
                        if (!sf) val &= 0xffffffff;
                        cpu_.set_rx(rd, val);
                        break;
                    case 3: // MOVK
                        val = cpu_.rx(rd);
                        uint64_t mask = (uint64_t)0xffff << (hw * 16);
                        val = (val & ~mask) | ((uint64_t)imm16 << (hw * 16));
                        if (!sf) val &= 0xffffffff;
                        cpu_.set_rx(rd, val);
                        break;
                }
            } else {
                // bit 31 = 1: logical immediate (AND, ORR, EOR, ANDS)
                uint8_t N = (insn >> 22) & 1;
                uint8_t immr = (insn >> 16) & 0x3f;
                uint8_t imms = (insn >> 10) & 0x3f;
                uint8_t rn2 = (insn >> 5) & 0x1f;
                // Decode logical immediate.
                uint64_t imm;
                int len = 0;
                uint32_t n = N, immr_v = immr, imms_v = imms;
                // Algoritmo de decode (ARM manual).
                uint64_t mask = 0;
                {
                    int level = 0;
                    uint32_t s = imms_v;
                    uint32_t r = immr_v;
                    uint32_t mask_v = (1 << (sf ? 6 : 5)) - 1;
                    if (sf) {
                        s = (s & 0x3f) | (n << 6);
                        if ((s & mask_v) == mask_v) {
                            // ALL_ONES / ALL_ZEROS handled below
                            cpu_.pc += 4;
                            return; // nao suportado
                        }
                    }
                    // Decode: encontrar tamanho
                    int esize = 0;
                    {
                        uint32_t tmp = sf ? (s | (1 << 6)) : s;
                        uint32_t tmask = 1 << (sf ? 6 : 5);
                        while ((tmp & tmask) == 0 && tmask > 0) {
                            esize++;
                            tmask >>= 1;
                        }
                    }
                    if (esize == 0) { cpu_.pc += 4; return; } // ALL_ONES/ZEROS
                    int d = 1 << esize;
                    if (d > (sf ? 64 : 32)) { cpu_.pc += 4; return; }
                    uint64_t welem = (1ULL << (s % d + 1)) - 1;
                    uint64_t telem = welem;
                    // rotate right by r % d
                    int rr = r % d;
                    if (rr) {
                        telem = ((welem >> rr) | (welem << (d - rr))) & ((1ULL << d) - 1);
                    }
                    // replicar
                    imm = telem;
                    for (int i = d; i < (sf ? 64 : 32); i += d) {
                        imm |= telem << i;
                    }
                    if (!sf) imm &= 0xffffffff;
                }
                uint64_t a = sf ? cpu_.rx(rn2) : cpu_.rw(rn2);
                uint64_t result;
                switch (opc) {
                    case 0: result = a & imm; break;        // AND
                    case 1: result = a | imm; break;        // ORR
                    case 2: result = a ^ imm; break;        // EOR
                    case 3: result = a & imm; break;        // ANDS
                }
                if (opc == 3) {
                    if (sf) set_nzcv_64(result, a, imm, false);
                    else set_nzcv_32((uint32_t)result, a, imm, false);
                }
                if (sf) cpu_.set_rx(rd, result);
                else cpu_.set_rw(rd, (uint32_t)result);
            }
            cpu_.pc += 4;
            return;
        }
        case 3: { // Bitfield / Extract
            uint8_t opc = (insn >> 29) & 0x3;
            uint8_t n = (insn >> 22) & 1;
            uint8_t immr = (insn >> 16) & 0x3f;
            uint8_t imms = (insn >> 10) & 0x3f;
            uint8_t rn2 = (insn >> 5) & 0x1f;
            uint8_t rm = (insn >> 16) & 0x1f;

            if (n == 1) {
                // EXTR: Rd = Rn[(Rm*8+31):(Rm*8)] ... extract
                // Simplificado: EXTR Xd, Xn, Xm, #lsb
                int lsb = immr;
                uint64_t a = sf ? cpu_.rx(rn2) : cpu_.rw(rn2);
                uint64_t b = sf ? cpu_.rx(rm) : cpu_.rw(rm);
                uint64_t result;
                int bits = sf ? 64 : 32;
                if (lsb == 0) result = a;
                else {
                    __uint128_t cat = ((__uint128_t)b << bits) | a;
                    result = (uint64_t)(cat >> lsb);
                    if (!sf) result &= 0xffffffff;
                }
                cpu_.set_rx(rd, result);
            } else {
                // Bitfield: SBFM (opc=0), BFM (opc=1), UBFM (opc=2)
                int immr_v = immr, imms_v = imms;
                int bits = sf ? 64 : 32;
                uint64_t src = sf ? cpu_.rx(rn2) : cpu_.rw(rn2);
                if (!sf) src &= 0xffffffff;

                uint64_t result;
                int r = immr_v, s = imms_v;
                // ALIAS decode
                bool sign_extend = (opc == 0); // SBFM
                bool insert = (opc == 1);     // BFM

                // Compute mask and positions
                int length = 0;
                if (s >= r) length = s - r + 1;
                else length = bits - r + s + 1;

                uint64_t wmask = (length >= bits) ? ~0ULL : ((1ULL << length) - 1);
                uint64_t t = src;
                // rotate right by r
                if (r) {
                    t = (t >> r) | (t << (bits - r));
                }
                t &= wmask;

                if (sign_extend) {
                    // sign-extend bit (length-1)
                    if (length < bits) {
                        uint64_t sign_bit = 1ULL << (length - 1);
                        if (t & sign_bit) t |= ~((1ULL << length) - 1);
                        else t &= (1ULL << length) - 1;
                    }
                } else if (insert) {
                    uint64_t dst = sf ? cpu_.rx(rd) : cpu_.rw(rd);
                    if (!sf) dst &= 0xffffffff;
                    // insert t into dst at position r
                    uint64_t mask = wmask;
                    if (s >= r) {
                        mask = wmask << r;
                        t = t << r;
                    } else {
                        // wraps around
                        mask = (wmask >> (bits - r));
                        t = (t >> (bits - r));
                    }
                    result = (dst & ~mask) | (t & mask);
                    if (!sf) result &= 0xffffffff;
                    cpu_.set_rx(rd, result);
                    cpu_.pc += 4;
                    return;
                }
                if (!sf) t &= 0xffffffff;
                cpu_.set_rx(rd, t);
            }
            cpu_.pc += 4;
            return;
        }
    }
    cpu_.pc += 4;
}

// ---------------------------------------------------------------------------
// Branches / System
// ---------------------------------------------------------------------------
void Interpreter::exec_branch(uint32_t insn) {
    uint8_t op0 = (insn >> 31) & 1;
    uint8_t op1 = (insn >> 29) & 0x3;

    if (op0 == 0 && op1 == 0) {
        // Conditional branch (B.cond) ou compare-and-branch ou test-and-branch
        uint8_t op2 = (insn >> 24) & 0x1f;
        if (op2 == 0x01 || op2 == 0x03 || op2 == 0x05 || op2 == 0x07 ||
            op2 == 0x09 || op2 == 0x0b || op2 == 0x0d || op2 == 0x0f) {
            // Compare and branch (CBZ: op2 bit0=0, CBNZ: bit0=1) — actually bit 24
            bool cbnz = (insn >> 24) & 1;
            uint8_t rt = insn & 0x1f;
            int64_t imm19 = ((int64_t)(insn & 0x7ffff) << 40) >> 40;
            uint64_t val = ((insn >> 31) & 1) ? cpu_.rx(rt) : cpu_.rw(rt);
            if ((cbnz && val != 0) || (!cbnz && val == 0)) {
                cpu_.pc = cpu_.pc + (imm19 << 2);
            } else {
                cpu_.pc += 4;
            }
            return;
        } else if (op2 >= 0x10) {
            // Test and branch (TBZ: bit 24=0, TBNZ: bit 24=1)
            bool tbnz = (insn >> 24) & 1;
            uint8_t rt = insn & 0x1f;
            uint8_t b5 = (insn >> 31) & 1;
            uint8_t b40 = (insn >> 19) & 0x1f;
            int bit = (b5 << 5) | b40;
            int64_t imm14 = ((int64_t)(insn & 0x3fff) << 45) >> 45;
            uint64_t val = cpu_.rx(rt);
            if (((val >> bit) & 1) == tbnz) {
                cpu_.pc = cpu_.pc + (imm14 << 2);
            } else {
                cpu_.pc += 4;
            }
            return;
        } else {
            // B.cond
            uint8_t cond = insn & 0xf;
            int64_t imm19 = ((int64_t)((insn >> 5) & 0x7ffff) << 40) >> 40;
            bool taken = false;
            switch (cond) {
                case 0: taken = cpu_.z(); break;            // EQ
                case 1: taken = !cpu_.z(); break;           // NE
                case 2: taken = cpu_.c(); break;            // CS
                case 3: taken = !cpu_.c(); break;           // CC
                case 4: taken = cpu_.n(); break;            // MI
                case 5: taken = !cpu_.n(); break;           // PL
                case 6: taken = cpu_.v(); break;            // VS
                case 7: taken = !cpu_.v(); break;           // VC
                case 8: taken = cpu_.c() && !cpu_.z(); break;   // HI
                case 9: taken = !cpu_.c() || cpu_.z(); break;    // LS
                case 10: taken = cpu_.n() == cpu_.v(); break;    // GE
                case 11: taken = cpu_.n() != cpu_.v(); break;    // LT
                case 12: taken = !cpu_.z() && (cpu_.n() == cpu_.v()); break; // GT
                case 13: taken = cpu_.z() || (cpu_.n() != cpu_.v()); break;  // LE
                case 14: taken = true; break;               // AL
                case 15: taken = false; break;              // NV
            }
            if (taken) cpu_.pc = cpu_.pc + (imm19 << 2);
            else cpu_.pc += 4;
            return;
        }
    }

    if (op0 == 0) {
        // op1 = 1: Unconditional branch immediate (B/BL)
        if (op1 == 1) {
            bool is_bl = (insn >> 31) & 1;
            int64_t imm26 = ((int64_t)(insn & 0x3ffffff) << 38) >> 38;
            if (is_bl) cpu_.x[30] = cpu_.pc + 4;
            cpu_.pc = cpu_.pc + (imm26 << 2);
            return;
        }
        // op1 = 2 or 3: Unconditional branch register (BR, BLR, RET)
        if (op1 == 2 || op1 == 3) {
            uint8_t opc = (insn >> 21) & 0x3;
            uint8_t rn = (insn >> 5) & 0x1f;
            uint64_t target = cpu_.rx(rn);
            if (opc == 1) { // BLR
                cpu_.x[30] = cpu_.pc + 4;
                call_host_or_jump(target);
            } else if (opc == 0) { // BR
                call_host_or_jump(target);
            } else { // RET (opc=2)
                if (cpu_.x[30] == 0) { cpu_.halt = true; return; }
                cpu_.pc = cpu_.x[30];
            }
            return;
        }
    }

    // op0 = 1: System / Exception generation / Load literal
    if (op0 == 1) {
        uint8_t op1_hi = (insn >> 29) & 0x7;
        if (op1_hi == 3) {
            // LDR (literal)
            uint8_t opc = (insn >> 30) & 0x3;
            uint8_t rt = insn & 0x1f;
            int64_t imm19 = ((int64_t)((insn >> 5) & 0x7ffff) << 40) >> 40;
            uint64_t addr = cpu_.pc + (imm19 << 2);
            bool is64 = opc & 1;
            bool is_signed = (opc >> 1) & 1;
            if (is64) {
                cpu_.set_rx(rt, mem_.read<uint64_t>(addr));
            } else {
                uint32_t v = mem_.read<uint32_t>(addr);
                if (is_signed) cpu_.set_rx(rt, (int64_t)(int32_t)v);
                else cpu_.set_rw(rt, v);
            }
            cpu_.pc += 4;
            return;
        }
        // System instructions: NOP, SVC, etc.
        // Simplificado: NOP e SVC
        if ((insn & 0xffe0001f) == 0xd503201f) { // NOP
            cpu_.pc += 4;
            return;
        }
        if ((insn & 0xffe0001f) == 0xd4000001) { // SVC #0
            // Syscall - nao suportado, halt
            cpu_.halt = true;
            return;
        }
        // Outras instrucoes de sistema: MRS, MSR - ignorar
        cpu_.pc += 4;
        return;
    }

    cpu_.pc += 4;
}

// ---------------------------------------------------------------------------
// Load / Store
// ---------------------------------------------------------------------------
void Interpreter::exec_load_store(uint32_t insn) {
    uint8_t opc = (insn >> 30) & 0x3;
    uint8_t V = (insn >> 26) & 1;
    uint8_t size = (insn >> 30) & 0x3; // for some
    uint8_t op0 = (insn >> 28) & 0x3;

    // Decodificar grupo principal por bits [25:24] e [27]
    uint8_t group = (insn >> 27) & 0x1f; // bits 31:27

    // Load/store register (immediate) — bits [24:23] = 01 (unsigned), 11 (pair)
    uint8_t ls_mode = (insn >> 23) & 0x3;

    if (((insn >> 28) & 0x3) == 0x2) {
        // Load/store pair (LDP/STP)
        uint8_t opc_pair = (insn >> 30) & 0x3;
        uint8_t V_pair = (insn >> 26) & 1;
        uint8_t size_pair = (insn >> 30) & 0x3; // not exactly
        bool is_load = (insn >> 22) & 1;
        uint8_t rt = insn & 0x1f;
        uint8_t rt2 = (insn >> 10) & 0x1f;
        uint8_t rn = (insn >> 5) & 0x1f;
        int32_t imm7 = (int32_t)((insn >> 15) & 0x7f);
        imm7 = (imm7 << 25) >> 25; // sign extend 7 bits
        uint8_t mode = (insn >> 23) & 0x3;
        uint8_t sz = (insn >> 30) & 0x3;
        int scale = (sz == 3) ? 3 : ((sz == 2) ? 2 : ((sz == 1) ? 1 : 0));
        int64_t offset = (int64_t)imm7 << scale;
        uint64_t base = cpu_.rx(rn);
        uint64_t addr1 = base + offset;
        uint64_t addr2 = base + offset + (1 << scale);

        if (!V_pair) {
            if (sz == 3) { // 64-bit
                if (is_load) {
                    cpu_.set_rx(rt, mem_.read<uint64_t>(addr1));
                    cpu_.set_rx(rt2, mem_.read<uint64_t>(addr2));
                } else {
                    mem_.write<uint64_t>(addr1, cpu_.rx(rt));
                    mem_.write<uint64_t>(addr2, cpu_.rx(rt2));
                }
            } else { // 32-bit
                if (is_load) {
                    cpu_.set_rw(rt, mem_.read<uint32_t>(addr1));
                    cpu_.set_rw(rt2, mem_.read<uint32_t>(addr2));
                } else {
                    mem_.write<uint32_t>(addr1, cpu_.rw(rt));
                    mem_.write<uint32_t>(addr2, cpu_.rw(rt2));
                }
            }
        }
        // Update base if pre/post-index
        if (mode == 0x1) { // post-index
            cpu_.set_rx(rn, base + offset);
        } else if (mode == 0x3) { // pre-index
            cpu_.set_rx(rn, addr1);
        }
        cpu_.pc += 4;
        return;
    }

    // Load/store register (unsigned immediate, register offset, pre/post-index)
    uint8_t rt = insn & 0x1f;
    uint8_t rn = (insn >> 5) & 0x1f;
    uint8_t opc_ls = (insn >> 30) & 0x3;
    bool is_load = opc_ls & 1;
    uint8_t sz_ls = (insn >> 30) & 0x3;
    uint8_t V_ls = (insn >> 26) & 1;

    // Determine size: bits [31:30] = opc; for LDR/STR size is encoded differently
    // Actually for LDR/STR: opc[1]=size(64), opc[0]=load
    // For LDRB/LDRH etc: size in bits [31:30]? No.
    // Let me decode by the full pattern.

    bool is_pair = ((insn >> 28) & 0x3) == 0x2;
    if (is_pair) { cpu_.pc += 4; return; } // handled above

    uint8_t ls_opc = (insn >> 22) & 0x3;
    uint8_t ls_V = (insn >> 26) & 1;
    uint8_t ls_size = (insn >> 30) & 0x3;

    // Decode size field: bits [31:30]
    // For regular LDR/STR: size=3 -> 64bit, size=2 -> 32bit, size=1 -> 16bit, size=0 -> 8bit
    // But opc also encodes load/store. Let me use bits [31:30] as the "size+load" field.
    // Actually: opc = bits[31:30]
    //   opc=00: STR (size from... no)
    // This is getting complex. Let me use a cleaner decode.

    uint32_t top4 = (insn >> 28) & 0xf;
    // top4: 0x0,0x1,0x8,0x9 = load/store
    if (top4 == 0x8 || top4 == 0x9 || top4 == 0x0 || top4 == 0x1) {
        // Single register
        uint8_t size_l = (insn >> 30) & 0x3;
        uint8_t opc_l = (insn >> 22) & 0x3;
        bool load = (insn >> 22) & 1;

        // Determine access size
        int access_size;
        bool sign_extend = false;
        if (size_l == 0) { access_size = 1; } // byte
        else if (size_l == 1) { access_size = 2; } // half
        else if (size_l == 2) { access_size = 4; } // word
        else { access_size = 8; } // double

        // For size 0/1: opc[1] determines sign extend
        if (size_l <= 1) {
            sign_extend = opc_l & 2;
        }

        // Addressing mode
        uint8_t addr_mode = (insn >> 24) & 0x3;
        uint64_t base = cpu_.rx(rn);
        uint64_t addr;
        uint64_t post_val = base;
        bool writeback = false;

        if (addr_mode == 0x2) {
            // Unsigned offset
            uint64_t imm12 = (insn >> 10) & 0xfff;
            int scale = (size_l == 3) ? 3 : ((size_l == 2) ? 2 : ((size_l == 1) ? 1 : 0));
            addr = base + (imm12 << scale);
        } else if (addr_mode == 0x1) {
            // Post-index
            int32_t imm9 = (int32_t)((insn >> 12) & 0x1ff);
            imm9 = (imm9 << 23) >> 23;
            addr = base;
            post_val = base + imm9;
            writeback = true;
        } else if (addr_mode == 0x3) {
            // Pre-index
            int32_t imm9 = (int32_t)((insn >> 12) & 0x1ff);
            imm9 = (imm9 << 23) >> 23;
            addr = base + imm9;
            post_val = addr;
            writeback = true;
        } else {
            // Register offset
            uint8_t rm = (insn >> 16) & 0x1f;
            uint8_t option = (insn >> 13) & 0x7;
            uint64_t offset = cpu_.rx(rm);
            // SXTW/UXTW etc - simplificado
            if (option == 1) offset = (int64_t)(int32_t)offset; // SXTW
            addr = base + offset;
        }

        if (!V_ls) {
            // Integer register
            if (load) {
                switch (access_size) {
                    case 1: {
                        uint8_t v = mem_.read<uint8_t>(addr);
                        if (sign_extend) cpu_.set_rx(rt, (int64_t)(int8_t)v);
                        else cpu_.set_rw(rt, v);
                        break;
                    }
                    case 2: {
                        uint16_t v = mem_.read<uint16_t>(addr);
                        if (sign_extend) cpu_.set_rx(rt, (int64_t)(int16_t)v);
                        else cpu_.set_rw(rt, v);
                        break;
                    }
                    case 4: {
                        uint32_t v = mem_.read<uint32_t>(addr);
                        cpu_.set_rw(rt, v);
                        break;
                    }
                    case 8: {
                        uint64_t v = mem_.read<uint64_t>(addr);
                        cpu_.set_rx(rt, v);
                        break;
                    }
                }
            } else {
                switch (access_size) {
                    case 1: mem_.write<uint8_t>(addr, (uint8_t)cpu_.rw(rt)); break;
                    case 2: mem_.write<uint16_t>(addr, (uint16_t)cpu_.rw(rt)); break;
                    case 4: mem_.write<uint32_t>(addr, cpu_.rw(rt)); break;
                    case 8: mem_.write<uint64_t>(addr, cpu_.rx(rt)); break;
                }
            }
        } else {
            // FP/SIMD register
            if (load) {
                switch (access_size) {
                    case 4: cpu_.set_f32(rt, mem_.read<float>(addr)); break;
                    case 8: cpu_.set_f64(rt, mem_.read<double>(addr)); break;
                }
            } else {
                switch (access_size) {
                    case 4: mem_.write<float>(addr, cpu_.f32(rt)); break;
                    case 8: mem_.write<double>(addr, cpu_.f64(rt)); break;
                }
            }
        }

        if (writeback) cpu_.set_rx(rn, post_val);
        cpu_.pc += 4;
        return;
    }

    cpu_.pc += 4;
}

// ---------------------------------------------------------------------------
// Data Processing - Register
// ---------------------------------------------------------------------------
void Interpreter::exec_data_proc_reg(uint32_t insn) {
    uint8_t sf = (insn >> 31) & 1;
    uint8_t op0 = (insn >> 30) & 0x1;
    uint8_t op1 = (insn >> 28) & 0x3;
    uint8_t op2 = (insn >> 21) & 0x7;

    uint8_t rd = insn & 0x1f;
    uint8_t rn = (insn >> 5) & 0x1f;
    uint8_t rm = (insn >> 16) & 0x1f;

    // MADD / MSUB / SMULH / UMULH
    if (op0 == 0 && op1 == 2 && (op2 & 0x6) == 0) {
        uint8_t opr = (insn >> 15) & 0x1;
        uint8_t op_madd = (insn >> 15) & 0x1;
        uint8_t ra = (insn >> 10) & 0x1f;
        uint64_t a = sf ? cpu_.rx(rm) : cpu_.rw(rm);
        uint64_t b = sf ? cpu_.rx(rn) : cpu_.rw(rn);
        uint64_t c = sf ? cpu_.rx(ra) : cpu_.rw(ra);
        uint64_t result;
        if (sf) {
            if (op2 == 0) {
                // MADD/MSUB
                result = op_madd ? (a * b + c) : (c - a * b);
            } else if (op2 == 1) {
                // SMULH
                result = (uint64_t)((__int128)(int64_t)a * (int64_t)b >> 64);
            } else {
                // UMULH
                result = (uint64_t)((__uint128_t)a * b >> 64);
            }
        } else {
            if (op2 == 0) {
                uint32_t r = op_madd ? ((uint32_t)a * (uint32_t)b + (uint32_t)c) : ((uint32_t)c - (uint32_t)a * (uint32_t)b);
                cpu_.set_rw(rd, r);
                cpu_.pc += 4;
                return;
            } else if (op2 == 1) {
                result = (uint64_t)((int32_t)a * (int32_t)b) >> 32;
            } else {
                result = ((uint64_t)(uint32_t)a * (uint32_t)b) >> 32;
            }
        }
        cpu_.set_rx(rd, result);
        cpu_.pc += 4;
        return;
    }

    // Add/sub (shifted register) — op0=0, op1=0
    if (op0 == 0 && op1 == 0) {
        uint8_t shift = (insn >> 22) & 0x3;
        uint64_t imm6 = (insn >> 10) & 0x3f;
        uint8_t S = (insn >> 29) & 1;
        uint8_t opcode = (insn >> 30) & 0x1; // bit 31:sf, bit30:op, bit29:S
        bool is_sub = (insn >> 30) & 1;
        uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
        uint64_t b = shift_reg(sf ? cpu_.rx(rm) : cpu_.rw(rm), shift, imm6, sf);
        uint64_t result = is_sub ? (a - b) : (a + b);
        if (S) {
            if (sf) set_nzcv_64((uint64_t)result, a, b, is_sub);
            else set_nzcv_32((uint32_t)result, a, b, is_sub);
        }
        if (sf) cpu_.set_rx(rd, (uint64_t)result);
        else cpu_.set_rw(rd, (uint32_t)result);
        cpu_.pc += 4;
        return;
    }

    // Add/sub (extend) — op0=0, op1=0, bit 21 set
    // (handled partially by shifted above; extend mode differs - bit 21=1)
    if (op0 == 0 && op1 == 0 && ((insn >> 21) & 1)) {
        uint8_t option = (insn >> 13) & 0x7;
        uint8_t imm3 = (insn >> 10) & 0x7;
        bool is_sub = (insn >> 30) & 1;
        uint8_t S = (insn >> 29) & 1;
        uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
        uint64_t b = cpu_.rx(rm);
        // extend
        switch (option) {
            case 0: b = (uint8_t)b; break;       // UXTB
            case 1: b = (uint16_t)b; break;      // UXTH
            case 2: b = (uint32_t)b; break;      // UXTW
            case 3: b = (uint64_t)b; break;      // UXTX
            case 4: b = (int64_t)(int8_t)b; break;  // SXTB
            case 5: b = (int64_t)(int16_t)b; break; // SXTH
            case 6: b = (int64_t)(int32_t)b; break; // SXTW
            case 7: b = (int64_t)b; break;       // SXTX
        }
        if (!sf) b &= 0xffffffff;
        b <<= imm3;
        uint64_t result = is_sub ? (a - b) : (a + b);
        if (S) {
            if (sf) set_nzcv_64((uint64_t)result, a, b, is_sub);
            else set_nzcv_32((uint32_t)result, a, b, is_sub);
        }
        if (sf) cpu_.set_rx(rd, (uint64_t)result);
        else cpu_.set_rw(rd, (uint32_t)result);
        cpu_.pc += 4;
        return;
    }

    // Logical (shifted register) — op0=1, op1=0
    if (op0 == 1 && op1 == 0) {
        uint8_t shift = (insn >> 22) & 0x3;
        uint64_t imm6 = (insn >> 10) & 0x3f;
        uint8_t opc = (insn >> 29) & 0x3;
        uint8_t N = (insn >> 21) & 1;
        uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
        uint64_t b = shift_reg(sf ? cpu_.rx(rm) : cpu_.rw(rm), shift, imm6, sf);
        if (N) b = ~b;
        uint64_t result;
        switch (opc) {
            case 0: result = a & b; break;        // AND
            case 1: result = a | b; break;        // ORR (BIC when N)
            case 2: result = a ^ b; break;        // EOR
            case 3: result = a & b; break;        // ANDS (BICS when N)
        }
        if (opc == 3) {
            if (sf) set_nzcv_64(result, a, b, false);
            else set_nzcv_32((uint32_t)result, a, b, false);
        }
        if (sf) cpu_.set_rx(rd, result);
        else cpu_.set_rw(rd, (uint32_t)result);
        cpu_.pc += 4;
        return;
    }

    // Shift (register) — op0=1, op1=1, bit 21=0
    if (op0 == 1 && op1 == 1 && !((insn >> 21) & 1)) {
        uint8_t opcode = (insn >> 30) & 0x3;
        uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
        uint64_t b = cpu_.rx(rm);
        if (!sf) b &= 0x3f; else b &= 0x3f;
        uint64_t result;
        switch (opcode) {
            case 0: result = a << (b & 0x3f); break;        // LSLV
            case 1: result = sf ? a >> (b & 0x3f) : (uint32_t)a >> (b & 0x3f); break; // LSRV
            case 2: result = sf ? (int64_t)a >> (b & 0x3f) : (int32_t)a >> (b & 0x3f); break; // ASRV
            case 3: { // RORV
                int bits = sf ? 64 : 32;
                b &= bits - 1;
                result = b ? ((a >> b) | (a << (bits - b))) : a;
                break;
            }
        }
        if (sf) cpu_.set_rx(rd, result);
        else cpu_.set_rw(rd, (uint32_t)result);
        cpu_.pc += 4;
        return;
    }

    // CSEL / CSINC / CSINV / CSNEG — op0=1, op1=1, bit 21=1, bits 11:10=0
    if (op0 == 1 && op1 == 1 && ((insn >> 21) & 1) && ((insn >> 10) & 0x3) == 0) {
        uint8_t op_csel = (insn >> 30) & 0x3;
        uint8_t cond = insn & 0xf;
        uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
        uint64_t b = sf ? cpu_.rx(rm) : cpu_.rw(rm);
        bool taken = false;
        switch (cond) {
            case 0: taken = cpu_.z(); break;
            case 1: taken = !cpu_.z(); break;
            case 2: taken = cpu_.c(); break;
            case 3: taken = !cpu_.c(); break;
            case 4: taken = cpu_.n(); break;
            case 5: taken = !cpu_.n(); break;
            case 6: taken = cpu_.v(); break;
            case 7: taken = !cpu_.v(); break;
            case 8: taken = cpu_.c() && !cpu_.z(); break;
            case 9: taken = !cpu_.c() || cpu_.z(); break;
            case 10: taken = cpu_.n() == cpu_.v(); break;
            case 11: taken = cpu_.n() != cpu_.v(); break;
            case 12: taken = !cpu_.z() && (cpu_.n() == cpu_.v()); break;
            case 13: taken = cpu_.z() || (cpu_.n() != cpu_.v()); break;
            case 14: taken = true; break;
            case 15: taken = false; break;
        }
        uint64_t result;
        if (taken) {
            result = a;
            if (op_csel & 1) result = ~result; // CSINV/CSNEG
            if (op_csel & 2) result += 1;       // CSINC/CSNEG
        } else {
            result = b;
            if (op_csel == 2) result += 1; // CSINC
        }
        if (sf) cpu_.set_rx(rd, result);
        else cpu_.set_rw(rd, (uint32_t)result);
        cpu_.pc += 4;
        return;
    }

    // SDIV / UDIV — op0=0, op1=1, bit 21=0, bits 15:10=000010
    if (op0 == 0 && op1 == 1 && !((insn >> 21) & 1)) {
        uint8_t op_div = (insn >> 10) & 0x3f;
        if (op_div == 2) { // UDIV
            uint64_t a = sf ? cpu_.rx(rn) : cpu_.rw(rn);
            uint64_t b = sf ? cpu_.rx(rm) : cpu_.rw(rm);
            if (!sf) { a &= 0xffffffff; b &= 0xffffffff; }
            uint64_t result = b ? (a / b) : 0;
            if (sf) cpu_.set_rx(rd, result);
            else cpu_.set_rw(rd, (uint32_t)result);
            cpu_.pc += 4;
            return;
        } else if (op_div == 3) { // SDIV
            int64_t a = sf ? (int64_t)cpu_.rx(rn) : (int32_t)cpu_.rw(rn);
            int64_t b = sf ? (int64_t)cpu_.rx(rm) : (int32_t)cpu_.rw(rm);
            int64_t result = b ? (a / b) : 0;
            if (sf) cpu_.set_rx(rd, (uint64_t)result);
            else cpu_.set_rw(rd, (uint32_t)result);
            cpu_.pc += 4;
            return;
        }
    }

    // Add/sub with carry (ADC/SBC)
    if (op0 == 0 && op1 == 0 && ((insn >> 21) & 1) && ((insn >> 10) & 0x3f) == 0) {
        // handled by extend branch above, but carry variant has imm3=0
        // skip - already handled
    }

    cpu_.pc += 4;
}

// ---------------------------------------------------------------------------
// FP / SIMD
// ---------------------------------------------------------------------------
void Interpreter::exec_simd_fp(uint32_t insn) {
    uint8_t rd = insn & 0x1f;
    uint8_t rn = (insn >> 5) & 0x1f;
    uint8_t rm = (insn >> 16) & 0x1f;

    // FP data processing (1 source): FMOV, FNEG, FABS, FSQRT
    if (((insn >> 24) & 0x3f) == 0x1f) {
        uint8_t opcode = (insn >> 15) & 0x1f;
        uint8_t ftype = (insn >> 22) & 0x3;
        bool is_double = (ftype == 1);
        switch (opcode) {
            case 0: // FMOV (register)
                if (is_double) cpu_.set_f64(rd, cpu_.f64(rn));
                else cpu_.set_f32(rd, cpu_.f32(rn));
                break;
            case 1: // FABS
                if (is_double) cpu_.set_f64(rd, fabs(cpu_.f64(rn)));
                else cpu_.set_f32(rd, fabsf(cpu_.f32(rn)));
                break;
            case 2: // FNEG
                if (is_double) cpu_.set_f64(rd, -cpu_.f64(rn));
                else cpu_.set_f32(rd, -cpu_.f32(rn));
                break;
            case 3: // FSQRT
                if (is_double) cpu_.set_f64(rd, sqrt(cpu_.f64(rn)));
                else cpu_.set_f32(rd, sqrtf(cpu_.f32(rn)));
                break;
            default: break;
        }
        cpu_.pc += 4;
        return;
    }

    // FP data processing (2 source): FADD, FSUB, FMUL, FDIV, FMADD, etc.
    if (((insn >> 24) & 0x1f) == 0x17) {
        uint8_t opcode = (insn >> 14) & 0x7;
        uint8_t ftype = (insn >> 22) & 0x3;
        bool is_double = (ftype == 1);
        uint8_t ra = (insn >> 10) & 0x1f;
        double a = is_double ? cpu_.f64(rn) : (double)cpu_.f32(rn);
        double b = is_double ? cpu_.f64(rm) : (double)cpu_.f32(rm);
        double c = is_double ? cpu_.f64(ra) : (double)cpu_.f32(ra);
        double result = 0;
        switch (opcode) {
            case 0: result = a + b; break;  // FADD
            case 1: result = a - b; break;  // FSUB
            case 2: result = a * b; break;  // FMUL
            case 3: result = a / b; break;  // FDIV
            case 4: result = a * c + b; break; // FMADD
            case 5: result = a * c - b; break; // FMSUB
            case 6: result = -(a * c) + b; break; // FNMADD
            case 7: result = -(a * c) - b; break; // FNMSUB
        }
        if (is_double) cpu_.set_f64(rd, result);
        else cpu_.set_f32(rd, (float)result);
        cpu_.pc += 4;
        return;
    }

    // FP compare: FCMP
    if (((insn >> 24) & 0x3f) == 0x1e && ((insn >> 14) & 0x3) == 0) {
        uint8_t ftype = (insn >> 22) & 0x3;
        bool is_double = (ftype == 1);
        uint8_t op = (insn >> 13) & 0x1;
        double a = is_double ? cpu_.f64(rn) : (double)cpu_.f32(rn);
        double b = is_double ? cpu_.f64(rm) : (double)cpu_.f32(rm);
        if (op) { // FCMP with zero
            b = 0.0;
        }
        bool n = false, z = false, c = false, v = false;
        if (std::isnan(a) || std::isnan(b)) {
            c = true; v = true; // unordered
        } else if (a < b) {
            n = true;
        } else if (a == b) {
            z = true; c = true;
        } else {
            c = true;
        }
        cpu_.set_nzcv(n, z, c, v);
        cpu_.pc += 4;
        return;
    }

    // FP convert: FCVT between single/double, FCVTZS (fixed-point)
    if (((insn >> 24) & 0x3f) == 0x1f && ((insn >> 19) & 0x1f) == 0x1e) {
        uint8_t opcode = (insn >> 15) & 0xf;
        uint8_t ftype = (insn >> 22) & 0x3;
        bool is_double = (ftype == 1);
        switch (opcode) {
            case 0: // FCVT double->single or single->double
                if (is_double) cpu_.set_f32(rd, (float)cpu_.f64(rn));
                else cpu_.set_f64(rd, (double)cpu_.f32(rn));
                break;
            case 7: // FCVTZS (float to int, round toward zero)
                if (is_double) {
                    int64_t v = (int64_t)cpu_.f64(rn);
                    cpu_.set_rx(rd, (uint64_t)v);
                } else {
                    int32_t v = (int32_t)cpu_.f32(rn);
                    cpu_.set_rw(rd, (uint32_t)v);
                }
                break;
            case 11: // SCVTF (int to float)
                if (is_double) cpu_.set_f64(rd, (double)(int64_t)cpu_.rx(rn));
                else cpu_.set_f32(rd, (float)(int32_t)cpu_.rw(rn));
                break;
            default: break;
        }
        cpu_.pc += 4;
        return;
    }

    // FMOV int<->fp
    if (((insn >> 24) & 0x3f) == 0x1e && ((insn >> 20) & 0x3f) == 0) {
        uint8_t ftype = (insn >> 22) & 0x3;
        uint8_t op = (insn >> 15) & 0x1;
        if (op) { // FMOV from int to fp
            if (ftype == 1) {
                uint64_t v = cpu_.rx(rn);
                memcpy(&cpu_.v[rd][0], &v, 8);
            } else {
                uint32_t v = cpu_.rw(rn);
                memcpy(&cpu_.v[rd][0], &v, 4);
            }
        } else { // FMOV from fp to int
            if (ftype == 1) {
                uint64_t v;
                memcpy(&v, &cpu_.v[rn][0], 8);
                cpu_.set_rx(rd, v);
            } else {
                uint32_t v;
                memcpy(&v, &cpu_.v[rn][0], 4);
                cpu_.set_rw(rd, v);
            }
        }
        cpu_.pc += 4;
        return;
    }

    cpu_.pc += 4;
}

// ---------------------------------------------------------------------------
// Dispatch
// ---------------------------------------------------------------------------
void Interpreter::exec(uint32_t insn) {
    uint8_t top = (insn >> 28) & 0xf;
    switch (top) {
        case 0x1: // 0001: data processing immediate
            exec_data_proc_imm(insn);
            return;
        case 0x0: case 0x4: case 0x5: case 0xc: case 0xd: // 0100/0101: branches; 1100/1101: more
            if ((insn >> 26) == 0) { // 00xxxx: branches/sys
                exec_branch(insn);
                return;
            }
            break;
        case 0x2: case 0x3: case 0x6: case 0x7: // 001x/011x: load/store
            exec_load_store(insn);
            return;
        case 0x8: case 0x9: // 100x: data processing register
            exec_data_proc_reg(insn);
            return;
        case 0xa: case 0xb: // 101x: SIMD/FP
            exec_simd_fp(insn);
            return;
        case 0xe: case 0xf: // 111x: SIMD/FP (128-bit)
            exec_simd_fp(insn);
            return;
    }
    // Fallback: branches for 0x0
    if (((insn >> 26) & 0x3) <= 1) {
        exec_branch(insn);
        return;
    }
    cpu_.pc += 4;
}

void Interpreter::step() {
    if (cpu_.halt) return;
    uint32_t insn = fetch();
    cpu_.step_count++;
    exec(insn);
}

uint64_t Interpreter::run(uint64_t entry, uint64_t max_steps) {
    cpu_.pc = entry;
    cpu_.halt = false;
    uint64_t start = cpu_.step_count;
    while (!cpu_.halt && (cpu_.step_count - start) < max_steps) {
        step();
    }
    return cpu_.step_count - start;
}

} // namespace bully::arm64
