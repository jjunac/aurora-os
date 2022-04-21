#pragma once

#include "kernel/kdef.hpp"
#include "kernel/tty.hpp"

namespace kernel {

struct Kernel {
    void Init();

    Tty tty;
};

Kernel& instance();

// Conveniency methods
inline Tty& tty() { return instance().tty; }

}
