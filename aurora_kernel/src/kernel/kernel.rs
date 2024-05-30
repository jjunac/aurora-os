use core::fmt;

use bootloader::BootInfo;
use x86_64::VirtAddr;

use super::{gdt, interrupts, memory, tty};

pub struct Kernel {
    pub tty: tty::Tty,
    pub vm_mapper: Option<memory::VmMapper>,
}

static mut KERNEL: Kernel = Kernel::new();

impl Kernel {
    const fn new() -> Self {
        Kernel {
            tty: tty::Tty::new(),
            vm_mapper: None,
        }
    }

    pub fn instance() -> &'static mut Kernel {
        unsafe { &mut KERNEL }
    }

    pub fn init(self: &mut Self, boot_info: &'static BootInfo) {
        interrupts::init_idt();
        gdt::init_gdt();
        interrupts::init_pics();

        self.vm_mapper = unsafe { Some(memory::VmMapper::new(VirtAddr::new(boot_info.physical_memory_offset), &boot_info.memory_map)) };
    }

    pub fn kprint_fmt(self: &mut Self, args: fmt::Arguments) {
        use core::fmt::Write;
        self.tty.writer.write_fmt(args).unwrap();
    }

    pub fn hlt_loop(self: &mut Self) -> ! {
        loop {
            x86_64::instructions::hlt();
        }
    }
}

#[macro_export]
macro_rules! kprint {
    ($($arg:tt)*) => ($crate::kernel::Kernel::instance().kprint_fmt(format_args!($($arg)*)));
}

#[macro_export]
macro_rules! kprintln {
    () => ($crate::kprint!("\n"));
    ($($arg:tt)*) => ($crate::kprint!("{}\n", format_args!($($arg)*)));
}



