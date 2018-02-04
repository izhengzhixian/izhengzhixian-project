import os
import math
import urllib
import requests
from z_tv_plugin import *
from bs4 import BeautifulSoup as bs


def add_mgtv_resource(url):
    # api中，video_id是视频id，collection_id是电视剧id
    mgtv_video_api = "https://pcweb.api.mgtv.com/episode/list?" + \
               "video_id=%s&page=%d&size=%d"
    mgtv_collection_api = "https://pcweb.api.mgtv.com/episode/list?" + \
                          "collection_id=%s&page=%d&size=%d"
    tv_url_parse = urllib.parse.urlparse(url)
    url_noext = os.path.splitext(tv_url_parse.path)[0]
    url_noext_split = url_noext.split("/")
    url_noext_len = len(url_noext_split)
    # 如果是视频，则是视频id，如果是电视剧，则是电视剧id
    now_url_id = url_noext_split[-1]
    try:
        int(now_url_id)
    except ValueError:
        return False, "url不符合mgtv解析模块规则"
    if url_noext_len == 3:
        mgtv_api = mgtv_collection_api
    elif url_noext_len == 4:
        mgtv_api = mgtv_video_api
    else:
        return False, "url不符合mgtv解析模块规则"
    # 默认mgtv_api一页集数的大小，经过测试不能超过50
    default_size = 50
    # 访问的页数，第0页没用，主要为了取得集数total
    access = 0
    total = 0
    count = 1
    while True:
        r = requests.get(mgtv_api % (now_url_id, access, default_size))
        if r.status_code == 200:
            j = r.json()
            if j["code"] == 200:
                if access == 0:
                    # 总共有多少集
                    total = j["data"]["total"]
                    name = j["data"]["info"]["title"]
                    single_tv = SINGLE_TV(name)
                    single_tv.set_src(url)
                else:
                    for i in j["data"]["list"]:
                        one_set = urllib.parse.urljoin(url, i["url"])
                        single_tv.append({
                            'name': str(count),
                            'url': one_set
                        })
                        count = count + 1
                    if math.ceil(total/default_size) == access:
                        return True, single_tv
                access = access + 1
            else:
                return False, j["msg"]
        else:
            return False, "访问网址错误"


def search_mgtv_resource(word):
    base_url = "https://so.mgtv.com/so/k-" + word
    r = requests.get(base_url)
    if r.status_code != 200:
        return False, "检索失败"
    analysis = bs(r.text, "lxml")
    contents = analysis.find(class_="search-resultlist")
    options = []
    for i in \
        contents.find_all(
            class_="so-result-info search-television clearfix"):
        tv_tag_info = i.div.div.p.em.a
        tv_name = ""
        for name in tv_tag_info.strings:
            tv_name = tv_name + name
        tv_url = urllib.parse.urljoin(base_url, tv_tag_info["href"])
        options.append({
            "tv_name": tv_name,
            "tv_url": tv_url
        })
    return True, options


plugin_info = {
        "key": "mgtv",
        "add": add_mgtv_resource,
        "search": search_mgtv_resource,
}
