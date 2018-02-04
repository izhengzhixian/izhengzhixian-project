#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"../../extend.h"
#include"../analyze_unit.h"
#include"analyze_unit_digit.h"

int analyze_unit_digit(char *str, int current_pos, size_t len){

	if(expect_unit == 2){
		error_flag = 6;
		pos_flag = str+current_pos;
		return -1;
	}

	/* 用来标识'.'是否出现 */
	int dot=0;
	/* 用来标识数字是否出现 */
	int digit=0;
	int ret;
	char ch;
	if(str[current_pos] == '0' && str[current_pos+1] == 'x'){
		/* 判断是否是16进制格式 */
	
		for(current_pos += 2;current_pos < len; current_pos++){
			ch=str[current_pos];
			

			if(ch>='0' && ch<='9' || ch>='a' && ch<='f'){
				digit++;
				continue;
			}else if(ch == '.'){
				if(dot == 0){
					dot = 1;
				}else{
					error_flag = 5;
					pos_flag = str+current_pos;
				}
			}else{
				if(digit)
					return current_pos - 1;
				else{
					/* 没有出现数字，就直接以字符结尾，例如0xh */
					error_flag = 2;
					pos_flag = str+ current_pos;
					return -1;
				}
			}

		}
	}else if(str[current_pos] == '0' && str[current_pos+1] == 'b'){
		for(current_pos += 2;current_pos < len; current_pos++){
			ch=str[current_pos];
			if(ch >= '0' && ch<='1'){
				digit++;
				continue;
			}else if(ch == '.'){
				if(dot == 0){
					dot = 1;
				}else{
					error_flag = 5;
					pos_flag = str+current_pos;
				}
			}else{
				if(digit)
					return current_pos - 1;
				else{
					/* 没有出现数字，就直接以字符结尾，例如0xh */
					error_flag = 2;
					pos_flag = str+ current_pos;
					return -1;
				}
			}
		}
	}else{

		for(;current_pos < len; current_pos++){
			ret = isdigit_dot(str[current_pos]);
			if(ret == 1){
				digit++;
				continue;
			}else if(ret == 2){
				if(dot == 0)
					dot = 1;
				else{
					/* 小数点错误，一个数字中出现了第二个小数点 */
					error_flag = 5;
					pos_flag = str+current_pos;
					return -1;
				}
			}else{
				if(digit)
					return current_pos - 1;
				else{
					/* 没有出现数字，就直接以字符结尾，例如.b*/
					error_flag = 2;
					pos_flag = str+ current_pos;
					return -1;
				}
			}
		}
	}

	return current_pos - 1;
}


int isdigit_dot(char ch){
	if(isdigit(ch)  )
		return 1;
	if(ch == '.')
		return 2;
	return 0;
}
