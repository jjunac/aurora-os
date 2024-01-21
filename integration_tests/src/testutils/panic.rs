
pub static mut _EXPECTED_PANIC: Option<&str> = None;

#[macro_export]
macro_rules! expect_panic {
    ($expected_panic:expr, $block:block) => {
        unsafe { $crate::testutils::panic::_EXPECTED_PANIC = Some($expected_panic) };
        let _result = (|| $block)();
        unsafe { $crate::testutils::panic::_EXPECTED_PANIC = None };
        // _result
    };
}

#[cfg(not(test))]
use core::panic::PanicInfo;

#[cfg(not(test))]
#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    use crate::testutils::qemu;
    match unsafe { _EXPECTED_PANIC } {
        Some(_expected_panic) => {
            // TODO: check if panic message matches expected panic
            qemu::exit_qemu(qemu::QemuExitCode::Success);
        }
        None => {
            qemu::exit_qemu(qemu::QemuExitCode::Failed);
        }
    }
    loop {}
}