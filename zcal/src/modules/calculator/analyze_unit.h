#ifndef _ANALYZE_UNIT_H
#define _ANALYZE_UNIT_H

#include<stdio.h>

/* 用于标识，解析计算器表达式，
 * 解析单元期望下次出现的单元类型
 * 0表示无所谓(不能是双目运算符)，用于计算器表达式开始计算前的初始化
 * 1表示期望数据单元，2表示期望双目运算符单元
 * 3表示期望单目运算符或数据单元 */

extern int expect_unit;


/***********************************************
 *               结构体区                      *
 * *********************************************/



/* 用于计算的树的结点
 * 结点储存两种内容，最小的单元数据和运算符
 * 运算符是单元数据的父结点 */
typedef struct cal_unit Unit;
struct cal_unit{
	char *str;		//结点字符串到起始位置
	int len;		//结点字符串到长度
	/* 负数代表数据单元，非负数代表运算符单元
	 * -3代表函数单元
	 * -2代表数字单元
	 * -1 空置
	 *  运算符单元与其在operator数组中的索引想对应
	 *  赋值时可以通过get_operator_pos得到运算符的索引 */
	int unit_flag;	
	/* 如果是单目运算符，设置为1，双目运算符设置为2
	 * 数据单元空置 */
	int num_flag;	
	/* 运算符的运算级别,从1开始，数据的运算级是0，属于最高 */
	int level;
	Unit * left;	//结点的左子树
	Unit * right;	//结点的右子树
	Unit * next;	//栈中使用
	Unit * pre;		//栈中使用
};

/* 单元结点的栈 
 * 生成树之前先把单元结点放到栈里面 */
typedef struct unit_stack Ustack;
struct unit_stack{
	int len;
	Unit *head;
};

/* 解析字符串参数，len是解析字符串到长度
 * 将字符串分解成多个单元，并按照顺序依次入栈
 * 错误将返回NULL */
Ustack * analyze_unit(char *str, size_t len);

/* 生成一个单元栈 */
Ustack * createUstack();
/* 入栈 */
void pushUstack(Ustack * pus, Unit * push_unit);
/* 出栈 */
Unit * popUstack(Ustack *pus);
/* 销毁单元栈,返回值是参数 */
void destroyUstack(Ustack *pus);


#endif
