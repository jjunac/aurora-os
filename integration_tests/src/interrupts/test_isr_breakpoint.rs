#![no_main]
#![feature(abi_x86_interrupt)]
#![no_std]
#[allow(dead_code)]

#[path = "../testutils/mod.rs"]
mod testutils;
use crate::testutils::*;

use aurora_kernel::Kernel;

#[no_mangle]
pub extern "C" fn _start() {
    Kernel::instance().init();

    x86_64::instructions::interrupts::int3();

    success();
}
