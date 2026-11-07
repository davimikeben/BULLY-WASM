// runtime/android/shim.h
// Shims para libandroid.so, liblog.so — substituem APIs Android usadas pelo libGame.so.
#pragma once
#include "../arm64/cpu.hpp"
#include "../elf/linker.hpp"
#include <cstdint>
#include <cstddef>

namespace bully::android {

// Registra todos os shims no linker usando HostFn.
void register_shims(bully::elf::Linker& linker);

// Define o diretorio de assets do host.
void set_assets_dir(const char* dir);

} // namespace bully::android
