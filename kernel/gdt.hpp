#pragma once

#include "kernel/kdefs.hpp"

namespace kernel {

// This structure contains the value of one GDT entry.
// We use the attribute 'packed' to tell GCC not to change
// any of the alignment in the structure.
struct GdtEntry
{
    GdtEntry() = default;
    GdtEntry(uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity)
        : limit_low(limit & 0xFFFF)
        , base_low(base & 0xFFFF)
        , base_middle((base >> 16) & 0xFF)
        , access(access)
        , granularity(((limit >> 16) & 0X0F) | granularity)
        , base_high((base >> 24) & 0xFF) {}

    uint16_t limit_low;           // The lower 16 bits of the limit.
    uint16_t base_low;            // The lower 16 bits of the base.
    uint8_t  base_middle;         // The next 8 bits of the base.
    uint8_t  access;              // Access flags, determine what ring this segment can be used in.
    uint8_t  granularity;
    uint8_t  base_high;           // The last 8 bits of the base.
} __attribute__((packed));


struct GdtPointer
{
    GdtPointer() = default;
    GdtPointer(uint16_t limit, uint32_t base) : limit(limit), base(base) {}

    uint16_t limit;               // The upper 16 bits of all selector limits.
    uint32_t base;                // The address of the first GdtEntry struct.
} __attribute__((packed));

}

extern "C" void kernel_load_gdt(uint32_t gdtPtr);