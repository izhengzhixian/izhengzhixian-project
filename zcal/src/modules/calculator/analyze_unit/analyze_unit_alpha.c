#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"../../extend.h"
#include"../analyze_unit.h"
#include"../operator.h"
#include"../func.h"

int analyze_unit_alpha(char *str, int current_pos, size_t len){
	

	if(expect_unit == 2){
		error_flag = 6;
		pos_flag = str+current_pos;
		return -1;
	}

	int i;
	/* 标记左括号出现的次数 */
	int l_bracket_count = 0;
	/* 当左括号'('出现++，当右括号')'出现--。 */
	int l_bracket_flag = 0;
	
	for(i=current_pos; i<len ;i++)
		if( isalpha(str[i]) || str[i] == '_' || isdigit(str[i]) ){
			/* 如果字符是函数名中的，则跳过
			 * 注意：使用本函数之前必须判定字符串第一个字符是否是字母或'_'
			 *
			 * 参数可以是表达式 */
		
		}else if(str[i] == '('){
			l_bracket_count++;
			l_bracket_flag++;
		}else if(str[i] == ')'){
			if(l_bracket_count){
				l_bracket_flag--;
				if(l_bracket_flag == 0)
					return i;
			}else{
				/* 没出现左括号，直接出现右括号，报错 */
				error_flag = 4;
				return -1;
			}
		}else if(is_func_arg_char(str[current_pos]) ||
				is_operator_char(str[current_pos])){
			/*函数参数中出现的特殊字符，如'-' , ',' '+'
			 * 注意，本分支中含有'('和')'，但是在上面就会被选择 */		
			if(!l_bracket_flag){
				error_flag = 4;
				return -1;
			}
		}else{
			/* 函数部分出现'&','$','@'等乱七八糟的字符 
			 * 返回格式错误 */
			error_flag = 4;
			return -1;
		}
		

	error_flag = 4;
	return -1;
}

