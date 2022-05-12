#include "kernel/isr.hpp"

#include "libcpp/stdio.hpp"

namespace kernel {


}

extern "C" void isr_handler(kernel::Registers regs)
{
    libcpp::printf("Interrupt received: %d\n", regs.int_no);
}
