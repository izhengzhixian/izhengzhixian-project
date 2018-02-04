#!/usr/bin/env python
#-*- coding:utf-8 -*-

import sys
import requests

span = 0
if len(sys.argv) > 1:
    span = int(sys.argv[1])

for i in range(1, 51):
    f = file(str(i) + ".json")
    send_json = f.read()
    f.close()
    print "测试第", i, "条请求:"
    print "发送数据:"
    print send_json
    req = requests.post("http://127.0.0.1:8000", data=send_json)
    if(req.status_code == 200):
        print "返回数据:"
        print req.content
    else:
        print "服务器代码出错"
        exit(1)
    span = span -1
    if span <= 0:
        tmp = raw_input()
