
use x86_64::structures::idt::{InterruptDescriptorTable, InterruptStackFrame};

use crate::kernel::gdt;

static mut IDT: InterruptDescriptorTable = InterruptDescriptorTable::new();

macro_rules! set_generic_handler {
    // Define a handler for a given interrupt and set it in the IDT.
    ($name:ident) => {
        extern "x86-interrupt" fn $name(stack_frame: InterruptStackFrame)
        {
            kprintln!("EXCEPTION: {}\n{:#?}", stringify!($name), stack_frame);
        }
        IDT.$name.set_handler_fn($name);
    };
}
macro_rules! _set_generic_handler_with_err_code {
    // Define a handler for a given interrupt and set it in the IDT.
    ($name:ident) => {
        extern "x86-interrupt" fn $name(stack_frame: InterruptStackFrame, error_code: u64) -> !
        {
            kprintln!("EXCEPTION: {} (err {})\n{:#?}", stringify!($name), error_code, stack_frame);
        }
        IDT.$name.set_handler_fn($name);
    };
}

extern "x86-interrupt" fn double_fault_handler(stack_frame: InterruptStackFrame, error_code: u64) -> !
{
    panic!("EXCEPTION: {} (err {})\n{:#?}", "DOUBLE FAULT", error_code, stack_frame);
}

pub fn init_idt() {
    unsafe {
        set_generic_handler!(breakpoint);
        IDT.double_fault.set_handler_fn(double_fault_handler)
            .set_stack_index(gdt::DOUBLE_FAULT_IST_INDEX);
        IDT.load();
    }
}

