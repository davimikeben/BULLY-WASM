// runtime/elf/parser.cpp
#include "parser.hpp"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <algorithm>

namespace bully::elf {

static const char* safe_cstr(const uint8_t* strtab, size_t strsz, uint32_t off) {
    if (off >= strsz) return "";
    // garante termino nulo
    for (size_t i = off; i < strsz; i++) {
        if (strtab[i] == 0) return reinterpret_cast<const char*>(strtab + off);
    }
    return "";
}

bool parse_elf(const std::string& path, ElfImage& out) {
    out.path = path;
    std::ifstream f(path, std::ios::binary);
    if (!f) {
        fprintf(stderr, "[elf] cannot open %s\n", path.c_str());
        return false;
    }
    f.seekg(0, std::ios::end);
    size_t sz = f.tellg();
    f.seekg(0, std::ios::beg);
    out.file_data.resize(sz);
    f.read(reinterpret_cast<char*>(out.file_data.data()), sz);
    const uint8_t* data = out.file_data.data();

    if (sz < sizeof(Elf64_Ehdr)) {
        fprintf(stderr, "[elf] file too small\n");
        return false;
    }
    memcpy(&out.header, data, sizeof(Elf64_Ehdr));

    if (memcmp(out.header.e_ident, "\x7f""ELF", 4) != 0) {
        fprintf(stderr, "[elf] not an ELF file\n");
        return false;
    }
    if (out.header.e_ident[4] != 2) { // ELFCLASS64
        fprintf(stderr, "[elf] not ELF64\n");
        return false;
    }
    if (out.header.e_machine != EM_AARCH64) {
        fprintf(stderr, "[elf] not AArch64 (machine=%d)\n", out.header.e_machine);
        return false;
    }

    // ---- Segmentos (PT_LOAD) ----
    const Elf64_Phdr* phdrs = reinterpret_cast<const Elf64_Phdr*>(data + out.header.e_phoff);
    for (uint16_t i = 0; i < out.header.e_phnum; i++) {
        const Elf64_Phdr& ph = phdrs[i];
        Segment seg;
        seg.type   = ph.p_type;
        seg.flags  = ph.p_flags;
        seg.offset = ph.p_offset;
        seg.vaddr  = ph.p_vaddr;
        seg.filesz = ph.p_filesz;
        seg.memsz  = ph.p_memsz;
        seg.align  = ph.p_align;
        seg.data   = data + ph.p_offset;
        out.segments.push_back(seg);
    }

    // ---- Secao dinamica ----
    const Elf64_Dyn* dyn = nullptr;
    size_t dyn_count = 0;
    for (const auto& seg : out.segments) {
        if (seg.type == PT_DYNAMIC) {
            dyn = reinterpret_cast<const Elf64_Dyn*>(data + seg.offset);
            dyn_count = seg.filesz / sizeof(Elf64_Dyn);
            break;
        }
    }
    if (!dyn) {
        fprintf(stderr, "[elf] no PT_DYNAMIC\n");
        return false;
    }

    // Encontrar strtab e symtab offsets (relativos a base)
    uint64_t strtab_off = 0, symtab_off = 0, strsz = 0, syment = 0;
    for (size_t i = 0; i < dyn_count; i++) {
        const Elf64_Dyn& d = dyn[i];
        switch (d.d_tag) {
            case DT_NULL:       i = dyn_count; break;
            case DT_NEEDED:     out.dyn.needed.push_back(""); break; // preenche depois
            case DT_SONAME:     out.dyn.soname = ""; break;
            case DT_STRTAB:     out.dyn.strtab = d.d_un.d_val; strtab_off = d.d_un.d_val; break;
            case DT_SYMTAB:     out.dyn.symtab = d.d_un.d_val; symtab_off = d.d_un.d_val; break;
            case DT_STRSZ:      out.dyn.strsz = d.d_un.d_val; strsz = d.d_un.d_val; break;
            case DT_SYMENT:     out.dyn.syment = d.d_un.d_val; syment = d.d_un.d_val; break;
            case DT_INIT:       out.dyn.init = d.d_un.d_val; break;
            case DT_FINI:       out.dyn.fini = d.d_un.d_val; break;
            case DT_INIT_ARRAY: out.dyn.init_array = d.d_un.d_val; break;
            case DT_INIT_ARRAYSZ: out.dyn.init_array_size = d.d_un.d_val; break;
            case DT_FINI_ARRAY: out.dyn.fini_array = d.d_un.d_val; break;
            case DT_FINI_ARRAYSZ: out.dyn.fini_array_size = d.d_un.d_val; break;
            case DT_RELA:       out.dyn.rela = d.d_un.d_val; break;
            case DT_RELASZ:     out.dyn.relasz = d.d_un.d_val; break;
            case DT_RELAENT:    out.dyn.relaent = d.d_un.d_val; break;
            case DT_JMPREL:     out.dyn.jmprel = d.d_un.d_val; break;
            case DT_PLTREL:     out.dyn.pltrel = d.d_un.d_val; break;
            case DT_PLTGOT:     out.dyn.pltgot = d.d_un.d_val; break;
            case DT_FLAGS:      out.dyn.flags = d.d_un.d_val; break;
            default: break;
        }
    }

    // Resolver strings NEEDED e SONAME com a strtab
    if (strtab_off && strsz) {
        // strtab_off eh um endereco virtual; precisamos mapear para offset no arquivo
        uint64_t strtab_fileoff = 0;
        for (const auto& seg : out.segments) {
            if (seg.type == PT_LOAD && strtab_off >= seg.vaddr && strtab_off < seg.vaddr + seg.filesz) {
                strtab_fileoff = seg.offset + (strtab_off - seg.vaddr);
                break;
            }
        }
        const uint8_t* strtab = data + strtab_fileoff;
        // Re-iterar para preencher strings
        size_t needed_idx = 0;
        out.dyn.needed.clear();
        for (size_t i = 0; i < dyn_count; i++) {
            const Elf64_Dyn& d = dyn[i];
            if (d.d_tag == DT_NULL) break;
            if (d.d_tag == DT_NEEDED) {
                out.dyn.needed.push_back(safe_cstr(strtab, strsz, (uint32_t)d.d_un.d_val));
            } else if (d.d_tag == DT_SONAME) {
                out.dyn.soname = safe_cstr(strtab, strsz, (uint32_t)d.d_un.d_val);
            }
        }
    }

    // ---- Simbolos ----
    if (symtab_off && strtab_off) {
        uint64_t symtab_fileoff = 0, strtab_fileoff = 0;
        for (const auto& seg : out.segments) {
            if (seg.type == PT_LOAD) {
                if (symtab_off >= seg.vaddr && symtab_off < seg.vaddr + seg.filesz)
                    symtab_fileoff = seg.offset + (symtab_off - seg.vaddr);
                if (strtab_off >= seg.vaddr && strtab_off < seg.vaddr + seg.filesz)
                    strtab_fileoff = seg.offset + (strtab_off - seg.vaddr);
            }
        }
        const Elf64_Sym* syms = reinterpret_cast<const Elf64_Sym*>(data + symtab_fileoff);
        const uint8_t* strtab = data + strtab_fileoff;
        size_t sym_count = 0;
        if (syment) {
            uint64_t end = strtab_off; // strtab logo apos symtab tipicamente; estimativa
            for (const auto& seg : out.segments) {
                if (seg.type == PT_LOAD && strtab_off >= seg.vaddr && strtab_off < seg.vaddr + seg.filesz) {
                    end = seg.vaddr + seg.filesz;
                }
            }
            // melhor: usar relacao symtab->strtab. num de simbolos = (strtab - symtab)/syment
            if (strtab_off > symtab_off) {
                sym_count = (strtab_off - symtab_off) / syment;
            }
        }
        for (size_t i = 0; i < sym_count; i++) {
            const Elf64_Sym& s = syms[i];
            Symbol sym;
            sym.name  = safe_cstr(strtab, strsz, s.st_name);
            sym.value = s.st_value;
            sym.size  = s.st_size;
            sym.bind  = ELF64_ST_BIND(s.st_info);
            sym.type  = ELF64_ST_TYPE(s.st_info);
            sym.shndx = s.st_shndx;
            sym.is_import = (s.st_shndx == 0); // SHN_UNDEF
            if (sym.name.empty()) continue;
            if (sym.is_import) out.imports.push_back(sym);
            else out.exports.push_back(sym);
        }
    }

    // ---- Relocations ----
    auto collect_rela = [&](uint64_t rela_vaddr, uint64_t sz, uint64_t ent) {
        if (!rela_vaddr || !sz || !ent) return;
        uint64_t rela_fileoff = 0;
        for (const auto& seg : out.segments) {
            if (seg.type == PT_LOAD && rela_vaddr >= seg.vaddr && rela_vaddr < seg.vaddr + seg.filesz) {
                rela_fileoff = seg.offset + (rela_vaddr - seg.vaddr);
                break;
            }
        }
        const Elf64_Rela* relas = reinterpret_cast<const Elf64_Rela*>(data + rela_fileoff);
        size_t n = sz / ent;
        for (size_t i = 0; i < n; i++) {
            const Elf64_Rela& r = relas[i];
            Relocation rel;
            rel.offset  = r.r_offset;
            rel.type    = ELF64_R_TYPE(r.r_info);
            rel.sym_idx = ELF64_R_SYM(r.r_info);
            rel.addend  = r.r_addend;
            out.relocations.push_back(rel);
        }
    };
    collect_rela(out.dyn.rela, out.dyn.relasz, out.dyn.relaent);
    if (out.dyn.pltrel == 7) { // DT_RELA
        uint64_t pltrelsz = 0;
        for (size_t i = 0; i < dyn_count; i++) {
            if (dyn[i].d_tag == 2) { /* DT_PLTRELSZ */ pltrelsz = dyn[i].d_un.d_val; break; }
        }
        // DT_PLTRELSZ = 2
        for (size_t i = 0; i < dyn_count; i++) {
            if (dyn[i].d_tag == 2) { pltrelsz = dyn[i].d_un.d_val; break; }
        }
        collect_rela(out.dyn.jmprel, pltrelsz, out.dyn.relaent);
    }

    // Resolver nomes dos simbolos de relocation
    for (auto& rel : out.relocations) {
        if (rel.sym_idx < out.exports.size() + out.imports.size()) {
            // symtab contem imports + exports; construimos a ordem nao trivial.
            // para simplicidade, procuramos no symtab direto se necessario.
        }
    }

    // ---- Init/Fini arrays ----
    auto load_array = [&](uint64_t vaddr, uint64_t sz, std::vector<uint64_t>& arr) {
        if (!vaddr || !sz) return;
        uint64_t fileoff = 0;
        for (const auto& seg : out.segments) {
            if (seg.type == PT_LOAD && vaddr >= seg.vaddr && vaddr < seg.vaddr + seg.filesz) {
                fileoff = seg.offset + (vaddr - seg.vaddr);
                break;
            }
        }
        const uint64_t* ptr = reinterpret_cast<const uint64_t*>(data + fileoff);
        size_t n = sz / 8;
        arr.assign(ptr, ptr + n);
    };
    load_array(out.dyn.init_array, out.dyn.init_array_size, out.init_array);
    load_array(out.dyn.fini_array, out.dyn.fini_array_size, out.fini_array);

    return true;
}

uint64_t ElfImage::find_export(const std::string& name) const {
    for (const auto& s : exports) {
        if (s.name == name) return base_addr + s.value;
    }
    return 0;
}

uint64_t compute_load_size(const ElfImage& img) {
    uint64_t max_end = 0;
    for (const auto& seg : img.segments) {
        if (seg.type == PT_LOAD) {
            uint64_t end = seg.vaddr + seg.memsz;
            if (end > max_end) max_end = end;
        }
    }
    // alinhar a 4KB
    return (max_end + 0xfff) & ~0xfffULL;
}

} // namespace bully::elf
