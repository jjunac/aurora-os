#include "kernel/kernel.hpp"

#include "libcpp/stdio.hpp"

#include "kernel/asm/io.hpp"
#include "kernel/idt.hpp"
#include "kernel/isr.hpp"
#include "kernel/tty.hpp"


namespace kernel {

void Kernel::init() {
    tty.init();

    initGdt();
    initIdt();

    // libcpp::println("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Massa sed elementum tempus egestas. Eget arcu dictum varius duis at consectetur lorem donec massa. Rhoncus dolor purus non enim praesent elementum facilisis leo vel. Viverra orci sagittis eu volutpat odio. Velit egestas dui id ornare arcu odio ut. Porta non pulvinar neque laoreet. Lorem ipsum dolor sit amet consectetur adipiscing elit pellentesque. Sit amet porttitor eget dolor morbi non arcu risus. Porttitor eget dolor morbi non arcu risus quis varius. Vitae ultricies leo integer malesuada nunc. Egestas pretium aenean pharetra magna ac placerat vestibulum lectus. Sed viverra tellus in hac habitasse platea dictumst vestibulum rhoncus. Rhoncus dolor purus non enim praesent elementum facilisis leo vel. Fringilla phasellus faucibus scelerisque eleifend donec pretium vulputate sapien nec. Nunc id cursus metus aliquam.\n");
    // libcpp::println("Quam adipiscing vitae proin sagittis nisl. Nulla malesuada pellentesque elit eget gravida cum sociis natoque. Donec et odio pellentesque diam. In cursus turpis massa tincidunt dui ut. Sed cras ornare arcu dui vivamus arcu felis bibendum ut. Purus non enim praesent elementum facilisis. Diam in arcu cursus euismod quis. Non quam lacus suspendisse faucibus interdum posuere lorem ipsum. Vitae proin sagittis nisl rhoncus mattis rhoncus urna. Vel risus commodo viverra maecenas.\n");
    // libcpp::println("A scelerisque purus semper eget duis at tellus at urna. Mi tempus imperdiet nulla malesuada pellentesque elit. Pharetra pharetra massa massa ultricies mi quis hendrerit. Amet justo donec enim diam vulputate ut pharetra sit. Viverra mauris in aliquam sem fringilla ut morbi. Mus mauris vitae ultricies leo integer malesuada. Id volutpat lacus laoreet non. Senectus et netus et malesuada fames. Ac tincidunt vitae semper quis lectus nulla at volutpat diam. Sit amet commodo nulla facilisi nullam vehicula ipsum a arcu. Nunc consequat interdum varius sit amet mattis. Nisl condimentum id venenatis a. Id leo in vitae turpis massa.\n");
    // libcpp::println("At imperdiet dui accumsan sit amet nulla. Facilisis volutpat est velit egestas dui id ornare arcu. Vitae nunc sed velit dignissim sodales. In hendrerit gravida rutrum quisque non tellus orci. Habitant morbi tristique senectus et netus et malesuada. Faucibus turpis in eu mi bibendum neque egestas. Morbi enim nunc faucibus a pellentesque sit amet. Cras ornare arcu dui vivamus arcu. Adipiscing elit duis tristique sollicitudin nibh sit amet commodo nulla. Amet consectetur adipiscing elit duis tristique sollicitudin nibh sit amet.\n");

    libcpp::printf("The answer to %s is %d.", "the ultimate question of life, the universe and everything", 42);
}

void Kernel::initGdt() {
    gdtPtr_.limit = sizeof(gdtEntries_) - 1;
    gdtPtr_.base  = &gdtEntries_[0];

    std::get<0>(gdtEntries_) = GdtEntry{0, 0         , 0   , 0   };     // Null segment
    std::get<1>(gdtEntries_) = GdtEntry{0, 0xFFFFFFFF, 0x9A, 0xCF};     // Kernel mode code segment
    std::get<2>(gdtEntries_) = GdtEntry{0, 0xFFFFFFFF, 0x92, 0xCF};     // Kernel mode data segment
    std::get<3>(gdtEntries_) = GdtEntry{0, 0xFFFFFFFF, 0xFA, 0xCF};     // User mode code segment
    std::get<4>(gdtEntries_) = GdtEntry{0, 0xFFFFFFFF, 0xF2, 0xCF};     // User mode data segment

    kernel_load_gdt(reinterpret_cast<uint32_t>(&gdtPtr_));
    libcpp::println("kernel_load_gdt done\n");
}

void Kernel::initIdt() {
    idtPtr_.limit = sizeof(idtEntries_) - 1;
    idtPtr_.base  = &idtEntries_[0];

    libcpp::printf("48: %d\n", 48);
    libcpp::printf("2048 : %d\n", 2048);
    libcpp::printf("12288 : %d\n", 12288);
    libcpp::printf("sizeof(IdtEntry): %d\n", sizeof(IdtEntry));
    libcpp::printf("sizeof(idtEntries_): %d\n", sizeof(idtEntries_));
    libcpp::printf("sizeof(idtPtr_): %d\n", sizeof(idtPtr_));
    libcpp::printf("Loading IDT base: %d limit: %d\n", idtPtr_.base, idtPtr_.limit);

    // Maps the CPU dedicated interrupts:
    //   0  - Division by zero exception
    //   1  - Debug exception
    //   2  - Non maskable interrupt
    //   3  - Breakpoint exception
    //   4  - 'Into detected overflow'
    //   5  - Out of bounds exception
    //   6  - Invalid opcode exception
    //   7  - No coprocessor exception
    //   8  - Double fault (pushes an error code)
    //   9  - Coprocessor segment overrun
    //   10 - Bad TSS (pushes an error code)
    //   11 - Segment not present (pushes an error code)
    //   12 - Stack fault (pushes an error code)
    //   13 - General protection fault (pushes an error code)
    //   14 - Page fault (pushes an error code)
    //   15 - Unknown interrupt exception
    //   16 - Coprocessor fault
    //   17 - Alignment check exception
    //   18 - Machine check exception
    //   19-31 - Reserved
    #define SET_ISR(z, n, data) std::get<n>(idtEntries_) = IdtEntry{reinterpret_cast<uint32_t>(&isr##n), 0x08, 0x8E};
        BOOST_PP_REPEAT(32, SET_ISR, _)
    #undef SET_ISR

    // Remap the irq table from 0-15 to 32-47 to avoid conflicting with CPU dedicated ISR
    // 0  - Programmable Interrupt Timer Interrupt
    // 1  - Keyboard Interrupt
    // 2  - Cascade (used internally by the two PICs. never raised)
    // 3  - COM2 (if enabled)
    // 4  - COM1 (if enabled)
    // 5  - LPT2 (if enabled)
    // 6  - Floppy Disk
    // 7  - LPT1 / Unreliable "spurious" interrupt (usually)
    // 8  - CMOS real-time clock (if enabled)
    // 9  - Free for peripherals / legacy SCSI / NIC
    // 10 - Free for peripherals / SCSI / NIC
    // 11 - Free for peripherals / SCSI / NIC
    // 12 - PS2 Mouse
    // 13 - FPU / Coprocessor / Inter-processor
    // 14 - Primary ATA Hard Disk
    // 15 - Secondary ATA Hard Disk
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    // TODO: map ISR/IRQ to specific functions
    #define SET_IRQ(z, n, data) std::get<32+n>(idtEntries_) = IdtEntry{reinterpret_cast<uint32_t>(&irq##n), 0x08, 0x8E};
        BOOST_PP_REPEAT(16, SET_IRQ, _)
    #undef SET_IRQ

    // std::get<32>(idtEntries_) = IdtEntry{reinterpret_cast<uint32_t>(&irq0), 0x08, 0x8E};
    std::get<33>(idtEntries_) = IdtEntry{reinterpret_cast<uint32_t>(&irq1), 0x08, 0x8E};

    kernel_load_idt(&idtPtr_);
    libcpp::println("kernel_load_idt done\n");

    __asm__ volatile ("int $0x3");
    __asm__ volatile ("int $0x4");
    __asm__ volatile ("int $0x1F");
    libcpp::println("After INT\n");

    // FIXME
    __asm__ volatile ("int $0x21");

}


static Kernel instance_{};

Kernel& instance() {
    return instance_;
}

}

extern "C" void kernel_main()
{
    kernel::instance().init();
}
