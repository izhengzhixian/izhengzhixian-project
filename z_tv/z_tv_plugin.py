import os
import json
import subprocess

class TV_INFO():
    def __init__(self, path=None):
        self.path = path
        if path is None:
            self.tv_info = {
                'tv':{},
                'save':{
                    'tv_name':'',
                    'tv_set': 1
                }
            }
        else:
            with open(path) as tv_info_file:
                self.tv_info = json.loads(tv_info_file.read())

            

    def set_path(self, path):
        self.path = path


    def get_path(self):
        return self.path


    def save(self, tv_name=None, tv_set=None):
        if self.path is None:
            return False
        if tv_name is not None and tv_set is not None:
            self.tv_info['save']['tv_name'] = tv_name
            self.tv_info['save']['tv_set'] = tv_set
            single_tv = self.get_single_tv(tv_name)
            if single_tv is None:
                return False
            single_tv.set_save(tv_set)
        with open(self.path, "w") as tv_info_file:
            tv_info_file.write(json.dumps(self.tv_info,
                ensure_ascii=False, indent=4))
        return True


    def load(self, path):
        self.__init__(self, path)


    def get_single_tv_list(self):
        ret = []
        for name, name_info in self.tv_info['tv'].items():
            ret.append(SINGLE_TV({name: name_info}))
        return ret


    def get_single_tv(self, name):
        name_info = self.tv_info['tv'].get(name)
        if name_info is None:
            return None
        else:
            return SINGLE_TV({name: name_info})
        

    def delete(self, name):
        single_tv = self.get_single_tv(name)
        if single_tv is None:
            return False
        del self.tv_info['tv'][name]
        return single_tv


    def delete_all(self):
        self.tv_info['tv'] = {}
        return True


    def get_save(self):
        return self.tv_info['save']['tv_name'], self.tv_info['save']['tv_set']


    def get_url(self, tv_name, tv_set):
        single_tv = self.get_single_tv(tv_name)
        if single_tv is None:
            return False, "本地没有电视剧资源"
        return single_tv.get_url(tv_set)


    def get_next_info(self, tv_name, tv_set):
        ret = self.guess(tv_name, tv_set)
        if ret[0] is False:
            return ret
        ret2 = self.get_url(ret[1], ret[2])
        if ret2[0] is False:
            return ret2
        return True, ret[1], ret[2], ret2[1]


    def guess(self, tv_name=None, tv_set=None):
        if tv_name is None:
            tv_name = self.tv_info["save"]["tv_name"]
            if tv_name == "":
                return False, "没有默认电视剧名字，请指定"
        single_tv= self.get_single_tv(tv_name)
        if single_tv is None:
            return False, "没有该电视剧资源，请先增加"
        if tv_set is None:
            tv_set = single_tv.get_save()
        elif tv_set <= 0:
            tv_set = tv_set + single_tv.get_save()
        return True, tv_name, tv_set


    def update(self, single_tv):
        self.tv_info['tv'].update(single_tv.single_tv)
        return True


    def play(self, tv_name, tv_set):
        '''
        try:
            requests.get("http://www.baidu.com")
        except:
            return False
        '''
        ret = self.guess(tv_name, tv_set)
        if ret[0] is False:
            return ret[0], ret[1]
        tv_name = ret[1]
        tv_set = ret[2]
        ret = self.get_url(tv_name, tv_set)
        if ret[0] is False:
            return ret[0], ret[1]
        one_set_url = ret[1]
        self.save(tv_name, tv_set+1)
        devnull = open(os.devnull, "w")
        ret = "you-get" + " -p" + " mpv " + one_set_url["url"]
        subprocess.Popen(["you-get", "-p", "mpv", one_set_url['url']],
                         stdout=devnull, stderr=devnull)
        return True, ret


class SINGLE_TV():
    def __init__(self, name):
        if isinstance(name, dict):
            self.single_tv = name
            self.name = tuple(name)[0]
        elif isinstance(name, str):
            self.single_tv = {
                name: {
                    'list': [],
                    'total': 0,
                    'save': 1,
                    'src': '',
                    'tag': ''
                }
            }
            self.name = name
        else:
            raise TypeError
        self.name_info = self.single_tv[self.name]


    def append(self, url):
        self.name_info['list'].append(url)
        self.name_info['total'] = self.get_total() + 1


    def extend(self, url_list):
        self.name_info['list'].extend(url_list)
        self.name_info['total'] = self.get_total() + len(url_list)


    def set_save(self, save):
        self.name_info['save'] = save


    def get_save(self):
        return self.name_info['save']


    def set_tag(self, tag):
        self.name_info['tag'] = tag


    def get_tag(self):
        return self.name_info['tag']


    def set_src(self, src):
        self.name_info['src'] = src


    def get_src(self):
        return self.name_info['src']


    def get_total(self):
        return self.name_info['total']


    def get_url(self, tv_set):
        if tv_set <= 0:
            tv_set = self.get_save() + tv_set
        if tv_set < 1 or self.get_total() < tv_set:
            return False, "没有该集"
        return True, self.name_info['list'][tv_set-1]


    def reverse(self):
        self.name_info['list'].reverse()

    
    def get_list(self):
        return tuple(self.name_info['list'])
