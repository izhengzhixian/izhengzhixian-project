#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../../extend.h"
#include"../analyze_unit.h"
#include"../operator.h"

int analyze_unit_operator(char *str, int current_pos, size_t len){

	/* 获取最长的运算符的长度 */
	int operator_max_len = get_operator_max_length();
	/* 用来标记当前字符串是运算符的有效个数
	 * 如'*'是运算符，有效字符个数为1
	 * '**'是运算符，有效字符个数为0 */
	int current_len = 0;
	/* 用来标记当前运算符是单目还是双目 */
	int current_num = 0;
	int j;

	char *oper_level;
	for(j=0; j<operator_max_len && 
			is_operator_char(str[current_pos+j]); j++){

		oper_level = get_operator_level(str+current_pos, j+1);
		if(oper_level[0]){
			current_len = j+1;
			current_num = oper_level[1];
		}
		free(oper_level);
	}
	if(current_len == 0){
		/* 没有识别出来一个有效的运算符 */
		error_flag = 8;
		pos_flag = str + current_pos;
		return -1;
	}else{
		if(expect_unit == 0 || expect_unit == 3){
			if(current_num % 2 == 1){
				return current_pos + current_len -1;
			}else{
				/* 非单目运算符，报错 */
				error_flag = 8;
				pos_flag = str + current_pos;
				return -1;
			}
		}else if(expect_unit == 1){
			/* 非数据单元，报错 */
			error_flag = 6;
			pos_flag = str + current_pos;
			return -1;
		}else if(expect_unit == 2){
			if(current_num >= 2 ){
				return current_pos + current_len -1;
			}else{
				/* 单目运算符，报错 */
				error_flag = 8;
				pos_flag = str + current_pos;
				return -1;
			}
		}else{
			exit(1);
		}
	}
}
