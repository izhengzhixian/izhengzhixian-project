#!/usr/bin/env python3

import os
import sys
import json
import urllib
import requests
import importlib
from z_tv_assist import *
import z_tv_plugin
from z_tv_plugin import *


def play(tv_name=None, tv_set=None):
    tv_name, tv_set = analysis_tv_name_set(tv_name, tv_set)
    ret = tv_info.play(tv_name, tv_set)
    if ret[0] is False:
        print(ret[1])
        return False
    else:
        print(ret[1])
        return True


def add_resource(url, update=False):
    global tv_info
    tv_url_parse = urllib.parse.urlparse(url)
    if tv_url_parse.hostname == "":
        print("无效url")
        return False
    for host_tag in add_map.keys():
        if host_tag in tv_url_parse.hostname:
            ret = add_map[host_tag](url)
            if ret[0] is False:
                print(ret[1])
                return False
            else:
                name = ret[1].name
                if update:
                    print("更新《" + name + "》成功")
                else:
                    print("添加《" + name + "》成功")
                single_tv = tv_info.get_single_tv(name)
                if single_tv is not None:
                    tv_set = single_tv.get_save()
                    ret[1].set_save( tv_set )
                tv_info.update(ret[1])
                tv_info.save()
                return True
    print("没有合适模块对网址进行解析")
    return True


def search_resource(word, site=None):
    if len(search_map) == 0:
        print("没有搜索任何网站的插件")
        return True
    if site is None:
        pre_show = []
        sites = tuple(search_map)
        for index, site in enumerate(sites):
            pre_show.append(str(index).rjust(2) + "  " + site)
        option = get_num("选择搜索站点(0-" + str(len(sites)-1) \
                + ", 默认为0)：", 0, len(sites)-1, 0,
                pre_show=pre_show)
        site = sites[option]
    print("当前使用" + site)
    ret = search_map.get(site)
    if ret is None:
        print("没有此站点")
        return False
    else:
        search_ret = ret(word)
    if search_ret[0] is False:
        print(search_ret[1])
        return False
    options = search_ret[1]
    count = len(options)
    if count == 0:
        print("没有搜索结果")
        return True
    pre_show = []
    for i in range(count):
        pre_show.append("选择: " + str(i+1).rjust(2) \
                + "    " + options[i]["tv_name"])
    option = get_num( "请输入选择(0-" + str(count) + \
            ", 0表示退出, 默认为0): ", 0, count, 0,
            pre_show=pre_show)
    if option == 0:
        return True
    else:
        ret = options[option-1]
        return add_resource(ret["tv_url"])


def list_tv_info():
    count = 1
    single_tv_list = tv_info.get_single_tv_list()
    pre_show = []
    for single_tv in single_tv_list:
        total = single_tv.get_total()
        pre_show.append(str(count).rjust(2) + "  " + myjust(single_tv.name, 30) \
                + "总集数: " + str(total))
        count = count + 1
    option = get_num( "请输入选择(0-" + str(count-1) + \
            ", 0表示退出，默认为0): ", 0, count-1, 0,
            pre_show=pre_show)
    if option == 0:
        return True
    else:
        single_tv = single_tv_list[option-1]
        total = single_tv.get_total()
        pre_show = []
        for index, item in enumerate(single_tv.get_list(), 1):
            pre_show.append(str(index).rjust(3)+": " + item['name'])
        while True:
            option = get_num( "请输入选择(0-" + str(total) + \
                    ", 0表示退出，默认为" + \
                    str(single_tv.get_save()) + "): ",
                    0, total, single_tv.get_save(), pre_show=pre_show)
            if option == 0:
                return True
            else:
                return play(single_tv.name, option)


def delete_tv_info(name=None):
    if name is not None:
        single_tv = tv_info.delete(name)
        if single_tv is False:
            print("本地没有该资源，删除错误")
            return False
        else:
            return True
    count = 1
    single_tv_list = tv_info.get_single_tv_list()
    pre_show = []
    for single_tv in single_tv_list:
        total = single_tv.get_total()
        pre_show.append(str(count).rjust(2) + " " + myjust(single_tv.name, 30) \
                + "总集数: " + str(total))
        count = count + 1
    option = get_num("请输入选择(0-" + str(count-1) + \
            ", 0表示退出，默认为0): ", 0, count-1, 0,
            pre_show=pre_show)
    if option == 0:
        return True
    else:
        single_tv = single_tv_list[option-1]
        tv_name = single_tv.name
        tv_info.delete(tv_name)
        tv_info.save()
        return True


