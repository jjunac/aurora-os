#[macro_use]
pub mod kernel;
pub use kernel::Kernel;
mod tty;
mod vga;
mod interrupts;
mod gdt;
