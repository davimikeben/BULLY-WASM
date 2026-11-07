#!/usr/bin/env python3
"""Bully ELF Analyzer — Inventario completo de dependencias para o runtime WASM.

Analisa o libGame.so (e libs relacionadas) e gera relatorio JSON + Markdown
com tudo o que o runtime de compatibilidade Android->WASM precisa implementar.

Uso:
    python3 elf_analyzer.py <caminho/libGame.so [--out report]

Saida:
    report.json   -> relatorio estruturado
    report.md     -> relatorio legivel por humanos
"""
import argparse
import json
import os
import sys
from collections import defaultdict
from dataclasses import dataclass, field, asdict
from typing import Any

try:
    from elftools.elf.elffile import ELFFile
    from elftools.elf.sections import SymbolTableSection
    from elftools.elf.dynamic import DynamicSection
    from elftools.elf.relocation import RelocationSection
    from elftools.elf.segments import Segment
except ImportError:
    sys.stderr.write("Erro: instale pyelftools: pip install pyelftools\n")
    sys.exit(1)


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------
R_AARCH64 = {
    0: "R_AARCH64_NONE",
    257: "R_AARCH64_ABS64",
    258: "R_AARCH64_ABS32",
    259: "R_AARCH64_ABS16",
    260: "R_AARCH64_PREL64",
    261: "R_AARCH64_PREL32",
    262: "R_AARCH64_PREL16",
    263: "R_AARCH64_MOVW_UABS_G0",
    264: "R_AARCH64_MOVW_UABS_G0_NC",
    265: "R_AARCH64_MOVW_UABS_G1",
    266: "R_AARCH64_MOVW_UABS_G1_NC",
    267: "R_AARCH64_MOVW_UABS_G2",
    268: "R_AARCH64_MOVW_UABS_G2_NC",
    269: "R_AARCH64_MOVW_UABS_G3",
    270: "R_AARCH64_MOVW_SABS_G0",
    271: "R_AARCH64_MOVW_SABS_G1",
    272: "R_AARCH64_MOVW_SABS_G2",
    273: "R_AARCH64_LD_PREL_LO19",
    274: "R_AARCH64_ADR_PREL_LO21",
    275: "R_AARCH64_ADR_PREL_PG_HI21",
    276: "R_AARCH64_ADR_PREL_PG_HI21_NC",
    277: "R_AARCH64_ADD_ABS_LO12_NC",
    278: "R_AARCH64_LDST8_ABS_LO12_NC",
    279: "R_AARCH64_TSTBR14",
    280: "R_AARCH64_CONDBR19",
    281: "R_AARCH64_JUMP26",
    282: "R_AARCH64_CALL26",
    283: "R_AARCH64_LDST16_ABS_LO12_NC",
    284: "R_AARCH64_LDST32_ABS_LO12_NC",
    285: "R_AARCH64_LDST64_ABS_LO12_NC",
    286: "R_AARCH64_LDST128_ABS_LO12_NC",
    287: "R_AARCH64_TLSDESC_LD64",
    288: "R_AARCH64_TLSDESC_ADD_LO12",
    289: "R_AARCH64_TLSDESC_CALL",
    290: "R_AARCH64_TLSLE_ADD_TPREL_HI12",
    291: "R_AARCH64_TLSLE_ADD_TPREL_LO12_NC",
    292: "R_AARCH64_TLSLE_LDST8_TPREL_LO12_NC",
    293: "R_AARCH64_TLSLE_LDST16_TPREL_LO12_NC",
    294: "R_AARCH64_TLSLE_LDST32_TPREL_LO12_NC",
    295: "R_AARCH64_TLSLE_LDST64_TPREL_LO12_NC",
    296: "R_AARCH64_TLSLE_LDST128_TPREL_LO12_NC",
    297: "R_AARCH64_TLSLE_MOVW_TPREL_G0",
    298: "R_AARCH64_TLSLE_MOVW_TPREL_G0_NC",
    299: "R_AARCH64_TLSLE_MOVW_TPREL_G1",
    300: "R_AARCH64_TLSLE_MOVW_TPREL_G1_NC",
    301: "R_AARCH64_TLSLE_MOVW_TPREL_G2",
    302: "R_AARCH64_TLSLE_MOVW_TPREL_G2_NC",
    303: "R_AARCH64_TLSLD_MOVW_DTPREL_G0",
    304: "R_AARCH64_TLSLD_MOVW_DTPREL_G0_NC",
    305: "R_AARCH64_TLSLD_MOVW_DTPREL_G1",
    306: "R_AARCH64_TLSLD_MOVW_DTPREL_G1_NC",
    307: "R_AARCH64_TLSLD_MOVW_DTPREL_G2",
    308: "R_AARCH64_TLSLD_MOVW_DTPREL_G2_NC",
    309: "R_AARCH64_TLSLD_LDST8_DTPREL_LO12_NC",
    310: "R_AARCH64_TLSLD_LDST16_DTPREL_LO12_NC",
    311: "R_AARCH64_TLSLD_LDST32_DTPREL_LO12_NC",
    312: "R_AARCH64_TLSLD_LDST64_DTPREL_LO12_NC",
    313: "R_AARCH64_TLSLD_LDST128_DTPREL_LO12_NC",
    314: "R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC",
    315: "R_AARCH64_TLSIE_LD_GOTTPREL_PREL19",
    316: "R_AARCH64_TLSIE_MOVW_GOTTPREL_G0",
    317: "R_AARCH64_TLSIE_MOVW_GOTTPREL_G0_NC",
    318: "R_AARCH64_TLSIE_MOVW_GOTTPREL_G1",
    319: "R_AARCH64_TLSIE_MOVW_GOTTPREL_G1_NC",
    320: "R_AARCH64_TLSIE_MOVW_GOTTPREL_G2",
    321: "R_AARCH64_TLSIE_MOVW_GOTTPREL_G2_NC",
    322: "R_AARCH64_TLSIE_ADR_GOTTPREL_PAGE21",
    323: "R_AARCH64_TLSIE_LD64_GOTTPREL_LO12_NC",
    1024: "R_AARCH64_COPY",
    1025: "R_AARCH64_GLOB_DAT",
    1026: "R_AARCH64_JUMP_SLOT",
    1027: "R_AARCH64_RELATIVE",
    1028: "R_AARCH64_TLS_DTPREL64",
    1029: "R_AARCH64_TLS_DTPMOD64",
    1030: "R_AARCH64_TLS_TPREL64",
    1031: "R_AARCH64_TLSDESC",
    1032: "R_AARCH64_IRELATIVE",
}


