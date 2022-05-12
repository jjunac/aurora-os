#pragma once

#include <array>

#include "kernel/gdt.hpp"
#include "kernel/idt.hpp"
#include "kernel/kdefs.hpp"
#include "kernel/tty.hpp"

namespace kernel {

class Kernel {
public:
    Kernel() = default;

    void init();

    Tty tty;

private:
    void initGdt();
    void initIdt();

    std::array<GdtEntry, 5> gdtEntries_;
    GdtPointer gdtPtr_;

    std::array<IdtEntry, 256> idtEntries_;
    IdtPointer idtPtr_;
};

Kernel& instance();

// Conveniency methods
inline Tty& tty() { return instance().tty; }

}
