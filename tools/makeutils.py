from argparse import Namespace
from dataclasses import dataclass
import functools
from importlib.machinery import SourceFileLoader

import logging
from os import path
from pathlib import Path
import subprocess
import sys
import traceback


common = SourceFileLoader("common", f"{path.dirname(__file__)}/common.py").load_module()


_recipes: dict[str, "Recipe"] = {}


@dataclass
class Recipe:
    name: str
    description: str
    func: callable
    aliases: list[str]
    deps: list[str]

    def execute(self, args, dep_for=None):
        HEADER_WIDTH = 100

        def log_header(log_func, msg="", footer=False):
            # TODO: output path of the make.py that was called
            exec_path = _get_caller_make_dir()
            exec_path = exec_path.removeprefix(common.AURORA_ROOT).removeprefix('/./').removesuffix('/.')
            exec_path = exec_path if exec_path else "aurora-os"
            header = f"[{self.name}] {exec_path}{f' {msg}' if msg else ''}{f' (depedency for {dep_for})' if dep_for else ''}"
            padding_len = (HEADER_WIDTH - 2 - len(header)) // 2
            if not footer:
                log_func("")
            log_func(f"#{'=' * (HEADER_WIDTH - 2)}#")
            log_func(
                f"#{' ' * padding_len}{header}{' ' * (padding_len + len(header)%2)}#"
            )
            log_func(f"#{'=' * (HEADER_WIDTH - 2)}#")
            if footer:
                log_func("")

        is_success = True

        for d in self.deps:
            is_success = _recipes[d].execute(args, dep_for=self.name)
            if not is_success:
                break

        if is_success:
            log_header(logging.info)
            is_success = self.func(args)

        if is_success:
            log_header(logging.info, f"PASSED", footer=True)
        else:
            log_header(logging.error, f"FAILED", footer=True)

        return is_success


def recipe(aliases=[], deps=[]):
    def recipe_decorator(func: callable):
        @functools.wraps(func)
        def recipe_wrapper(*args, **kwargs):
            return func(*args, **kwargs)

        _recipes[func.__name__] = Recipe(
            func.__name__, func.__doc__, recipe_wrapper, aliases=aliases, deps=deps
        )
        logging.debug(f"Registered recipe `{func.__name__}`")
        return recipe_wrapper

    return recipe_decorator


def make():
    import argparse

    # Custom help formatter to remove the metavar from the subcommands list
    # https://stackoverflow.com/a/13429281
    class SubcommandHelpFormatter(argparse.RawDescriptionHelpFormatter):
        def _format_action(self, action):
            parts = super(argparse.RawDescriptionHelpFormatter, self)._format_action(
                action
            )
            if action.nargs == argparse.PARSER:
                parts = "\n".join(parts.split("\n")[1:])
            return parts

    parser = argparse.ArgumentParser(
        description="AURORA OS build system", formatter_class=SubcommandHelpFormatter
    )
    parser.add_argument(
        "-v", "--verbose", action="store_true", help="Enable verbose logging"
    )

    recipe_subparsers = parser.add_subparsers(
        title="recipe", dest="recipe", required=True, metavar="recipe"
    )

    for _, recipe in _recipes.items():
        recipe_parser = recipe_subparsers.add_parser(
            recipe.name, help=recipe.description, aliases=recipe.aliases
        )
        recipe_parser.set_defaults(func=recipe.execute)

    recipe_subparsers.add_parser(
        "help", help="Show this help message and exit"
    ).set_defaults(func=lambda _: parser.print_help())

    args = parser.parse_args()

    if args.verbose:
        logging.getLogger().setLevel(logging.DEBUG)

    if args.recipe is None:
        parser.print_help()
        return

    is_success = args.func(Namespace(args=args))
    exit(0 if is_success else 1)


def exec_child_make(command, paths) -> bool:
    """Execute child make.py"""
    current_make_dir = _get_caller_make_dir()

    for p in paths:
        make_path = path.join(current_make_dir, p, "make.py")
        if not path.isfile(make_path):
            logging.error(f"Child make.py not found at `{make_path}`")
            return False
        logging.info(f"Executing child make.py at `{make_path}`")
        process = subprocess.Popen(
            [make_path, command],
            cwd=path.dirname(make_path),
            stdout=sys.stdout,
            stderr=sys.stderr,
        )
        process.wait()
        if process.returncode != 0:
            logging.error(
                f"Child make.py at `{make_path}` exited with code {process.returncode}"
            )
            return False

    return True


def _get_caller_make_dir():
    # Find the last frame where the filename is not makeutils.py
    for frame in reversed(traceback.extract_stack()):
        if frame.filename != __file__:
            return path.dirname(frame.filename)
