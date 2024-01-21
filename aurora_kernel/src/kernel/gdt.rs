use x86_64::VirtAddr;
use x86_64::instructions::tables::load_tss;
use x86_64::registers::segmentation::{CS, Segment};
use x86_64::structures::gdt::{GlobalDescriptorTable, Descriptor};
use x86_64::structures::tss::TaskStateSegment;

pub const DOUBLE_FAULT_IST_INDEX: u16 = 0;

static mut GDT: GlobalDescriptorTable =  GlobalDescriptorTable::new();
static mut TSS: TaskStateSegment =  TaskStateSegment::new();

pub fn init_gdt() {
    unsafe { 
        TSS.interrupt_stack_table[DOUBLE_FAULT_IST_INDEX as usize] = {
            const STACK_SIZE: usize = 4096 * 5;
            static mut STACK: [u8; STACK_SIZE] = [0; STACK_SIZE];

            let stack_start = VirtAddr::from_ptr(&STACK);
            let stack_end = stack_start + STACK_SIZE;
            stack_end
        };

        let code_selector = GDT.add_entry(Descriptor::kernel_code_segment());
        let tss_selector = GDT.add_entry(Descriptor::tss_segment(&TSS));
        GDT.load();

        CS::set_reg(code_selector);
        load_tss(tss_selector);
    }

}
