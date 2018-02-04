#!/usr/bin/env python3

import sys
from z_tv_func import command


def main():
    argc = len(sys.argv)

    if argc > 1:
        if command.exist(sys.argv[1]):
            return command.run(sys.argv[1], (sys.argv[2:]))
        else:
            return command.run("play", (sys.argv[1:]))
    else:
        return command.run("help")


if __name__ == "__main__":
    main()
