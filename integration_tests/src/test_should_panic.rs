#![no_main]
#![feature(abi_x86_interrupt)]
#![no_std]
#[allow(dead_code)]

mod testutils;
use crate::testutils::*;

use aurora_kernel::Kernel;

#[no_mangle]
pub extern "C" fn _start() {
    Kernel::instance().init();

    expect_panic!("Kernel panic!", {
        panic!("Kernel panic!");
    });

    fail();
}
