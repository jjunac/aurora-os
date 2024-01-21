pub mod qemu;
#[macro_use]
pub mod panic;

pub fn success() {
    qemu::exit_qemu(qemu::QemuExitCode::Success);
}

pub fn fail() {
    qemu::exit_qemu(qemu::QemuExitCode::Failed);
}

