#!/usr/bin/env python2
#-*- coding:utf-8 -*-
import json
import requests
import ttk
from Tkinter import *

identify = None
#server_api = "http://127.0.0.1:8000"
server_api = "http://www.free521.cn:12345"
master = None


def logout():
    global identify
    identify = None
    if master:
        master.destroy()
    CCMSClient()

def post_api(post_json):
    adata=json.dumps(post_json)
    try:
        req = requests.post(server_api, data=json.dumps(post_json))
        req_error = False
    except:
        req_error = True
    if req_error:
        return (False, "server died")
    if req.status_code != 200:
        return (False, "server error")
    try:
        ret = req.json()
    except:
        req_error = True
    if req_error:
        return (False, "server return no json data")
    return (True, ret)
    

class CCMSClient:
    def __init__(self):
        self.identify = None
        self.api = server_api
        global master
        global identify
        self.master= Tk()
        master = self.master
        self.master.title("ccmslogin")
        self.master.geometry("800x600")
        frame1 = LabelFrame(self.master, text="login")
        self.username = StringVar()
        self.password = StringVar()
        self.login_str = StringVar()
        login_tip = Label(frame1, textvariable=self.login_str, fg="red")
        login_tip.grid(row=0, column=1, sticky=E, padx=10, pady=5)
        Label(frame1, text="username:").grid(row=1, column=0)
        Label(frame1, text="password:").grid(row=2, column=0)
        e1 = Entry(frame1, textvariable=self.username)
        e2 = Entry(frame1, textvariable=self.password, show="*")
        e1.grid(row=1, column=1, padx=10, pady=10)
        e2.grid(row=2, column=1, padx=10, pady=10)

        Button(frame1, text="login", width=10, command=self.login)\
            .grid(row=3, column=0, padx=10, pady=10)

        Button(frame1, text="exit", width=10, command=self.master.quit)\
            .grid(row=3, column=1, sticky=E, padx=10, pady=10)

        
        frame1.pack(padx=10, pady=10, expand=1)

        mainloop()

    def login(self):
        post_data = {'action': 'login',
                     'values': {'username': self.username.get(),
                                'password': self.password.get()}}
        ok, ret_json = post_api(post_data)
        if ok:
            if(ret_json['code'] == 0):
                self.identify = ret_json['values']['identify']
            else:
                tip_str = ret_json['message']
        else:
            tip_str = ret_json
        if(self.identify is None):
            self.login_str.set(tip_str)
        else:
            global identify
            identify = self.identify
            self.master.destroy()
            User_show()

