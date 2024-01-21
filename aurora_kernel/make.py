#!/usr/bin/env python3

from os import path
from importlib.machinery import SourceFileLoader

common = SourceFileLoader("common", f"{path.dirname(__file__)}/../tools/common.py").load_module()
makeutils = common.load_tool("makeutils")
buildutils = common.load_tool("buildutils")

@makeutils.recipe(aliases=["b"])
def build(ctx) -> bool:
    '''Build the kernel image'''
    return buildutils.build_image()

@makeutils.recipe(aliases=["r"], deps=["build"])
def run(ctx) -> bool:
    '''Run the kernel in QEMU'''
    return buildutils.run_image("aurora_kernel")

@makeutils.recipe(aliases=["t"])
def test(ctx) -> bool:
    '''Run the unittests'''
    return buildutils.run_cargo("test", with_target=True)

makeutils.make()