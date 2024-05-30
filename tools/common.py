from importlib.machinery import SourceFileLoader
import logging
from os import path
from pathlib import Path

class Colors:
    """ ANSI color codes """
    BLACK = "\033[30m"
    RED = "\033[31m"
    GREEN = "\033[32m"
    BROWN = "\033[33m"
    BLUE = "\033[34m"
    PURPLE = "\033[35m"
    CYAN = "\033[36m"
    LIGHT_GRAY = "\033[37m"
    DARK_GRAY = "\033[90m"
    LIGHT_RED = "\033[91m"
    LIGHT_GREEN = "\033[92m"
    YELLOW = "\033[93m"
    LIGHT_BLUE = "\033[94m"
    LIGHT_PURPLE = "\033[95m"
    LIGHT_CYAN = "\033[96m"
    LIGHT_WHITE = "\033[97m"
    BOLD = "\033[1m"
    FAINT = "\033[2m"
    ITALIC = "\033[3m"
    UNDERLINE = "\033[4m"
    BLINK = "\033[5m"
    NEGATIVE = "\033[7m"
    CROSSED = "\033[9m"
    RESET = "\033[0m"
    # cancel SGR codes if we don't write to a terminal
    if not __import__("sys").stdout.isatty():
        for _ in dir():
            if isinstance(_, str) and _[0] != "_":
                locals()[_] = ""
    else:
        # set Windows console in VT mode
        if __import__("platform").system() == "Windows":
            kernel32 = __import__("ctypes").windll.kernel32
            kernel32.SetConsoleMode(kernel32.GetStdHandle(-11), 7)
            del kernel32

class ColoredFormatter(logging.Formatter):
    COLORS = {
        "INFO": Colors.LIGHT_CYAN,
        "WARNING": Colors.YELLOW,
        "ERROR": Colors.LIGHT_RED,
        "CRITICAL": Colors.BOLD + Colors.LIGHT_RED,
    }

    def __init__(self):
        logging.Formatter.__init__(
            self,
            "%(asctime)s.%(msecs)03d | %(levelname)-8s | %(module)-16s | %(message)s",
            datefmt="%H:%M:%S",
        )

    def format(self, record):
        return (
            ColoredFormatter.COLORS.get(
                record.levelname, Colors.RESET
            )
            + logging.Formatter.format(self, record)
            + Colors.RESET
        )


logging.basicConfig(level=logging.INFO)
logging.getLogger().handlers[0].setFormatter(ColoredFormatter())

AURORA_ROOT: str = str(Path(__file__).parent.parent.resolve())
logging.info(f"Detected AURORA_ROOT={AURORA_ROOT}")


def load_tool(name):
    return SourceFileLoader(
        name, f"{path.dirname(__file__)}/{name}.py"
    ).load_module()
