#ifndef _CAL_TREE_H
#define _CAL_TREE_H

#include"analyze_unit.h"



typedef struct unit_result Result;
struct unit_result{
	/* 这里存放结点单元返回的结果 */
	double result;
	/* 这里标记结果是否有效，1有效，0无效 */
	char flag;
};



/* 参数是一棵计算器表达式解析后生成到表达式树
 * 本函数主要用于表达式树的计算，返回计算结果有效标志
 * 0代表cal_result全局变量无效，1代表有效 */
int cal_tree(Unit * cal_root);

/* 将一个字符串转化成相应数字 */
Result cal_str_digit(char *str, int len);

#endif
