#ifndef _JUDGE_H
#define _JUDGE_H


/* 对用户输入进行初次加工，主要有以下两点
 * 1. 全部变成小写
 * 2. 去除空格
 * 返回值是参数 */
char * str_filter(char *str);

/* 判断用户输入字符串是哪种类型
 * 如果是内部命令，输出0；
 * 如果是计算器表达式，输出1；*/
int judge(char *str);


#endif