def reloc_name(r_type):
    return R_AARCH64.get(r_type, f"R_AARCH64_UNKNOWN_{r_type}")


def safe_str(b):
    if isinstance(b, bytes):
        try:
            return b.decode("utf-8", errors="replace")
        except Exception:
            return repr(b)
    return str(b)


# ---------------------------------------------------------------------------
# Analise
# ---------------------------------------------------------------------------
@dataclass
class ElfReport:
    file_name: str
    file_size: int
    machine: str
    elf_class: str
    endian: str
    type: str
    abi: str
    entry: int
    segments: list = field(default_factory=list)
    needed: list = field(default_factory=list)
    soname: str = ""
    rpath: str = ""
    runpath: str = ""
    flags: str = ""
    exports: list = field(default_factory=list)
    imports: list = field(default_factory=list)
    jni_exports: list = field(default_factory=list)
    relocations: dict = field(default_factory=dict)
    init_array: list = field(default_factory=list)
    init_func: str = ""
    fini_func: str = ""
    tls: dict = field(default_factory=dict)
    dynamic_tags: dict = field(default_factory=dict)
    import_groups: dict = field(default_factory=dict)


def analyze(path: str) -> ElfReport:
    with open(path, "rb") as f:
        elf = ELFFile(f)
        eh = elf.header

        rep = ElfReport(
            file_name=os.path.basename(path),
            file_size=os.path.getsize(path),
            machine=eh["e_machine"],
            elf_class=eh["e_ident"]["EI_CLASS"],
            endian=eh["e_ident"]["EI_DATA"],
            type=eh["e_type"],
            abi=eh["e_ident"]["EI_OSABI"],
            entry=eh["e_entry"],
        )

        # --- Segmentos ---
        for seg in elf.iter_segments():
            rep.segments.append({
                "type": seg["p_type"],
                "offset": seg["p_offset"],
                "vaddr": seg["p_vaddr"],
                "filesz": seg["p_filesz"],
                "memsz": seg["p_memsz"],
                "flags": seg["p_flags"],
                "align": seg["p_align"],
            })

        # --- Secao dinamica (NEEDED, SONAME, flags) ---
        STRING_TAGS = {"DT_NEEDED", "DT_SONAME", "DT_RPATH", "DT_RUNPATH", "DT_AUDIT", "DT_DEPAUDIT", "DT_FILTER", "DT_FILTER"}
        dyn_tags = {}
        for sec in elf.iter_sections():
            if isinstance(sec, DynamicSection):
                for tag in sec.iter_tags():
                    name = tag.entry.d_tag
                    raw = tag.entry.d_val
                    if name in STRING_TAGS:
                        try:
                            val = sec._stringtable.get_string(raw)
                        except Exception:
                            val = safe_str(raw)
                    else:
                        val = safe_str(raw)
                    dyn_tags.setdefault(name, []).append(val)
                    if name == "DT_NEEDED":
                        rep.needed.append(val)
                    elif name == "DT_SONAME":
                        rep.soname = val
                    elif name == "DT_RPATH":
                        rep.rpath = val
                    elif name == "DT_RUNPATH":
                        rep.runpath = val
                    elif name == "DT_FLAGS":
                        rep.flags = val
        rep.dynamic_tags = {k: v for k, v in dyn_tags.items()}

        # --- Simbolos dinamicos ---
        symtab = None
        for sec in elf.iter_sections():
            if isinstance(sec, SymbolTableSection) and sec.name == ".dynsym":
                symtab = sec
                break

        if symtab:
            for sym in symtab.iter_symbols():
                name = sym.name
                if not name:
                    continue
                info = sym["st_info"]
                if hasattr(info, "bind"):
                    bind = info.bind
                    typ = info.type
                else:
                    bind = info >> 4
                    typ = info & 0xf
                shndx = sym["st_shndx"]
                entry = {
                    "name": name,
                    "value": sym["st_value"],
                    "size": sym["st_size"],
                    "bind": bind,
                    "type": typ,
                    "shndx": shndx,
                }
                if shndx == "SHN_UNDEF":
                    rep.imports.append(entry)
                else:
                    rep.exports.append(entry)
                    if name.startswith("Java_"):
                        rep.jni_exports.append(entry)

        # --- Relocations ---
        reloc_counts = defaultdict(int)
        reloc_samples = defaultdict(list)
        for sec in elf.iter_sections():
            if isinstance(sec, RelocationSection):
                symtab_idx = sec["sh_link"]
                sym_sec = elf.get_section(symtab_idx)
                for rel in sec.iter_relocations():
                    rtype = rel["r_info_type"]
                    rname = reloc_name(rtype)
                    reloc_counts[rname] += 1
                    if len(reloc_samples[rname]) < 5:
                        sym_name = ""
                        if sym_sec and rel["r_info_sym"]:
                            try:
                                sym_name = sym_sec.get_symbol(rel["r_info_sym"]).name
                            except Exception:
                                pass
                        reloc_samples[rname].append({
                            "offset": rel["r_offset"],
                            "addend": rel["r_addend"],
                            "symbol": sym_name,
                        })
        rep.relocations = {
            "counts": dict(reloc_counts),
            "samples": dict(reloc_samples),
        }

        # --- Init array / fini ---
        for sec in elf.iter_sections():
            if sec.name in (".init_array", ".fini_array", ".preinit_array"):
                data = sec.data()
                addrs = []
                for i in range(0, len(data), 8):
                    addr = int.from_bytes(data[i:i+8], "little")
                    addrs.append(addr)
                if sec.name == ".init_array":
                    rep.init_array = addrs
            if sec.name == ".init":
                rep.init_func = "present"
            if sec.name == ".fini":
                rep.fini_func = "present"

        # --- TLS ---
        tls_seg = elf.get_section_by_name(".tbss") or elf.get_section_by_name(".tdata")
        tls = {}
        for sec in elf.iter_sections():
            if sec["sh_type"] == "SHT_TLS":
                tls[sec.name] = {"size": sec["sh_size"], "entsize": sec["sh_entsize"]}
        if tls:
            rep.tls = tls

        return rep


