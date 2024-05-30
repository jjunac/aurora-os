#[macro_use]
pub mod kernel;
pub use kernel::Kernel;
mod gdt;
mod interrupts;
mod memory;
mod tty;
mod vga;
