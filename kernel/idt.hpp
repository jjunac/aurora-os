#pragma once

#include "kernel/kdefs.hpp"

namespace kernel {

// A struct describing an interrupt gate.
struct IdtEntry
{
    IdtEntry() = default;
    IdtEntry(uint32_t handler, uint16_t selector, uint8_t flags)
        : base_low(handler & 0xFFFF)
        , selector(selector)
        , reserved(0)
        // We must uncomment the OR below when we get to using userspace.
        // It sets the interrupt gate's privilege level to 3.
        , flags(flags /* | (userspace ? 0x60 : 0) */)
        , base_high((handler >> 16) & 0xFFFF) {}

    uint16_t base_low;      // The lower 16 bits of the address to jump to when this interrupt fires
    uint16_t selector;      // The GDT segment selector that the CPU will load into CS before calling the ISR
    uint8_t  reserved;      // This must always be 0
    uint8_t  flags;         // More flags. See documentation
    uint16_t base_high;     // The upper 16 bits of the address to jump to
} __attribute__((packed));


// A struct describing a pointer to an array of interrupt handlers.
// This is in a format suitable for giving to 'lidt'.
struct IdtPointer
{
    IdtPointer() = default;
    IdtPointer(uint16_t limit, uint32_t base) : limit(limit), base(base) {}

    uint16_t limit;
    uint32_t base;      // The address of the first element in our IdtEntry array.
} __attribute__((packed));

}

inline void kernel_load_idt(uint32_t idtPtr) {
    __asm__ volatile ("lidt %0" : : "m"(idtPtr)); // load the new IDT
    __asm__ volatile ("sti"); // set the interrupt flag
}