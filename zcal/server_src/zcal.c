#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"modules/define.h"
#include"modules/extend.h"
#include"modules/judge.h"
#include"modules/command.h"
#include"modules/calculator.h"

char * zcal(char * str){

	char * str_result = NULL;
	size_t length;
	int judge_result;

	/* 每次执行命令，错误标志清空 */
	error_flag = 0;

	/* 对用户输入进行初次加工,在judge模块中 */
	str_filter(str);

	/* 如果用户输入为空，则重新输入 */
	if(strlen(str) == 0 ){
		str_result = malloc(MAX_USER_OUTPUT);
		strcpy(str_result, "用户输入为空");
		return str_result;
	}

	judge_result=judge(str);

	/* command函数和calculator函数，如果返回NULL，说明运行错误
	 * 则需要执行explain_error函数解释错误
	 * 除此之外，command，calculator，explain_error返回的字符串
	 * 都是在堆上申请到空间，所以while结尾要free掉。 */

	if( judge_result == 0 ){
		str_result = command(str);
		if(str_result != NULL){
			return str_result;
		}
	}else{
		if( calculator(str) ){
			str_result = malloc(MAX_USER_OUTPUT);
			sprintf(str_result, "%f", cal_result);
			int i;
			for(i = strlen(str_result)-1; str_result[i] == '0'; i--){
				str_result[i] = 0;
			}
			int cal_str_len = strlen(str_result);
			if (str_result[ cal_str_len-1 ] == '.')
				str_result[ cal_str_len-1 ] = 0;

			return str_result;
		}
	}
	if(error_flag != 0){
		str_result = explain_error(); 
	}
	return str_result;
}