class User_show:
    def __init__(self):
        global master
        self.master= Tk()
        master = self.master
        self.master.title("ccms  " + identify['position'])
        self.master.geometry("1024x768")
        self.frame_left = Frame(self.master)
        self.frame_right= Frame(self.master)
        self.frame_right_view = None
        self.table_view = None
        self.now_func = StringVar()
        Label(self.frame_left, text=identify['position'].capitalize(), width=15,
              height=2).pack(pady=20)
        self.function = {
            '学生开户': (self.func_kaihu, self.post_kaihu),
            '学生卡挂失': (self.func_guashi, self.post_guashi),
            '学生卡补卡': (self.func_buka, self.post_buka),
            '学生注销': (self.func_zhuxiao, self.post_zhuxiao),
            '学生卡消费': (self.func_consume, self.post_consume),
            '学生卡充值': (self.func_chongzhi, self.post_chongzhi),
            '图书登记': (self.func_dengji, self.post_dengji),
            '图书借阅': (self.func_jieyue, self.post_jieyue),
            '图书续借': (self.func_xujie, self.post_xujie),
            '图书归还': (self.func_guihuan, self.post_guihuan),
            '图书查询': (self.func_book_chaxun, self.post_book_chaxun),
            '消费查询': (self.func_consume_chaxun, self.post_consume_chaxun),
            '门禁查询': (self.func_menjin_chaxun, self.post_menjin_chaxun),
        }
        self.identify_func = {
            'all': ('学生开户', '学生卡挂失', '学生卡补卡', '学生注销', '学生卡消费',
                    '学生卡充值', '图书登记', '图书借阅', '图书续借', '图书归还',
                    '图书查询', '消费查询', '门禁查询',),
            'book': ('图书登记', '图书借阅', '图书续借', '图书归还','图书查询'),
            'student': ('学生开户',),
            'card': ('学生卡补卡', '学生卡充值', '学生卡消费', '学生卡挂失')
        }
        self.now_identify_func = self.identify_func[identify['position']]
        self.function_now = {key: self.function[key] for key in self.now_identify_func}
        for index, func_name in enumerate(self.function_now):
                Radiobutton(self.frame_left, text=func_name, indicatoron=0,
                            width=15, height=2, command=self.callback,
                            value=func_name, variable=self.now_func).pack()
        self.now_func.set(self.function_now.keys()[0])
        self.function_now[self.function_now.keys()[0]][0]()
        menubar = Menu(self.master)
        menubar.add_command(label="注销", command=logout)
        menubar.add_command(label="退出", command=self.master.quit)
        self.master['menu'] = menubar
        self.frame_left.pack(side=LEFT, fill=Y)
        self.frame_right.pack(side=LEFT, fill=Y)
        mainloop()

    def func_kaihu(self):
        if 'kaihu_view' not in dir(self):
            self.kaihu_view = Frame(self.frame_right)
            Label(self.kaihu_view, text="学号：").grid(row=0, column=0)
            self.kaihu_view_xuehao = StringVar()
            Entry(self.kaihu_view, textvariable=self.kaihu_view_xuehao)\
                .grid(row=0, column=1)
            Label(self.kaihu_view, text="姓名：").grid(row=1, column=0)
            self.kaihu_view_xingming = StringVar()
            Entry(self.kaihu_view, textvariable=self.kaihu_view_xingming)\
                .grid(row=1, column=1)
            Label(self.kaihu_view, text="院系：").grid(row=2, column=0)
            self.kaihu_view_yuanxi = StringVar()
            Entry(self.kaihu_view, textvariable=self.kaihu_view_yuanxi)\
                .grid(row=2, column=1)
            Button(self.kaihu_view, text="清空", command=self.clear_kaihu)\
                .grid(row=3, column=0, sticky=E)
            Button(self.kaihu_view, text="提交", command=self.post_kaihu)\
                .grid(row=3, column=1, sticky=E)
            self.kaihu_view_tip = StringVar()
            Label(self.kaihu_view, textvariable=self.kaihu_view_tip, fg="red")\
                .grid(row=4, column=1, sticky=E)
        self.kaihu_view.pack(padx=200, pady=100)
        self.frame_right_view = self.kaihu_view

    def post_kaihu(self):
        post_data = {
            'action': 'kaihu',
            'values': {
                'xuehao': self.kaihu_view_xuehao.get(),
                'xingming': self.kaihu_view_xingming.get(),
                'yuanxi': self.kaihu_view_yuanxi.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.kaihu_view_tip.set(tip_str)

    def clear_kaihu(self):
        self.kaihu_view_xuehao.set("")
        self.kaihu_view_xingming.set("")
        self.kaihu_view_yuanxi.set("")
        self.kaihu_view_tip.set("")

    def func_guashi(self):
        if 'guashi_view' not in dir(self):
            self.guashi_view = Frame(self.frame_right)
            Label(self.guashi_view, text="学号：").grid(row=0, column=0)
            self.guashi_view_xuehao = StringVar()
            Entry(self.guashi_view, textvariable=self.guashi_view_xuehao)\
                .grid(row=0, column=1)
            Button(self.guashi_view, text="清空", command=self.clear_guashi)\
                .grid(row=1, column=0, sticky=E)
            Button(self.guashi_view, text="提交", command=self.post_guashi)\
                .grid(row=1, column=1, sticky=E)
            self.guashi_view_tip = StringVar()
            Label(self.guashi_view, textvariable=self.guashi_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.guashi_view.pack(padx=200, pady=100)
        self.frame_right_view = self.guashi_view

    def post_guashi(self):
        post_data = {
            'action': 'guashi',
            'values': {
                'xuehao': self.guashi_view_xuehao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.guashi_view_tip.set(tip_str)

    def clear_guashi(self):
        self.guashi_view_xuehao.set("")
        self.guashi_view_tip.set("")

    def func_buka(self):
        if 'buka_view' not in dir(self):
            self.buka_view = Frame(self.frame_right)
            Label(self.buka_view, text="学号：").grid(row=0, column=0)
            self.buka_view_xuehao = StringVar()
            Entry(self.buka_view, textvariable=self.buka_view_xuehao)\
                .grid(row=0, column=1)
            Button(self.buka_view, text="清空", command=self.clear_buka)\
                .grid(row=1, column=0, sticky=E)
            Button(self.buka_view, text="提交", command=self.post_buka)\
                .grid(row=1, column=1, sticky=E)
            self.buka_view_tip = StringVar()
            Label(self.buka_view, textvariable=self.buka_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.buka_view.pack(padx=200, pady=100)
        self.frame_right_view = self.buka_view

    def post_buka(self):
        post_data = {
            'action': 'buka',
            'values': {
                'xuehao': self.buka_view_xuehao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.buka_view_tip.set(tip_str)

    def clear_buka(self):
        self.buka_view_xuehao.set("")
        self.buka_view_tip.set("")

    def func_zhuxiao(self):
        if 'zhuxiao_view' not in dir(self):
            self.zhuxiao_view = Frame(self.frame_right)
            Label(self.zhuxiao_view, text="学号：").grid(row=0, column=0)
            self.zhuxiao_view_xuehao = StringVar()
            Entry(self.zhuxiao_view, textvariable=self.zhuxiao_view_xuehao)\
                .grid(row=0, column=1)
            Button(self.zhuxiao_view, text="清空", command=self.clear_zhuxiao)\
                .grid(row=1, column=0, sticky=E)
            Button(self.zhuxiao_view, text="提交", command=self.post_zhuxiao)\
                .grid(row=1, column=1, sticky=E)
            self.zhuxiao_view_tip = StringVar()
            Label(self.zhuxiao_view, textvariable=self.zhuxiao_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.zhuxiao_view.pack(padx=200, pady=100)
        self.frame_right_view = self.zhuxiao_view

    def post_zhuxiao(self):
        post_data = {
            'action': 'zhuxiao',
            'values': {
                'xuehao': self.zhuxiao_view_xuehao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.zhuxiao_view_tip.set(tip_str)

    def clear_zhuxiao(self):
        self.zhuxiao_view_xuehao.set("")
        self.zhuxiao_view_tip.set("")

    def func_chongzhi(self):
        if 'chongzhi_view' not in dir(self):
            self.chongzhi_view = Frame(self.frame_right)
            Label(self.chongzhi_view, text="学号：").grid(row=0, column=0)
            self.chongzhi_view_xuehao = StringVar()
            Entry(self.chongzhi_view, textvariable=self.chongzhi_view_xuehao)\
                .grid(row=0, column=1)
            Label(self.chongzhi_view, text="充值金额：").grid(row=1, column=0)
            self.chongzhi_view_jine = DoubleVar()
            Entry(self.chongzhi_view, textvariable=self.chongzhi_view_jine)\
                .grid(row=1, column=1)
            Button(self.chongzhi_view, text="清空", command=self.clear_chongzhi)\
                .grid(row=2, column=0, sticky=E)
            Button(self.chongzhi_view, text="提交", command=self.post_chongzhi)\
                .grid(row=2, column=1, sticky=E)
            self.chongzhi_view_tip = StringVar()
            Label(self.chongzhi_view, textvariable=self.chongzhi_view_tip, fg="red")\
                .grid(row=3, column=1, sticky=E)
        self.chongzhi_view.pack(padx=200, pady=100)
        self.frame_right_view = self.chongzhi_view

    def post_chongzhi(self):
        try:
            jine = self.chongzhi_view_jine.get()
        except:
            self.chongzhi_view_tip.set('金额必须是数字')
            return
        post_data = {
            'action': 'chongzhi',
            'values': {
                'xuehao': self.chongzhi_view_xuehao.get(),
                'jine': jine
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.chongzhi_view_tip.set(tip_str)

    def clear_chongzhi(self):
        self.chongzhi_view_xuehao.set("")
        self.chongzhi_view_jine.set("")
        self.chongzhi_view_tip.set("")

    def func_dengji(self):
        if 'dengji_view' not in dir(self):
            self.dengji_view = Frame(self.frame_right)
            Label(self.dengji_view, text="索引号：").grid(row=0, column=0)
            self.dengji_view_isbn = StringVar()
            Entry(self.dengji_view, textvariable=self.dengji_view_isbn)\
                .grid(row=0, column=1)
            Label(self.dengji_view, text="书号：").grid(row=1, column=0)
            self.dengji_view_bno= StringVar()
            Entry(self.dengji_view, textvariable=self.dengji_view_bno)\
                .grid(row=1, column=1)
            Label(self.dengji_view, text="书名：").grid(row=2, column=0)
            self.dengji_view_bname = StringVar()
            Entry(self.dengji_view, textvariable=self.dengji_view_bname)\
                .grid(row=2, column=1)
            Label(self.dengji_view, text="作者：").grid(row=3, column=0)
            self.dengji_view_bauthor = StringVar()
            Entry(self.dengji_view, textvariable=self.dengji_view_bauthor)\
                .grid(row=3, column=1)
            Label(self.dengji_view, text="出版社：").grid(row=4, column=0)
            self.dengji_view_bpress= StringVar()
            Entry(self.dengji_view, textvariable=self.dengji_view_bpress)\
                .grid(row=4, column=1)
            Button(self.dengji_view, text="清空", command=self.clear_dengji)\
                .grid(row=5, column=0, sticky=E)
            Button(self.dengji_view, text="提交", command=self.post_dengji)\
                .grid(row=5, column=1, sticky=E)
            self.dengji_view_tip = StringVar()
            Label(self.dengji_view, textvariable=self.dengji_view_tip, fg="red")\
                .grid(row=6, column=1, sticky=E)
        self.dengji_view.pack(padx=200, pady=100)
        self.frame_right_view = self.dengji_view

    def post_dengji(self):
        post_data = {
            'action': 'dengji',
            'values': {
                'isbn': self.dengji_view_isbn.get(),
                'bno': self.dengji_view_bno.get(),
                'bname': self.dengji_view_bname.get(),
                'bauthor': self.dengji_view_bauthor.get(),
                'bpress': self.dengji_view_bpress.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.dengji_view_tip.set(tip_str)

    def clear_dengji(self):
        self.dengji_view_isbn.set("")
        self.dengji_view_bno.set("")
        self.dengji_view_bname.set("")
        self.dengji_view_bauthor.set("")
        self.dengji_view_bpress.set("")
        self.dengji_view_tip.set("")

    def func_jieyue(self):
        if 'jieyue_view' not in dir(self):
            self.jieyue_view = Frame(self.frame_right)
            Label(self.jieyue_view, text="学号：").grid(row=0, column=0)
            self.jieyue_view_xuehao = StringVar()
            Entry(self.jieyue_view, textvariable=self.jieyue_view_xuehao)\
                .grid(row=0, column=1)
            Label(self.jieyue_view, text="索引号：").grid(row=1, column=0)
            self.jieyue_view_suoyinhao= StringVar()
            Entry(self.jieyue_view, textvariable=self.jieyue_view_suoyinhao)\
                .grid(row=1, column=1)
            Button(self.jieyue_view, text="清空", command=self.clear_jieyue)\
                .grid(row=2, column=0, sticky=E)
            Button(self.jieyue_view, text="提交", command=self.post_jieyue)\
                .grid(row=2, column=1, sticky=E)
            self.jieyue_view_tip = StringVar()
            Label(self.jieyue_view, textvariable=self.jieyue_view_tip, fg="red")\
                .grid(row=3, column=1, sticky=E)
        self.jieyue_view.pack(padx=200, pady=100)
        self.frame_right_view = self.jieyue_view

    def post_jieyue(self):
        post_data = {
            'action': 'jieyue',
            'values': {
                'xuehao': self.jieyue_view_xuehao.get(),
                'suoyinhao': self.jieyue_view_suoyinhao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.jieyue_view_tip.set(tip_str)

    def clear_jieyue(self):
        self.jieyue_view_xuehao.set("")
        self.jieyue_view_suoyinhao.set("")
        self.jieyue_view_tip.set("")
    
    def func_xujie(self):
        if 'xujie_view' not in dir(self):
            self.xujie_view = Frame(self.frame_right)
            Label(self.xujie_view, text="学号：").grid(row=0, column=0)
            self.xujie_view_xuehao = StringVar()
            Entry(self.xujie_view, textvariable=self.xujie_view_xuehao)\
                .grid(row=0, column=1)
            Label(self.xujie_view, text="索引号：").grid(row=1, column=0)
            self.xujie_view_suoyinhao= StringVar()
            Entry(self.xujie_view, textvariable=self.xujie_view_suoyinhao)\
                .grid(row=1, column=1)
            Button(self.xujie_view, text="清空", command=self.clear_xujie)\
                .grid(row=2, column=0, sticky=E)
            Button(self.xujie_view, text="提交", command=self.post_xujie)\
                .grid(row=2, column=1, sticky=E)
            self.xujie_view_tip = StringVar()
            Label(self.xujie_view, textvariable=self.xujie_view_tip, fg="red")\
                .grid(row=3, column=1, sticky=E)
        self.xujie_view.pack(padx=200, pady=100)
        self.frame_right_view = self.xujie_view

    def post_xujie(self):
        post_data = {
            'action': 'xujie',
            'values': {
                'xuehao': self.xujie_view_xuehao.get(),
                'suoyinhao': self.xujie_view_suoyinhao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.xujie_view_tip.set(tip_str)

    def clear_xujie(self):
        self.xujie_view_xuehao.set("")
        self.xujie_view_suoyinhao.set("")
        self.xujie_view_tip.set("")

    def func_guihuan(self):
        if 'guihuan_view' not in dir(self):
            self.guihuan_view = Frame(self.frame_right)
            Label(self.guihuan_view, text="索引号：").grid(row=0, column=0)
            self.guihuan_view_suoyinhao= StringVar()
            Entry(self.guihuan_view, textvariable=self.guihuan_view_suoyinhao)\
                .grid(row=0, column=1)
            Button(self.guihuan_view, text="清空", command=self.clear_guihuan)\
                .grid(row=1, column=0, sticky=E)
            Button(self.guihuan_view, text="提交", command=self.post_guihuan)\
                .grid(row=1, column=1, sticky=E)
            self.guihuan_view_tip = StringVar()
            Label(self.guihuan_view, textvariable=self.guihuan_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.guihuan_view.pack(padx=200, pady=100)
        self.frame_right_view = self.guihuan_view

    def post_guihuan(self):
        post_data = {
            'action': 'guihuan',
            'values': {
                'suoyinhao': self.guihuan_view_suoyinhao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            if ret_json['code'] == 0:
                tip_str = '罚款是'+str(ret_json['values']['fakuan'])
            else:
                tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.guihuan_view_tip.set(tip_str)

    def clear_guihuan(self):
        self.guihuan_view_suoyinhao.set("")
        self.guihuan_view_tip.set("")

    def func_book_chaxun(self):
        if 'book_chaxun_view' not in dir(self):
            self.book_chaxun_view = Frame(self.frame_right)
            Label(self.book_chaxun_view, text="索引号：").grid(row=0, column=0)
            self.book_chaxun_view_suoyinhao= StringVar()
            Entry(self.book_chaxun_view, textvariable=self.book_chaxun_view_suoyinhao)\
                .grid(row=0, column=1)
            Button(self.book_chaxun_view, text="清空", command=self.clear_book_chaxun)\
                .grid(row=1, column=0, sticky=E)
            Button(self.book_chaxun_view, text="提交", command=self.post_book_chaxun)\
                .grid(row=1, column=1, sticky=E)
            self.book_chaxun_view_tip = StringVar()
            Label(self.book_chaxun_view, textvariable=self.book_chaxun_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.book_chaxun_view.pack(padx=200, pady=100)
        self.frame_right_view = self.book_chaxun_view

    def post_book_chaxun(self):
        post_data = {
            'action': 'book_chaxun',
            'values': {
                'suoyinhao': self.book_chaxun_view_suoyinhao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            if ret_json['code'] == 0:
                length = len(ret_json['values'])
                if length == 0:
                    tip_str = "Null record"
                else:
                    self.frame_right_view.pack_forget()
                    self.table_view = ttk.Treeview(
                        self.frame_right, show="headings", height=20,
                        columns=('tushubianma', 'shuming', 'zuozhe', 'zhuangtai'))
                    self.table_view.column('tushubianma', width=150, anchor=CENTER)
                    self.table_view.heading('tushubianma', text="索引号")
                    self.table_view.column('shuming', width=200, anchor=CENTER)
                    self.table_view.heading('shuming', text="书名")
                    self.table_view.column('zuozhe', width=100, anchor=CENTER)
                    self.table_view.heading('zuozhe', text="作者")
                    self.table_view.column('zhuangtai', width=100, anchor=CENTER)
                    self.table_view.heading('zhuangtai', text="状态")
                    for book in ret_json['values']:
                        item = (book['tushubianma'], book['shuming'],
                                book['zuozhe'], book['zhuangtai'])
                        self.table_view.insert('', END, values=item)
                    self.table_view.pack(anchor=CENTER, padx=50, pady=20)
                    self.frame_right_view = None
                    return
            else:
                tip_str = ret_json['message']

        else:
            tip_str = ret_json
        self.book_chaxun_view_tip.set(tip_str)

    def clear_book_chaxun(self):
        self.book_chaxun_view_suoyinhao.set("")
        self.book_chaxun_view_tip.set("")

    def func_consume(self):
        if 'consume' not in dir(self):
            self.consume_view = Frame(self.frame_right)
            Label(self.consume_view, text="学号：").grid(row=0, column=0)
            self.consume_view_xuehao = StringVar()
            Entry(self.consume_view, textvariable=self.consume_view_xuehao)\
                .grid(row=0, column=1)
            Label(self.consume_view, text="类型：").grid(row=1, column=0)
            self.consume_view_type = StringVar()
            Entry(self.consume_view, textvariable=self.consume_view_type)\
                .grid(row=1, column=1)
            Label(self.consume_view, text="金额：").grid(row=2, column=0)
            self.consume_view_amount = DoubleVar()
            Entry(self.consume_view, textvariable=self.consume_view_amount)\
                .grid(row=2, column=1)
            Button(self.consume_view, text="清空", command=self.clear_consume)\
                .grid(row=3, column=0, sticky=E)
            Button(self.consume_view, text="提交", command=self.post_consume)\
                .grid(row=3, column=1, sticky=E)
            self.consume_view_tip = StringVar()
            Label(self.consume_view, textvariable=self.consume_view_tip, fg="red")\
                .grid(row=4, column=1, sticky=E)
        self.consume_view.pack(padx=200, pady=100)
        self.frame_right_view = self.consume_view
        
    def post_consume(self):
        try:
            amount = self.consume_view_amount.get()
        except:
            self.consume_view_tip.set('金额必须是数字')
            return
        post_data = {
            'action': 'consume',
            'values': {
                'xuehao': self.consume_view_xuehao.get(),
                'type': self.consume_view_type.get(),
                'amount': amount,
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.consume_view_tip.set(tip_str)


    def clear_consume(self):
        self.consume_view_xuehao.set("")
        self.consume_view_type.set("")
        self.consume_view_amount.set("")
        self.consume_view_tip.set("")

    def func_consume_chaxun(self):
        if 'consume_chaxun_view' not in dir(self):
            self.consume_chaxun_view = Frame(self.frame_right)
            Label(self.consume_chaxun_view, text="学号：").grid(row=0, column=0)
            self.consume_chaxun_view_xuehao = StringVar()
            Entry(self.consume_chaxun_view, textvariable=self.consume_chaxun_view_xuehao)\
                .grid(row=0, column=1)
            Button(self.consume_chaxun_view, text="清空", command=self.clear_consume_chaxun)\
                .grid(row=1, column=0, sticky=E)
            Button(self.consume_chaxun_view, text="提交", command=self.post_consume_chaxun)\
                .grid(row=1, column=1, sticky=E)
            self.consume_chaxun_view_tip = StringVar()
            Label(self.consume_chaxun_view, textvariable=self.consume_chaxun_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.consume_chaxun_view.pack(padx=200, pady=100)
        self.frame_right_view = self.consume_chaxun_view

    def post_consume_chaxun(self):
        post_data = {
            'action': 'consume_chaxun',
            'values': {
                'xuehao': self.consume_chaxun_view_xuehao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            if ret_json['code'] == 0:
                length = len(ret_json['values'])
                if length == 0:
                    tip_str = "Null record"
                else:
                    self.frame_right_view.pack_forget()
                    self.table_view = ttk.Treeview(
                        self.frame_right, show="headings", height=20,
                        columns=('xuehao', 'type', 'amount', 'time'))
                    self.table_view.column('xuehao', width=150, anchor=CENTER)
                    self.table_view.heading('xuehao', text="学号")
                    self.table_view.column('type', width=150, anchor=CENTER)
                    self.table_view.heading('type', text="类型")
                    self.table_view.column('amount', width=100, anchor=CENTER)
                    self.table_view.heading('amount', text="金额")
                    self.table_view.column('time', width=150, anchor=CENTER)
                    self.table_view.heading('time', text="时间")
                    for consume in ret_json['values']:
                        item = (consume['xuehao'], consume['type'],
                                consume['amount'], consume['time'])
                        self.table_view.insert('', END, values=item)
                    self.table_view.pack(anchor=CENTER, padx=50, pady=20)
                    self.frame_right_view = None
                    return
            else:
                tip_str = ret_json['message']

        else:
            tip_str = ret_json
        self.consume_chaxun_view_tip.set(tip_str)

    def clear_consume_chaxun(self):
        self.consume_chaxun_view_xuehao.set("")
        self.consume_chaxun_view_tip.set("")

    def func_open_menjin(self):
        if 'open_menjin_view' not in dir(self):
            self.open_menjin_view = Frame(self.frame_right)
            Label(self.open_menjin_view, text="学号：").grid(row=0, column=0)
            self.open_menjin_view_xuehao = StringVar()
            Entry(self.open_menjin_view, textvariable=self.open_menjin_view_xuehao)\
                .grid(row=0, column=1)
            Button(self.open_menjin_view, text="清空", command=self.clear_open_menjin)\
                .grid(row=1, column=0, sticky=E)
            Button(self.open_menjin_view, text="提交", command=self.post_open_menjin)\
                .grid(row=1, column=1, sticky=E)
            self.open_menjin_view_tip = StringVar()
            Label(self.open_menjin_view, textvariable=self.open_menjin_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.open_menjin_view.pack(padx=200, pady=100)
        self.frame_right_view = self.open_menjin_view

    def post_open_menjin(self):
        post_data = {
            'action': 'open_menjin',
            'values': {
                'xuehao': self.open_menjin_view_xuehao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.open_menjin_view_tip.set(tip_str)

    def clear_open_menjin(self):
        self.open_menjin_view_xuehao.set("")
        self.open_menjin_view_tip.set("")

    def func_close_menjin(self):
        if 'close_menjin_view' not in dir(self):
            self.close_menjin_view = Frame(self.frame_right)
            Label(self.close_menjin_view, text="学号：").grid(row=0, column=0)
            self.close_menjin_view_xuehao = StringVar()
            Entry(self.close_menjin_view, textvariable=self.close_menjin_view_xuehao)\
                .grid(row=0, column=1)
            Button(self.close_menjin_view, text="清空", command=self.clear_close_menjin)\
                .grid(row=1, column=0, sticky=E)
            Button(self.close_menjin_view, text="提交", command=self.post_close_menjin)\
                .grid(row=1, column=1, sticky=E)
            self.close_menjin_view_tip = StringVar()
            Label(self.close_menjin_view, textvariable=self.close_menjin_view_tip, fg="red")\
                .grid(row=2, column=1, sticky=E)
        self.close_menjin_view.pack(padx=200, pady=100)
        self.frame_right_view = self.close_menjin_view

    def post_close_menjin(self):
        post_data = {
            'action': 'close_menjin',
            'values': {
                'xuehao': self.close_menjin_view_xuehao.get(),
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            tip_str = ret_json['message']
        else:
            tip_str = ret_json
        self.close_menjin_view_tip.set(tip_str)

    def clear_close_menjin(self):
        self.close_menjin_view_xuehao.set("")
        self.close_menjin_view_tip.set("")

    def func_menjin_chaxun(self):
        if 'menjin_chaxun_view' not in dir(self):
            self.menjin_chaxun_view = Frame(self.frame_right)
            Button(self.menjin_chaxun_view, text="提交", command=self.post_menjin_chaxun)\
                .grid(row=0, column=0, sticky=E)
            self.menjin_chaxun_view_tip = StringVar()
            Label(self.menjin_chaxun_view, textvariable=self.menjin_chaxun_view_tip, fg="red")\
                .grid(row=1, column=1, sticky=E)
        self.menjin_chaxun_view.pack(padx=200, pady=100)
        self.frame_right_view = self.menjin_chaxun_view

    def post_menjin_chaxun(self):
        post_data = {
            'action': 'menjin_chaxun',
            'values': {
            }
        }
        ok, ret_json = post_api(post_data)
        if ok:
            if ret_json['code'] == 0:
                length = len(ret_json['values'])
                if length == 0:
                    tip_str = "Null record"
                else:
                    self.frame_right_view.pack_forget()
                    self.table_view = ttk.Treeview(
                        self.frame_right, show="headings", height=20,
                        columns=('xuehao', 'weizhi', 'menpaihao', 'time'))
                    self.table_view.column('xuehao', width=150, anchor=CENTER)
                    self.table_view.heading('xuehao', text="学号")
                    self.table_view.column('weizhi', width=200, anchor=CENTER)
                    self.table_view.heading('weizhi', text="位置")
                    self.table_view.column('menpaihao', width=100, anchor=CENTER)
                    self.table_view.heading('menpaihao', text="门牌号")
                    self.table_view.column('time', width=150, anchor=CENTER)
                    self.table_view.heading('time', text="时间")
                    for book in ret_json['values']:
                        item = (book['xuehao'], book['weizhi'],
                                book['menpaihao'], book['time'])
                        self.table_view.insert('', END, values=item)
                    self.table_view.pack(anchor=CENTER, padx=50, pady=20)
                    self.frame_right_view = None
                    return
            else:
                tip_str = ret_json['message']

        else:
            tip_str = ret_json
        self.menjin_chaxun_view_tip.set(tip_str)

    def callback(self):
        status = self.now_func.get().encode("utf-8")
        if self.table_view is not None:
            self.table_view.destroy()
            self.table_view = None
        if self.frame_right_view is not None:
            self.frame_right_view.pack_forget()
            self.frame_right_view = None
        self.function_now[status][0]()


CCMSClient()
#User_show()