def group_imports(imports: list) -> dict:
    """Agrupa imports por prefixo de API (gl*, egl*, al*, A*, Java*, libc*)"""
    groups = defaultdict(list)
    for s in imports:
        n = s["name"]
        if n.startswith("gl"):
            groups["OpenGL ES"].append(n)
        elif n.startswith("egl"):
            groups["EGL"].append(n)
        elif n.startswith("al") or n.startswith("alc") or n.startswith("AL"):
            groups["OpenAL"].append(n)
        elif n.startswith("A") and len(n) > 1 and n[1].isupper():
            groups["Android (libandroid)"].append(n)
        elif n.startswith("Java_"):
            groups["JNI"].append(n)
        elif n.startswith("ANative") or n.startswith("AAsset"):
            groups["Android NDK"].append(n)
        elif any(n.startswith(p) for p in ("__", "pthread", "malloc", "free", "calloc", "realloc", "mem", "str", "printf", "fopen", "fread", "fwrite", "fclose", "open", "read", "write", "close", "mmap", "munmap", "dlopen", "dlsym", "dlclose", "dlerror")):
            groups["libc/libdl/pthread"].append(n)
        else:
            groups["Outros"].append(n)
    return {k: sorted(set(v)) for k, v in groups.items()}


# ---------------------------------------------------------------------------
# Relatorios
# ---------------------------------------------------------------------------
def to_json(rep: ElfReport) -> str:
    d = asdict(rep)
    return json.dumps(d, indent=2, ensure_ascii=False, default=str)


