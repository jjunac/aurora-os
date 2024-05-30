#!/usr/bin/env python3

from glob import glob
import logging
from os import path
from importlib.machinery import SourceFileLoader
import time
import tomllib

common = SourceFileLoader("common", f"{path.dirname(__file__)}/../tools/common.py").load_module()
makeutils = common.load_tool("makeutils")
buildutils = common.load_tool("buildutils")

TEST_ROOT = path.dirname(__file__)

def get_cargo_config():
    with open(f"{TEST_ROOT}/Cargo.toml", "rb") as f:
        return tomllib.load(f)

def harness_tests() -> tuple[set[str], set[str]]:
    test_paths_from_cfg = {t["path"] for t in get_cargo_config()["bin"]}
    test_paths = {t.replace(f"{TEST_ROOT}/", "") for t in glob(f"{TEST_ROOT}/src/**/test_*.rs", recursive=True)}

    logging.info(f"Tests registered in Cargo.toml: {test_paths_from_cfg}")
    logging.info(f"Tests found: {test_paths}")

    for t in test_paths.difference(test_paths_from_cfg):
        logging.warning(f"Test `{t}` is not registered in Cargo.toml")

    return test_paths_from_cfg, test_paths

@makeutils.recipe(aliases=["b"])
def build(ctx) -> bool:
    '''Build the integration tests'''
    is_success = buildutils.build_image()
    # This will emit a warning if a test is not registered in Cargo.toml
    test_paths_from_cfg, test_paths = harness_tests()
    return is_success

@makeutils.recipe(aliases=["t"], deps=["build"])
def test(ctx) -> bool:
    '''Run the integration tests'''

    test_paths_from_cfg, test_paths = harness_tests()

    failed_tests = []

    start_time = time.time()
    logging.info(f"[==========] Running {len(test_paths_from_cfg)} integration tests")

    for t in test_paths_from_cfg:
        logging.info(f"[ RUN      ] {t}")
        if t not in test_paths:
            logging.error(f"File `{t}` is registered in Cargo.toml but does not exist in src")
            failed_tests.append(t)
            continue

        is_success = buildutils.run_image(path.basename(t)[:-3], display=False)

        if is_success:
            logging.info(f"[       OK ] {t}")
        else:
            logging.error(f"[  FAILED  ] {t}")
            failed_tests.append(t)

    logging.info(f"[==========] {len(test_paths_from_cfg)} integration tests ({time.time() - start_time:.3f}s total)")
    logging.info(f"[  PASSED  ] {len(test_paths_from_cfg) - len(failed_tests)} tests")
    if ingored := test_paths.difference(test_paths_from_cfg):
        logging.warning(f"[  IGNORED ] {len(ingored)} tests, listed below:")
        for t in ingored:
            logging.warning(f"[  IGNORED ] {t}")
    if failed_tests:
        logging.error(f"[  FAILED  ] {len(failed_tests)} tests, listed below:")
        for t in failed_tests:
            logging.error(f"[  FAILED  ] {t}")

    return len(failed_tests) == 0

makeutils.make()