from dataclasses import dataclass
from importlib.machinery import SourceFileLoader
from os import path
from pathlib import Path
import logging
import re
import subprocess
import sys

common = SourceFileLoader("common", f"{path.dirname(__file__)}/common.py").load_module()


@dataclass
class Target:
    triple: str
    arch: str
    vendor: str
    os: str
    abi: str


def detect_target() -> Target:
    """Detect target with `rustc --version --verbose`"""
    rustc_info = subprocess.check_output(["rustc", "--version", "--verbose"], text=True)
    triple = re.search(r"host: (.+)", rustc_info).group(1)
    arch, vendor, os, abi = (triple.split("-", 4) + [""] * 4)[:4]
    return Target(triple, arch, vendor, os, abi)


TARGET = detect_target()
logging.info(f"Detected TARGET={TARGET}")

if TARGET.arch != "x86_64":
    logging.error(f"Unsupported target arch `{TARGET.arch}`")
    sys.exit(1)


def build_image() -> tuple[bool, dict[str, str]]:
    """
    Build the images by:
     - Running the `cargo bootimage -Zbuild-std=core,compiler_builtins -Zbuild-std-features=compiler-builtins-mem` command
     - Print the output and parse to find the image path with pattern "Created bootimage for `$image` at `$path`"
     - Returning all the images paths
    """
    is_success = True

    command = [
        "cargo",
        "bootimage",
        "-Zbuild-std=core,compiler_builtins",
        "-Zbuild-std-features=compiler-builtins-mem",
    ]
    process = subprocess.Popen(command, stdout=subprocess.PIPE, text=True)

    images = {}
    while line := process.stdout.readline():
        print(line, end="")
        if m := re.match(
            r"Created bootimage for `(?P<image>.+)` at `(?P<path>.+)`", line
        ):
            logging.info(f"Built image `{m.group('image')}` at `{m.group('path')}`")
            images[m.group("image")] = m.group("path")

    logging.info(f"Built images: {images.keys()}")
    process.wait()

    is_success = process.returncode == 0

    for image_name, image_path in images.items():
        if image_path != _compute_image_path(image_name):
            logging.error(
                f"Image path `{image_path}` does not match the expected path `{_compute_image_path(image_name)}`"
            )
            is_success = False

    return is_success


def run_image(image: str, interrupt=True, isa_debug_exit=True, display=True) -> bool:
    """
    Run the image with QEMU by:
     - Running the `qemu-system-x86_64 -drive format=raw,file=$image_path` command
    """
    image_path = _compute_image_path(image)
    logging.debug(f"Running image `{image}` at `{image_path}`")

    if not Path(image_path).exists():
        logging.error(f"Image at path `{image_path}` does not exist")
        return False

    command = ["qemu-system-x86_64", "-drive", f"format=raw,file={image_path}"]
    if isa_debug_exit:
        command += ["-device", "isa-debug-exit,iobase=0xf4,iosize=0x04"]
    if not display:
        command += ["-display", "none"]

    try:
        p = subprocess.run(command, stdout=sys.stdout, stderr=sys.stderr, check=False)
        if p.returncode == 0:
            return True
        elif p.returncode == 33:
            logging.debug("QEMU exited with code 33 (ISA debug exit)")
            if isa_debug_exit:
                return True
        logging.error(f"QEMU exited with code {p.returncode}")
        return False
    except KeyboardInterrupt:
        logging.warn("Caught KeyboardInterrupt, killing QEMU")
        return interrupt


def run_cargo(command, with_target=False) -> bool:
    """
    Run a cargo command
    """
    cargo_command = ["cargo", command]
    if with_target:
        cargo_command += ["--target", TARGET.triple]
    p = subprocess.run(cargo_command, stdout=sys.stdout, stderr=sys.stderr, check=False)
    if p.returncode == 0:
        return True
    logging.error(f"Cargo exited with code {p.returncode}")
    return False


def _compute_image_path(image_name: str) -> str:
    return f"{common.AURORA_ROOT}/target/x86_64-aurora-os/debug/bootimage-{image_name}.bin"