def to_markdown(rep: ElfReport) -> str:
    lines = []
    a = lambda s="": lines.append(s)
    a(f"# Relatorio ELF — `{rep.file_name}`")
    a()
    a("## 1. Cabecalho ELF")
    a()
    a(f"- Arquitetura: **{rep.machine}")
    a(f"- Classe: {rep.elf_class}")
    a(f"- Endianness: {rep.endian}")
    a(f"- Tipo: {rep.type}")
    a(f"- ABI: {rep.abi}")
    a(f"- Entry point: `0x{rep.entry:x}`")
    a(f"- Tamanho: {rep.file_size:,} bytes ({rep.file_size / 1024 / 1024:.1f} MB)")
    a()
    a("## 2. Segmentos (Program Headers)")
    a()
    a("| Tipo | Offset | VAddr | FileSz | MemSz | Flags | Align |")
    a("|------|--------|-------|-------|-------|-------|-------|")
    for s in rep.segments:
        a(f"| {s['type']} | 0x{s['offset']:x} | 0x{s['vaddr']:x} | {s['filesz']} | {s['memsz']} | {s['flags']} | {s['align']} |")
    a()
    a("## 3. Dependencias dinamicas (DT_NEEDED)")
    a()
    for n in rep.needed:
        a(f"- `{n}`")
    if rep.soname:
        a()
        a(f"SONAME: `{rep.soname}`")
    a()
    a("## 4. Simbolos")
    a()
    a(f"- **Exports**: {len(rep.exports)}")
    a(f"- **Imports**: {len(rep.imports)}")
    a(f"- **JNI exports**: {len(rep.jni_exports)}")
    a()
    a("### 4.1 Imports agrupados por API")
    a()
    groups = group_imports(rep.imports)
    for grp, syms in groups.items():
        a(f"#### {grp} ({len(syms)})")
        a()
        a("```")
        for s in syms:
            a(s)
        a("```")
        a()
    a("### 4.2 Exports JNI")
    a()
    a("```")
    for s in rep.jni_exports:
        a(s["name"])
    a("```")
    a()
    a("## 5. Relocations")
    a()
    a("| Tipo | Contagem |")
    a("|------|----------|")
    for rtype, cnt in sorted(rep.relocations["counts"].items()):
        a(f"| {rtype} | {cnt} |")
    a()
    a("### 5.1 Amostras de relocations")
    a()
    for rtype, samples in rep.relocations["samples"].items():
        a(f"#### {rtype}")
        a()
        a("| Offset | Addend | Simbolo |")
        a("|--------|--------|---------|")
        for sm in samples:
            a(f"| 0x{sm['offset']:x} | {sm['addend']} | {sm['symbol']} |")
        a()
    a("## 6. Inicializacao")
    a()
    a(f"- .init_array entries: {len(rep.init_array)}")
    if rep.init_array:
        a()
        a("```")
        for addr in rep.init_array[:20]:
            a(f"0x{addr:x}")
        if len(rep.init_array) > 20:
            a(f"... (+{len(rep.init_array)-20} mais)")
        a("```")
    a()
    a(f"- .init: {rep.init_func or 'ausente'}")
    a(f"- .fini: {rep.fini_func or 'ausente'}")
    a()
    a("## 7. TLS")
    a()
    if rep.tls:
        for name, info in rep.tls.items():
            a(f"- {name}: size={info['size']}, entsize={info['entsize']}")
    else:
        a("- Nenhuma secao TLS encontrada")
    a()
    a("## 8. Tags dinamicas")
    a()
    a("| Tag | Valor |")
    a("|-----|-------|")
    for tag, vals in rep.dynamic_tags.items():
        for v in vals:
            a(f"| {tag} | {v} |")
    a()
    return "\n".join(lines)