def delete_tv_info_all():
    tv_info.delete_all()
    tv_info.save()


def update_all(name=None):
    if name is not None:
        single_tv = tv_info.get_single_tv(name)
        if single_tv is None:
            print("本地没有该电视剧资源")
            return False
        else:
            return add_resource(single_tv.get_src(), update=True)
    else:
        for single_tv in tv_info.get_single_tv_list():
            add_resource(single_tv.get_src(), update=True)
        return True


def next_info(tv_name=None, tv_set=None):
    tv_name, tv_set = analysis_tv_name_set(tv_name, tv_set)
    ret = tv_info.get_next_info(tv_name, tv_set)
    if ret[0] is False:
        print(ret[1])
        return False
    print("接下来播放<<" + ret[1] + ">>第" + str(ret[2]) + "集")
    print("name: " + ret[3]['name'])
    print("url: " + ret[3]['url'])
    return True


def scan_plugin():
    global add_map
    global search_map
    root_dir = os.path.dirname(os.path.realpath(__file__))
    for file_name in os.listdir(os.path.join(root_dir, "plugin")):
        if file_name.startswith("plugin_") and \
           file_name.endswith(".py"):
            plugin = importlib.import_module("plugin." \
                    + os.path.splitext(file_name)[0])
            if 'plugin_info' in dir(plugin):
                plugin_info = plugin.plugin_info
                if not isinstance(plugin_info, dict):
                    continue
                key = plugin_info.get('key')
                if not isinstance(key, str):
                    continue
                add = plugin_info.get('add')
                if callable(add):
                    add_map[key] = add
                search = plugin_info.get('search')
                if callable(search):
                    search_map[key] = search


def help_info(cmd=None):
    print(command.help(cmd))
    return True


tv_info_path = os.path.expanduser("~/.config/z-kits/z-tv.info")
tv_info_dir = os.path.dirname(tv_info_path)
if not os.path.isdir(tv_info_dir):
    os.makedirs(tv_info_dir)
if os.path.isfile(tv_info_path):
    tv_info = TV_INFO(tv_info_path)
else:
    tv_info = TV_INFO()
    tv_info.set_path(tv_info_path)
    tv_info.save()

z_tv_plugin.global_tv_info = tv_info


add_map = {}
search_map = {}


command = COMMAND()
command.add("add", add_resource, 1, 1, 
        "add_URL", "添加一个资源")
command.add("search", search_resource, 1, 2,
        "search WORD [SITE]", 
        "通过关键词搜索资源",
        lambda : "可通过指定" + " ".join(tuple(search_map)) + "来搜索，")
command.add("list", list_tv_info, 0, 0, 
        "list", "列出资源，手动选择下次播放的资源")
command.add("delete", delete_tv_info, 0, 1,
        "delete [name]", 
        "删除本地资源，默认列出资源选择，也可以用参数指定资源名")
command.add("delete-all", next_info, 0, 0,
        "delete-all", 
        "删除所有本地资源")
command.add("update", update_all, 0, 1,
        "update [name]", 
        "更新本地资源，默认更新所有，也可以用参数指定资源名更新")
command.add("next", next_info, 0, 2,
        "next [name] [set]", 
        "显示下次播放资源信息，默认使用保存信息，课手动指定")
command.add("help", help_info, 0, 1,
        "help [command]", 
        "默认显示所有命令帮助，可通过指定命令来查看详细信息")
command.add("play", play, 0, 2,
        "[play] [name] [set]",
        "播放本地资源的，如果没有指定，则使用保存信息",
        "其中，set为正整数，则手动指定集数，如果非正，播放当前集的偏移\n" \
                + "比如，-1为，默认播放集的前一集")

command.add_run_hook(scan_plugin)
command.add_run_hook_exclude_cmd("play")
