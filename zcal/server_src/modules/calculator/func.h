#ifndef _FUNC_H
#define _FUNC_H

#include"cal_tree.h"
/* 判断ch是否是函数中可以出现的符号 */
int is_func_arg_char(char ch);
/* 得到所有内置函数的数量，即func数组的元素的个数 */
int get_func_count();

extern char *func[];
extern Result (*func_func[])(char *str);
extern char *help_str[];

#endif