# ---------------------------------------------------------------------------
def main():
    ap = argparse.ArgumentParser(description="Bully ELF Analyzer")
    ap.add_argument("elf", help="Caminho do .so (libGame.so)")
    ap.add_argument("--out", default="report", help="Prefixo de saida (report.json / report.md)")
    args = ap.parse_args()

    if not os.path.isfile(args.elf):
        sys.stderr.write(f"Arquivo nao encontrado: {args.elf}\n")
        sys.exit(1)

    rep = analyze(args.elf)
    rep.import_groups = group_imports(rep.imports)

    json_path = f"{args.out}.json"
    md_path = f"{args.out}.md"

    with open(json_path, "w", encoding="utf-8") as f:
        f.write(to_json(rep))
    with open(md_path, "w", encoding="utf-8") as f:
        f.write(to_markdown(rep))

    print(f"OK. JSON -> {json_path}")
    print(f"OK. MD   -> {md_path}")
    print()
    print(f"Arquitetura: {rep.machine}")
    print(f"Imports: {len(rep.imports)} | Exports: {len(rep.exports)} | JNI: {len(rep.jni_exports)}")
    print(f"NEEDED: {', '.join(rep.needed)}")
    print(f"Tipos de relocation: {len(rep.relocations['counts'])}")
    print(f"Init array entries: {len(rep.init_array)}")


if __name__ == "__main__":
    main()
