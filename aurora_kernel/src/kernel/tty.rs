use super::vga::{VgaBuffer, VgaWriter};

pub struct Tty {
    pub writer: VgaWriter,
}

impl Tty {
    pub const fn new() -> Self {
        Tty {
            writer: VgaWriter::new(0xb8000 as *mut VgaBuffer),
        }
    }
}
