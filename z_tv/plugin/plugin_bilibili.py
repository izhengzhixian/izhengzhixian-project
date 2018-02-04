import urllib
import demjson
import requests
from z_tv_plugin import *
from z_tv_assist import *
from bs4 import BeautifulSoup as bs


def add_bilibili_resource(url):
    req = requests.get(url)
    if req.status_code != 200:
        return False, "访问网址错误"
    url = req.url
    tv_url_parse = urllib.parse.urlparse(url)
    path_split = tv_url_parse.path.split("/")
    while '' in path_split:
        path_split.remove('')

    if path_split[-2] == "play":
        center_right = req.text.split("window.__INITIAL_STATE__=")[-1]
        center = center_right.split(";(function(){", 1)[0]
        script_info = demjson.decode(center)
        single_tv = SINGLE_TV(script_info['mediaInfo']['title'])
        single_tv.set_src(url)
        for ep in script_info["epList"]:
            index = ep['index']
            index_title = ep['index_title']
            if index.isdigit():
                index = "第" + index + "话"
            set_name = index + " " + index_title
            set_name = set_name.strip()
            set_url = urllib.parse.urljoin(url,
                    '//www.bilibili.com/bangumi/play/ep' + str(ep['ep_id']))
            single_tv.append({
                'name': set_name,
                'url': set_url
                })
        return True, single_tv
    elif path_split[-2] == "anime":
        res_url = "//bangumi.bilibili.com/jsonp/seasoninfo/" \
                + path_split[-1] \
                + ".ver?callback=seasonListCallback&jsonp=jsonp" 
        res_url = urllib.parse.urljoin(url, res_url)
        req = requests.get(res_url)
        center_right = req.text.split("seasonListCallback(", 1)[-1]
        center = center_right .rsplit(");", 1)[0]
        script_info = demjson.decode(center)
        if script_info['code'] != 0:
            return False, script_info['message']
        script_info = script_info['result']
        script_info['episodes'].reverse()
        single_tv = SINGLE_TV(script_info['media']['title'])
        single_tv.set_src(url)
        for ep in script_info['episodes']:
            index = ep['index']
            index_title = ep['index_title']
            if index.isdigit():
                index = "第" + index + "话"
            set_name = index + " " + index_title
            set_name = set_name.strip()
            single_tv.append({
                'name': set_name,
                'url': urllib.parse.urljoin(url,
                    '//www.bilibili.com/bangumi/play/ep' + ep['episode_id'])
                })
        return True, single_tv
    else:
        return False, 'bilibili模块不能处理url'


def search_bilibili_resource(word):
    type_indexs = ("0", "1")
    type_tips = ("番剧", "影视")
    url_types = ("bangumi", "pgc")
    index = get_num_tuple("请选择搜索类型(默认为0):) ",
            type_indexs, type_tips, "0")
    url_type = url_types[int(index)]
    req_url = "https://search.bilibili.com/" \
           + url_type + "?keyword=" + word
    req = requests.get(req_url)
    if req.status_code != 200:
        return False, "网址访问错误"
    analysis = bs(req.text, "lxml")
    options = []
    root = analysis.find(class_="ajax-render")
    if url_type == "bangumi":
        items = root.find_all(class_="synthetical")
        for item in items:
            title = item.find(class_="title")["title"]
            for tinyitem in item.find(class_="so-episode").find_all("a"):
                options.append({
                    "tv_name": title + " " + tinyitem["title"],
                    "tv_url": urllib.parse.urljoin(req_url, tinyitem["href"])
                    })
    elif url_type == "pgc":
        items = root.find_all(class_="movie-item")
        for item in items:
            options.append({
                "tv_name": item.a["title"],
                "tv_url": urllib.parse.urljoin(req_url, item.a["href"])
                })

    return True, options


plugin_info = {
        "key": "bilibili",
        "add": add_bilibili_resource,
        "search": search_bilibili_resource,
        }
