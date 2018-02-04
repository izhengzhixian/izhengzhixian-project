import math
import time
import urllib
import demjson
import calendar
import requests
from z_tv_plugin import *
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


def get_label(item):
    label = item.find(class_="l_serial")
    if label is not None:
        return int(label.label.string)
    label = item.find(class_="serial")
    if label is not None:
        return int(label.label.string)
    label = item.find(class_="sn_num")
    if label is None:
        return int(label.string)
    return None


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
    r = requests.get(from_to_api)
    if r.status_code != 200:
        return False
    ret = []
    html = demjson.decode(r.text.split("zzxflag", 2)[-1][1:-2])['html']
    analysis = bs(html, 'lxml')
    items = analysis.find(class_="lists").find(class_="items")
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


def auxiliary_common(Drama, pageConfig):
    firstMon = int(pageConfig['page']['firstMon'])
    lastMon = int(pageConfig['page']['lastMon'])
    year = int(pageConfig['page']['year'])
    pageType = pageConfig['page']['type']
    total = int(pageConfig['page']['totalepisodes'])
    ret = []
    reverse = False
    if pageType == "Num":
        if total <= 100:
            first = True
            items = Drama.find(class_="lists").find(class_="items")
            for item in items.find_all(class_="item"):
                if item.a is None:
                    continue
                if first is True:
                    label = get_label(item)
                    if label is not None and label != 1:
                        reverse = True
                    first = False
                one_set = urllib.parse.urljoin(pageConfig['url'],
                                               item.a["href"])
                ret.append({
                    'name': get_name(item),
                    'url': one_set
                })
        else:
            for page in range(1, math.ceil(total/100) + 1):
                page_ret = load_num_api(pageConfig, str(page))
                if page_ret is not False:
                    ret.extend(page_ret)
    elif pageType == "Date":
        for month in range(firstMon, lastMon+1):
            days = calendar.monthrange(year, month)[1]
            start = str(year) + str(month).rjust(2, "0") + "01"
            end = str(year) + str(month).rjust(2, "0") + str(days)
            month_ret = load_date_api(pageConfig,
                                      start, end, str(month))
            if month_ret is not False:
                ret.extend(month_ret)
    elif pageType == "Scroll":
        return False, "不能处理"
        data = {
                "steal_params": {
                    "ccode": "0502",
                    "client_ip": "192.168.1.1",
                    "utid": "g0v0EpFiiSYCAbZ/gNEIqAha",
                    "client_ts": int(time.time()),
                    "ckey": "106# YoBKbB BnLB2CKaBBBBBpZb54QZo0Yi9PrYo0Zc5W4YyTii74hZk0Z15PHZoTYs9JkKBBgi4gLsISDm/8SKBB7h4gxVl7s/BCBE1Zzz0qmb1ZHi4gc0ylmh5IDvl1VbyloN1Zzciqmbyf7i4gc0ylmhJELKBmdBtJFv4NEsk4xIStd92fyi6RWbsYTs7R9qkc85Bfzx Ky9sFLRKQzFpegeaZiLmbp1q7m9BNySPboB3I7c7TD1wkDUNLD 9otcNLV4 N0JUy74 aQg0bzyl8QUNLD 9otci4AMnBB2wyg2t8zImCjcqkDMbffRBCBA1Zyz0qmbXCoBK4 FDUb1BCBoQ2oBBXyKBBfBQZKC#0",
                    },
                "biz_params": {
                    "vid": pageConfig["videoId2"],
                    "playlist_id": pageConfig["folderId"],
                    "ob": "1",
                    },
                "ad_params": {
                    "site": 1,
                    "wintype": "interior",
                    "p": 1,
                    "fu": 0,
                    "vs": "1.0",
                    "rst": "mp4",
                        "dq": "flv",
                        "os": "linux",
                        "osv": "",
                        "d": pageConfig["folderId"],
                        "bt": "",
                        "aw": "w",
                        "needbf": 1
                        }
                    
                    }
        payload = {
                "jsv": "2.4.11",
                "appKey": "24679788",
                "t": int(time.time() * 1000),
                "sign": "f794a94d83cf45ef8a199899d6e25920",
                "api": "mtop.youku.play.ups.appinfo.get",
                "v": "1.1",
                "timeout": 20000,
                "YKPid": "20160317PLF000211",
                "YKLoginRequest": True,
                "type": "jsonp",
                "dataType": "jsonp",
                "callback": "mtopjsonp1",
                #"data": urllib.parse.urlencode(demjson.encode(data))
                "data": demjson.encode(data)
                }
        req = requests.get(
                "http://acs.youku.com/h5/mtop.youku.play.ups.appinfo.get/1.1/",
                params=payload)
        print(req.url)
        print(req.status_code)
        print(req.text)
        exit(1)
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
        base_info = analysis.find(class_="base_info")
        tag = base_info.h1.a.string[1:-1]
        Drama = analysis.find(id="Drama")
        if Drama is None:
            return False, "没有列表资源"
        name = Drama.div.h3.string
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
        single_tv = SINGLE_TV(name)
        single_tv.set_src(url)
        single_tv.set_tag(tag)
        auxiliary = tag_map.get(tag)
        if auxiliary is None:
            return False, "视频标签处理模块错误"
        ret = auxiliary(Drama, pageConfig)
        if ret[0] is False:
            return ret[0], ret[1]
        single_tv.extend(ret[1])
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
