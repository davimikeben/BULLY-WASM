// runtime/libc/shim.hpp
// Shims para libc, libm, libdl, pthread, C++ ABI.
#pragma once
#include "../arm64/cpu.hpp"
#include "../elf/linker.hpp"

namespace bully::libc {

void register_shims(bully::elf::Linker& linker);

} // namespace bully::libc
