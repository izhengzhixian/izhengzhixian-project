#!/usr/bin/env python3

import os
import time
import platform
from threading import Thread


def connect_network():
    if platform.system() == "Windows":
        ping_cmd = "ping 8.8.8.8"
    else:
        ping_cmd = "ping 8.8.8.8 -c 4"
    ping_ret = os.system(ping_cmd)
    return not ping_ret


def add_thread(bool_func, run_func, interval, count, not_bool):
    while count != 0:
        bool_ret = bool_func()
        if not_bool:
            bool_ret = not bool_ret
        if bool_ret:
            return run_func()
        count = count - 1
        time.sleep(interval)
    return False


def register_startup(bool_func, run_func, interval=1,
                     count=-1, not_bool=False):
    t = Thread(target=add_thread,
               args=(bool_func, run_func, interval, count, not_bool))
    t.start()
