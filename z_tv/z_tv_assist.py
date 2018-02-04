import math


def check_int(num):
    try:
        num = int(num)
    except ValueError:
        return False
    return True


def get_num(tip, min_num, max_num, default=None,
        extra=tuple(), pre_show=None, page_size=20):
    num_str_tuple = [str(i) for i in range(min_num, max_num+1)]
    if pre_show is not None:
        pages = math.ceil(len(pre_show)/page_size)
        local_cmd = {}
        for i in range(pages):
            local_cmd["p" + str(i+1)] = "\n" \
                    + "\n".join(pre_show[i*page_size:(i+1)*page_size]) + "\n"
        tip = "输入p1 --- p" + str(pages) + "来选页\n" + tip
        print(local_cmd["p1"])

    while True:
        option = input(tip)
        if option in extra:
            return option
        elif option == '' and default is not None:
            return default
        elif option in num_str_tuple:
            return int(option)
        elif local_cmd.get(option):
            print(local_cmd.get(option))
        else:
            print("输入错误，请重新输入")


def get_num_tuple(tip, indexs, tips, default=None, onelen=2):
    len_min = len(indexs)
    if len_min > len(tips):
        len_min = len(tips)
    indexs = indexs[0:len_min]
    tips = tips[0:len_min]
    while True:
        for i in range(len_min):
            print(indexs[i].rjust(onelen), "", tips[i])
        option = input(tip)
        if option == '' and default is not None:
            return default
        if option in indexs:
            return option
        else:
            print("输入错误，请重新输入")


def analysis_tv_name_set(arg1, arg2):
    tv_name = None
    tv_set = None
    if arg1 is not None:
        if check_int(arg1):
            tv_set = int(arg1)
        else:
            tv_name = arg1
    if arg2 is not None:
        if check_int(arg2):
            tv_set = int(arg2)
        else:
            tv_name = arg2
    return tv_name, tv_set


def myjust(string, length=0):
    leng = 0
    for s in string:
        if ord(s) <= 255:
            leng = leng + 1
        else:
            leng = leng + 2
    if length > leng:
        return string + " " * (length-leng)
    return string


class COMMAND:
    def __init__(self):
        self.function = {}
        self.help_use_len = 0
        self.help_slot = 4
        self.run_hook = []
        self.run_hook_exclude_cmd = []
    def add(self, name, func, argc_min, argc_max, 
            help_use, help_explain, help_detail=None):
        '''
            name: 命令名
            func: 函数
            argc_min: 函数需要最少参数个数
            argc_max: 函数需要最大参数个数
            help_use: 函数使用用法
            help_explain: 函数解释
            help_detail: 函数详细解释，可选
        '''
        self.function[name] = {
                    "func": func,
                    "argc_min": argc_min,
                    "argc_max": argc_max,
                    "help": {
                        'use': help_use,
                        'explain': help_explain,
                        'detail': help_detail
                        }
                }
        self.help_use_len = max((len(help_use), self.help_use_len))
        return True


    def exist(self, name):
        return bool(self.function.get(name))

    
    def set_help_slot(self, new_help_slot):
        if new_help_slot > 0:
            self.help_slot = new_help_slot
            return True
        return False


    def get_help(self, name, detail=False):
        if not self.exist(name):
            return ""
        help_ = self.function[name]['help']
        ret = help_['use']
        ret = ret.ljust(self.help_use_len + self.help_slot)
        ret = ret + help_['explain'] + "\n"
        if detail and help_['detail']:
            ret = ret + self.help_slot * " " + "详细: "
            if isinstance(help_['detail'], str):
                ret = ret + help_['detail']
            elif callable(help_['detail']):
                ret = ret + help_['detail']()
        return ret.strip()


    def help(self, name=None):
        if name is not None:
            return self.get_help(name, True)
        ret = []

        function_cmd = list(self.function)
        function_cmd.sort()
        for cmd in function_cmd:
            ret.append(self.get_help(cmd))
        return "\n".join(ret)


    def run(self, name, argv=tuple()):
        if self.exist(name):
            cmd = self.function[name]
            if cmd['argc_min'] <= len(argv) <= cmd['argc_max']:
                if name not in self.run_hook_exclude_cmd:
                    while len(self.run_hook) != 0:
                        now_run = self.run_hook.pop()
                        now_run()
                return cmd['func'](*argv)
            else:
                print("参数个数错误")
                return False
        else:
            print("错误命令")
            return False


    def add_run_hook(self, func):
        self.run_hook.append(func)
        return True


    def add_run_hook_exclude_cmd(self, cmd):
        self.run_hook_exclude_cmd.append(cmd)
        return True
