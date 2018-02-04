#ifndef _OPERATOR_H
#define _OPERATOR_H
#include"cal_tree.h"

/* 运算符结构体 */
typedef struct cal_operator Operator;
struct cal_operator{
	char str[3];	//运算符字符串
	char len;		//运算符字符串长度
	char level;		//运算符等级
	char num;		//1代表单目，2代表双目，3代表单双目都可以
};

/* 获取运算符优先级函数
 * 参数是运算符起始位置，和有效长度
 * 返回一个在堆上长度为2的字符串，使用完需要free
 * 只使用字符的数值，两个字符分别代表等级，（单双目，用1，2表示）
 * 如果第一个字符为0，则代表没有该运算符 */
char * get_operator_level(char *str ,size_t len);

/* 得到operator数组中元素的个数 */
int get_operator_count();

/* 得到运算符在operator数组中的索引，找不到则返回-1 */
int get_operator_pos(char *str, size_t len);

/* 得到所有运算符中最长的运算符的长度 */
int get_operator_max_length();


/* 如果是运算符中含有的字符，返回1，否则返回0 */
int is_operator_char(char ch);


/* 判断一个double是不是整型数字，如果是，返回1，否则返回0 */
int judge_int(double dou);


extern const double esp;

extern Result (*operator_func[])(Result left, Result right);

#endif
