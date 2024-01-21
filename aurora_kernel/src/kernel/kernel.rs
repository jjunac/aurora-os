use core::fmt;

use super::{tty, interrupts, gdt};



pub struct Kernel {
    pub tty: tty::Tty,
}

static mut KERNEL: Kernel = Kernel::new();

impl Kernel {
    const fn new() -> Self {
        Kernel {
            tty: tty::Tty::new(),
        }
    }

    pub fn instance() -> &'static mut Kernel {
        unsafe { &mut KERNEL }
    }

    pub fn init(self: &mut Self) {
        interrupts::init_idt();
        gdt::init_gdt();
    }

    pub fn kprint_fmt(self: &mut Self, args: fmt::Arguments) {
        use core::fmt::Write;
        self.tty.writer.write_fmt(args).unwrap();
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



