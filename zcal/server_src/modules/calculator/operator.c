#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../extend.h"
#include"cal_tree.h"
#include"operator/operator_func.h"
#include"operator.h"

const double eps = 1e-10;

/* 运算符 
 * 正负号不在这种出现，会在analyze_unit中直接处理 */
Operator operator[]={
	/* 长度，级别，单双目(1单，2双，3单双都可以) */
	{"**",2,1,2},
	{"~",1,2,1},
	{"*",1,3,2},{"/",1,3,2},{"%",1,3,2},{"//",2,3,2},
	{"+",1,4,3},{"-",1,4,3},
	{"&",1,5,2},
	{"^",1,6,2},
	{"|",1,7,2}
};

Result (*operator_func[])(Result left, Result right)={
	
	_operator_pow,
	_operator_not,
	_operator_multiply,_operator_divide,_operator_remainder,_operator_exact_divide,
	_operator_add,_operator_sub,
	_operator_and,
	_operator_xor,
	_operator_or

};



char * get_operator_level(char *str, size_t len){
	char *ret_str = my_calloc(2);
	int i;
	int operator_len = get_operator_count();
	/* 找到与传入字符串相等的运算符
	 * 并把运算符等级，与单双目写入返回字符串 */
	for(i=0; i<operator_len; i++)
		if( operator[i].len == len && !strncmp(str, operator[i].str, len) ){
			ret_str[0] = operator[i].level;
			ret_str[1] = operator[i].num;

			return ret_str;
		}

	/* 找不到，则返回一个为"\0\0"的字符串 */
	return ret_str;
}

int get_operator_count(){
	return sizeof(operator) / sizeof(*operator);
}

int get_operator_pos(char *str, size_t len){

	int i;
	int operator_len = get_operator_count();
	for(i=0; i<operator_len; i++)
		if( operator[i].len == len && !strncmp(str, operator[i].str, len) )
			return i;

	return -1;
}

int get_operator_max_length(){
	int ret = 0;
	int operator_len = sizeof(operator) / sizeof(*operator);
	int i;
	for(i=0; i<operator_len; i++){
		ret = operator[i].len > ret ? operator[i].len: ret;
	}
	return ret;
}


int is_operator_char(char ch){
	int i;
	int operator_len = sizeof(operator) / sizeof(*operator);
	for(i=0; i<operator_len; i++)
		if(strchr(operator[i].str,ch))
			return 1;
	return 0;
}

int judge_int(double dou){
	if((dou - (double)(int)dou) < eps)
		return 1;
	return 0;
}
