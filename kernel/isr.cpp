#include "kernel/isr.hpp"

#include "libcpp/stdio.hpp"
#include "kernel/asm/io.hpp"

namespace kernel {


}

extern "C" void isr_handler(kernel::Registers regs)
{
    
    libcpp::printf("Interrupt received: %d ", regs.int_no);

    switch (regs.int_no) {
    case 0:  libcpp::printf("Division by zero exception"); break;
    case 1:  libcpp::printf("Debug exception"); break;
    case 2:  libcpp::printf("Non maskable interrupt"); break;
    case 3:  libcpp::printf("Breakpoint exception"); break;
    case 4:  libcpp::printf("Into detected overflow"); break;
    case 5:  libcpp::printf("Out of bounds exception"); break;
    case 6:  libcpp::printf("Invalid opcode exception"); break;
    case 7:  libcpp::printf("No coprocessor exception"); break;
    case 8:  libcpp::printf("Double fault (error %d)", regs.err_code); break;
    case 9:  libcpp::printf("Coprocessor segment overrun"); break;
    case 10: libcpp::printf("Bad TSS (error %d)", regs.err_code); break;
    case 11: libcpp::printf("Segment not present (error %d)", regs.err_code); break;
    case 12: libcpp::printf("Stack fault (error %d)", regs.err_code); break;
    case 13: libcpp::printf("General protection fault (error %d)", regs.err_code); break;
    case 14: libcpp::printf("Page fault (error %d)", regs.err_code); break;
    case 15: libcpp::printf("Unknown interrupt exception"); break;
    case 16: libcpp::printf("Coprocessor fault"); break;
    case 17: libcpp::printf("Alignment check exception"); break;
    case 18: libcpp::printf("Machine check exception"); break;
    default:
        if (regs.int_no < 32) {
            libcpp::print("Reserved");
        } else {
            libcpp::print("Unknown");
        }
    }
    libcpp::putchar('\n');
}

extern "C" void irq_handler(kernel::Registers regs)
{
    // // Send an EOI (end of interrupt) signal to the PICs.
    // // If this interrupt involved the slave.
    // if (regs.int_no >= 7)
    // {
    //     // Send reset signal to slave.
    //     outb(0xA0, 0x20);
    // }
    // // Send reset signal to master. (As well as slave, if necessary).
    // outb(0x20, 0x20);

    libcpp::printf("Interrupt received: %d ", regs.int_no);

    switch (regs.int_no) {
    case 0:  libcpp::printf("Programmable Interrupt Timer Interrupt"); break;
    case 1:  libcpp::printf("Keyboard Interrupt"); break;
    case 2:  libcpp::printf("Cascade (used internally by the two PICs. never raised)"); break;
    case 3:  libcpp::printf("COM2 (if enabled)"); break;
    case 4:  libcpp::printf("COM1 (if enabled)"); break;
    case 5:  libcpp::printf("LPT2 (if enabled)"); break;
    case 6:  libcpp::printf("Floppy Disk"); break;
    case 7:  libcpp::printf("LPT1 / Unreliable 'spurious' interrupt (usually)"); break;
    case 8:  libcpp::printf("CMOS real-time clock (if enabled)"); break;
    case 9:  libcpp::printf("Free for peripherals / legacy SCSI / NIC"); break;
    case 10: libcpp::printf("Free for peripherals / SCSI / NIC"); break;
    case 11: libcpp::printf("Free for peripherals / SCSI / NIC"); break;
    case 12: libcpp::printf("PS2 Mouse"); break;
    case 13: libcpp::printf("FPU / Coprocessor / Inter-processor"); break;
    case 14: libcpp::printf("Primary ATA Hard Disk"); break;
    case 15: libcpp::printf("Secondary ATA Hard Disk"); break;
    default:
        libcpp::print("Unknown");
    }
    libcpp::putchar('\n');
}
