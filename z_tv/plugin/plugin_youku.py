import math
import time
import urllib
import demjson
import calendar
import requests
from z_tv_plugin import *
from z_tv_assist import *
from bs4 import BeautifulSoup as bs


def get_name(item):
    name = item.get("title")
    if name is not None:
        return name
    name = item.find(class_="l_title")
    if name is not None:
        name = name.string
    else:
        name = item.div.div['title']
    return name


def load_date_api(pageConfig, start, end, page):
    from_to_api = "http://v.youku.com/page/playlist/pm_" \
                  + pageConfig["playmode"] + "_vid_" + pageConfig["videoId"] \
                  + "_showid_" + pageConfig["showid"] + "_datefrom_" + start \
                  + "_dateto_" + end + "_lastmon_" \
                  + pageConfig["page"]["lastMon"] \
                  + "_page_" + page + "_pid_" + pageConfig["pid"] \
                  + "?beta&callback=zzxflag"

    r = requests.get(from_to_api)
    if r.status_code != 200:
        return False
    ret = []
    html = demjson.decode(r.text.split("zzxflag", 2)[-1][1:-2])['html']
    analysis = bs(html, 'lxml')
    for item in analysis.find(class_="showlists").find_all(class_="item"):
        if item.a is None:
            continue
        one_set = urllib.parse.urljoin(pageConfig['url'], item.a["href"])
        ret.append({
            'name': get_name(item),
            'url': one_set
        })
    return ret


def load_num_api(pageConfig, page):
    from_to_api = "http://v.youku.com/page/playlist/pm_" \
                  + pageConfig["playmode"] + "_vid_" \
                  + pageConfig["videoId"] + "_showid_" \
                  + pageConfig["showid"] + "_page_" + page \
                  + "?beta&callback=zzxflag"
    from_to_api_sec = " http://v.youku.com/page/playlist?&pm=" \
            + pageConfig["playmode"] + "&vid=" + pageConfig["videoId"] \
            + "&fid=" + pageConfig["folderId"] \
            + "&showid=" + pageConfig["showid"] \
            + "&sid=0&page=" + page + "&callback=zzxflag"
    r = requests.get(from_to_api)
    if r.status_code != 200:
        return False
    callback = demjson.decode(r.text.split("zzxflag", 2)[-1][1:-2])
    if callback.get("html") is None:
        r = requests.get(from_to_api_sec)
        if r.status_code != 200:
            return False
        callback = demjson.decode(r.text.split("zzxflag", 2)[-1][1:-2])
        if callback.get("html") is None:
            return False
    html = callback['html']
    analysis = bs(html, 'lxml')
    items = analysis
    ret = []
    for item in items.find_all(class_="item"):
        if item.a is None:
            continue
        one_set = urllib.parse.urljoin(pageConfig['url'],
                                       item.a["href"])
        ret.append({
            'name': get_name(item),
            'url': one_set
        })
    return ret


def auxiliary_common(Drama, pageConfig, reverse=False):
    firstMon = int(pageConfig['page']['firstMon'])
    lastMon = int(pageConfig['page']['lastMon'])
    year = int(pageConfig['page']['year'])
    pageType = pageConfig['page']['type']
    total = int(pageConfig['page']['totalepisodes'])
    ret = []
    if pageType == "Num":
        tmp_total = total
        page = 1
        while tmp_total > 0 and page <= total:
            page_ret = load_num_api(pageConfig, str(page))
            if page_ret is not False:
                ret.extend(page_ret)
                tmp_total = tmp_total - len(page_ret)
            page = page + 1
    elif pageType == "Date":
        for month in range(firstMon, lastMon+1):
            days = calendar.monthrange(year, month)[1]
            start = str(year) + str(month).rjust(2, "0") + "01"
            end = str(year) + str(month).rjust(2, "0") + str(days)
            month_ret = load_date_api(pageConfig,
                                      start, end, str(month))
            if month_ret is not False:
                ret.extend(month_ret)
    else:
        return False, "页面类型错误"
    if reverse:
        ret.reverse()
    return True, ret


tag_map = {
        '综艺': auxiliary_common,
        '动漫': auxiliary_common,
        '剧集': auxiliary_common,
        '电影': auxiliary_common,
        '电视剧': auxiliary_common,
}


def add_youku_resource(url):
    r = requests.get(url)
    if r.status_code == 200:
        analysis = bs(r.text, "lxml")
        tvinfo = analysis.find(class_="tvinfo")
        Drama = analysis.find(id="Drama")
        if Drama is None:
            return False, "没有列表资源"
        name = tvinfo.a.string
        if name is None:
            name = Drama.find(class_="singer").string
        # 通过网页的标签的一个脚本的得到pageConfig
        script_text = analysis.find(class_="g-footer")\
                              .next_sibling.next_sibling.string
        pageConfig = script_text.split("PageConfig")[1]
        unquote_str = urllib.parse.unquote(pageConfig[pageConfig.find("{"):-1])
        pageConfig = demjson.decode(unquote_str)
        # 结束
        pageConfig['url'] = url
        tag = pageConfig['catName']
        single_tv = SINGLE_TV(name)
        single_tv.set_src(url)
        single_tv.set_tag(tag)
        auxiliary = tag_map.get(tag)
        if auxiliary is None:
            return False, "优酷视频标签处理模块错误"
        save_single_tv = None
        if global_tv_info is not None:
            save_single_tv = global_tv_info.get_single_tv(name)
        if save_single_tv is not None:
            reverse = save_single_tv.get_reverse()
        else:
            reverse = get_num(
                    "请输入目录是否逆序(0正序，1逆序，默认0)：",
                    0, 1, default=0)
        ret = auxiliary(Drama, pageConfig, reverse)
        if ret[0] is False:
            return ret[0], ret[1]
        single_tv.extend(ret[1])
        single_tv.set_reverse(reverse)
        return True, single_tv
    return False, "访问网址错误"


def search_youku_resource(word):
    base_url = "http://www.soku.com/search_video/q_" + word
    r = requests.get(base_url)
    if r.status_code == 200:
        analysis = bs(r.text, "lxml")
        contents = analysis.find(class_="DIR")
        options = []
        for i in contents.find_all(class_="s_detail"):
            base_name = i.find(class_="base_name")
            if base_name is None:
                continue
            tv_tag_info = base_name.a
            tv_name = tv_tag_info["_log_title"]
            tv_url = urllib.parse.urljoin(base_url, tv_tag_info["href"])
            options.append({
                "tv_name": tv_name,
                "tv_url": tv_url
            })
        return True, options
    else:
        return False, "检索失败"


plugin_info = {
        "key": "youku",
        "add": add_youku_resource,
        "search": search_youku_resource,
}
