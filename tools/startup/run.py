#!/usr/bin/env python3

import os
import startup


def main():
    startup.register_startup(startup.connect_network,
                             lambda: not os.system("sudo miredo"))


if __name__ == "__main__":
    main()
