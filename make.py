#!/usr/bin/env python3

from os import path
from importlib.machinery import SourceFileLoader

common = SourceFileLoader("common", f"{path.dirname(__file__)}/tools/common.py").load_module()
makeutils = common.load_tool("makeutils")
buildutils = common.load_tool("buildutils")

# makeutils = SourceFileLoader("makeutils", f"{path.dirname(__file__)}/tools/makeutils.py").load_module()
# buildutils = SourceFileLoader("buildutils", f"{path.dirname(__file__)}/tools/buildutils.py").load_module()


@makeutils.recipe(aliases=["t"])
def test(ctx) -> bool:
    '''Run all the tests'''
    return makeutils.exec_child_make("test", ["aurora_kernel", "integration_tests"])

makeutils.make()
