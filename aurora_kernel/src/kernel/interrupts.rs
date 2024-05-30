use pic8259::ChainedPics;
use x86_64::structures::idt::{InterruptDescriptorTable, InterruptStackFrame};

use crate::kernel::gdt;

pub const PIC_1_OFFSET: u8 = 32;
pub const PIC_2_OFFSET: u8 = PIC_1_OFFSET + 8;

static mut IDT: InterruptDescriptorTable = InterruptDescriptorTable::new();
static mut PICS: ChainedPics = unsafe { ChainedPics::new(PIC_1_OFFSET, PIC_2_OFFSET) };

#[derive(Debug, Clone, Copy)]
#[repr(u8)]
pub enum InterruptIndex {
    Timer = PIC_1_OFFSET,
    Keyboard,
}

impl InterruptIndex {
    fn as_u8(self) -> u8 {
        self as u8
    }

    fn as_usize(self) -> usize {
        usize::from(self.as_u8())
    }
}

macro_rules! set_generic_handler {
    // Define a handler for a given interrupt and set it in the IDT.
    ($name:ident) => {
        extern "x86-interrupt" fn $name(stack_frame: InterruptStackFrame) {
            kprintln!("EXCEPTION: {}\n{:#?}", stringify!($name), stack_frame);
        }
        IDT.$name.set_handler_fn($name);
    };
}
macro_rules! _set_generic_handler_with_err_code {
    // Define a handler for a given interrupt and set it in the IDT.
    ($name:ident) => {
        extern "x86-interrupt" fn $name(stack_frame: InterruptStackFrame, error_code: u64) -> ! {
            kprintln!(
                "EXCEPTION: {} (err {})\n{:#?}",
                stringify!($name),
                error_code,
                stack_frame
            );
        }
        IDT.$name.set_handler_fn($name);
    };
}

extern "x86-interrupt" fn double_fault_handler(
    stack_frame: InterruptStackFrame,
    error_code: u64,
) -> ! {
    panic!(
        "EXCEPTION: {} (err {})\n{:#?}",
        "DOUBLE FAULT", error_code, stack_frame
    );
}

extern "x86-interrupt" fn timer_interrupt_handler(_stack_frame: InterruptStackFrame) {
    // kprint!(".");
    unsafe {
        PICS.notify_end_of_interrupt(InterruptIndex::Timer.as_u8());
    }
}

extern "x86-interrupt" fn keyboard_interrupt_handler(_stack_frame: InterruptStackFrame) {
    use pc_keyboard::{layouts, DecodedKey, HandleControl, Keyboard, ScancodeSet1};
    use x86_64::instructions::port::Port;

    static mut KEYBOARD: Keyboard<layouts::Us104Key, ScancodeSet1> =
        Keyboard::new(ScancodeSet1::new(), layouts::Us104Key, HandleControl::Ignore);

    let keyboard = unsafe { &mut KEYBOARD };

    let mut port = Port::new(0x60);

    let scancode: u8 = unsafe { port.read() };
    if let Ok(Some(key_event)) = keyboard.add_byte(scancode) {
        if let Some(key) = keyboard.process_keyevent(key_event) {
            match key {
                DecodedKey::Unicode(character) => kprint!("{}", character),
                DecodedKey::RawKey(key) => kprint!("{:?}", key),
            }
        }
    }

    unsafe {
        PICS.notify_end_of_interrupt(InterruptIndex::Keyboard.as_u8());
    }
}

pub fn init_idt() {
    unsafe {
        set_generic_handler!(breakpoint);
        IDT.double_fault
            .set_handler_fn(double_fault_handler)
            .set_stack_index(gdt::DOUBLE_FAULT_IST_INDEX);
        IDT[InterruptIndex::Timer.as_usize()].set_handler_fn(timer_interrupt_handler);
        IDT[InterruptIndex::Keyboard.as_usize()].set_handler_fn(keyboard_interrupt_handler);
        IDT.load();
    }
}

pub fn init_pics() {
    unsafe { PICS.initialize() }
    x86_64::instructions::interrupts::enable();
}
