#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include<math.h>
#include"../define.h"
#include"../extend.h"
#include"analyze_unit.h"
#include"cal_tree.h"
#include"func.h"
#include"operator.h"

double char2digit(int ch, int redix);
Result cal_tree_control(Unit *sub_tree);
Result cal_func(Unit *func_unit);
Result cal_operator(Unit *operator_unit);
Result cal_digit(Unit *digit_unit);

int cal_tree(Unit * main_tree){

	if(main_tree == NULL)
		return 0;
	Result result = cal_tree_control(main_tree);
	if(result.flag == -1)
		return 0;
	else if(result.flag == 1){
		cal_result = result.result;
		return 1;
	}else{
		return 0;
	}
	
}

Result cal_tree_control(Unit *sub_tree){

	Result result;

	if(sub_tree == NULL){
		result.flag = 0;
		return result;
	}


	if(sub_tree->unit_flag >= 0){
		result = cal_operator(sub_tree);
	}else if(sub_tree->unit_flag == -2){
		result = cal_digit(sub_tree);
	}else if(sub_tree->unit_flag == -3){
		result = cal_func(sub_tree);
	}else{
		result.flag = -1;
	}
	

	return result;
}

Result cal_func(Unit *func_unit){

	Result result;
	result.flag = -1;
	char *str=func_unit->str;
	int len = func_unit->len;
	int l_bracket;
	for(l_bracket=0; l_bracket<len; l_bracket++)
		if(str[l_bracket] == '(')
			break;
	
	/* 在这里，过滤掉来函数参数为空的情况 */
	int arg_len = len - l_bracket - 2;
	if(arg_len <= 0){
		error_flag = 4;
		return result;
	}

	char *str_func = my_calloc(MAX_USER_INPUT);
	char *str_arg = my_calloc(MAX_USER_INPUT);
	memcpy(str_func, str, l_bracket);
	memcpy(str_arg, str+l_bracket+1, arg_len);
	str_func[l_bracket] = 0;
	str_arg[arg_len] = 0;

	int i;
	int func_count = get_func_count();
	/* 判断函数是否存在 */
	for(i=0; i<func_count; i++){
		if(strlen(func[i]) == l_bracket && !strcmp(func[i], str_func) ){
			result = func_func[i](str_arg);
			break;
		}
	}
	free(str_func);
	free(str_arg);

	return result;
}
Result cal_operator(Unit *operator_unit){

	Result left;
	Result right;
	Result result;

	left = cal_tree_control(operator_unit->left);
	right = cal_tree_control(operator_unit->right);
	if(left.flag == -1){
		free(operator_unit);
		return left;

	}
	if(right.flag == -1){
		free(operator_unit);
		return right;
	}
	result =  operator_func[operator_unit->unit_flag](left, right);
	free(operator_unit);

	return result;

}

Result cal_digit(Unit *digit_unit){
	Result result = cal_str_digit(digit_unit->str, digit_unit->len);
	free(digit_unit);
	return result;
}

Result cal_str_digit(char *str, int len){

	Result result;
	result.flag = -1;
	int i;
	int dot_pos;
	double dot_front = 0;
	double dot_back = 0;
	double current_bit;
	int current_redix;

	/* 判断进制，并将进制位字符删去 */
	if(str[0] == '0' && str[1] == 'b'){
		current_redix = 2;
		str += 2;
		len -= 2;
	}else if(str[0] == '0' && str[1] == 'x'){
		current_redix = 16;
		str += 2;
		len -= 2;
	}else{
		current_redix = 10;
	}

	for(dot_pos=0; dot_pos<len; dot_pos++)
		if(str[dot_pos] == '.')
			break;
	/* 如果dot_pos == len，说明数字中没有点 */

	/* 计算小数点前的数字 */
	for(i=0; i<dot_pos; i++){
		current_bit = char2digit(str[i], current_redix);
		if(current_bit == -1){
			error_flag = 2;
			return result;
		}
		dot_front = dot_front + current_bit * pow(current_redix, dot_pos - i -1);
	}


	/* 计算小数点后的数字 */
	for(i = dot_pos+1; i<len; i++){
		current_bit = char2digit(str[i], current_redix);
		if(current_bit == -1){
			error_flag = 2;
			return result;
		}
		dot_back = dot_back + current_bit * pow(current_redix, dot_pos - i);
	}
	result.result = dot_front + dot_back;
	result.flag = 1;

	return result;
}

double char2digit(int ch, int redix){

	int run = 0;
	if(redix == 16 && isxdigit(ch) || 
		redix == 10 && isdigit(ch) ||
		redix == 2 && (ch == '1' || ch == '0')   )
		run = 1;
		

	if(run){
		switch(ch){
			case '0':
				return 0;
			case '1':
				return 1;
			case '2':
				return 2;
			case '3':
				return 3;
			case '4':
				return 4;
			case '5':
				return 5;
			case '6':
				return 6;
			case '7':
				return 7;
			case '8':
				return 8;
			case '9':
				return 9;
			case 'a':
				return 10;
			case 'b':
				return 11;
			case 'c':
				return 12;
			case 'd':
				return 13;
			case 'e':
				return 14;
			case 'f':
				return 15;

		}
	}

	return -1;
}
