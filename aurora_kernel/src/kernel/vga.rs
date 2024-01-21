#[cfg(test)]
#[path = "vga_tests.rs"]
mod vga_tests;

use core::{ptr, fmt};

#[allow(dead_code)]
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum Color {
    Black = 0,
    Blue = 1,
    Green = 2,
    Cyan = 3,
    Red = 4,
    Magenta = 5,
    Brown = 6,
    LightGray = 7,
    DarkGray = 8,
    LightBlue = 9,
    LightGreen = 10,
    LightCyan = 11,
    LightRed = 12,
    Pink = 13,
    Yellow = 14,
    White = 15,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(transparent)]
pub struct ColorCode(u8);

impl ColorCode {
    pub const fn new(foreground: Color, background: Color) -> ColorCode {
        ColorCode((background as u8) << 4 | (foreground as u8))
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(C)]
pub struct VgaEntry {
    pub ascii_character: u8,
    pub color_code: ColorCode,
}

pub const BUFFER_HEIGHT: usize = 25;
pub const BUFFER_WIDTH: usize = 80;
pub const BUFFER_SIZE: usize = BUFFER_HEIGHT * BUFFER_WIDTH;

#[repr(transparent)]
pub struct VgaBuffer {
    chars: [VgaEntry; BUFFER_SIZE],
}

pub struct VgaWriter {
    position: usize,
    color_code: ColorCode,
    buffer: *mut VgaBuffer,
}

// FIXME: For now we don't care about concurrency since we are mono-threaded
unsafe impl Sync for VgaWriter {}

impl VgaWriter {
    pub const fn new(buffer: *mut VgaBuffer) -> Self {
        VgaWriter {
            position: 0,
            color_code: ColorCode::new(Color::White, Color::Black),
            buffer,
        }
    }

    pub fn write_byte(&mut self, byte: u8) {
        if self.position >= BUFFER_SIZE {
            self.scroll();
        }
        match byte {
            b'\n' => self.new_line(),
            byte => {
                let buffer = unsafe { &mut *self.buffer };
                let color_code = self.color_code;
                buffer.chars[self.position] = VgaEntry {
                    ascii_character: byte,
                    color_code,
                };
                self.position += 1;
            }
        }
    }

    pub fn new_line(&mut self) {

        let written = self.position % BUFFER_WIDTH;
        self.position += BUFFER_WIDTH - written;
    }

    fn scroll(&mut self) {
        unsafe {
            let buffer = &mut *self.buffer;
            // Shift the buffer one line up
            for i in 0..BUFFER_HEIGHT - 1 {
                ptr::copy_nonoverlapping(
                    buffer.chars.as_ptr().offset((i + 1) as isize * BUFFER_WIDTH as isize),
                    buffer.chars.as_mut_ptr().offset(i as isize * BUFFER_WIDTH as isize),
                    BUFFER_WIDTH,
                );
            }
            // Clear the last line
            ptr::write_bytes(buffer.chars.as_mut_ptr().offset((BUFFER_SIZE - BUFFER_WIDTH) as isize), 0, BUFFER_WIDTH);
        };
        // Move the position back one line
        self.position -= BUFFER_WIDTH;
    }
}

impl core::fmt::Write for VgaWriter {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for b in s.bytes() {
            match b {
                // printable ASCII byte or newline
                0x20..=0x7e | b'\n' => self.write_byte(b),
                // not part of printable ASCII range
                _ => self.write_byte(0xfe),
            }
        }
        Ok(())
    }
}
