#pragma once

#include <boost/preprocessor/repetition/repeat.hpp>

#include "kernel/kdefs.hpp"

namespace kernel {

struct Registers
{
   uint32_t ds;                  // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
   uint32_t int_no, err_code;    // Interrupt number and error code (if applicable)
   uint32_t eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
};

}

#define ISR_DECLARATION(z, n, data) extern "C" void isr##n ();
    BOOST_PP_REPEAT(32, ISR_DECLARATION, _)
#undef ISR_DECLARATION


