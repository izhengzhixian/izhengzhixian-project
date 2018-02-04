#ifndef _COMMAND_H
#define _COMMAND_H


/* 程序的帮助部分，用来执行程序内置命令
 * 参数是命令字符串的起始位置
 * 返回结果字符串 */
char * command(char *str);
/* 得到所有内置命令的数量，即cmd数组的元素的个数 */
int get_cmd_count();

extern char *cmd[];

#endif
