#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"../../extend.h"
#include"../cal_tree.h"
#include"_handle_digit.h"

Result _func_exp(char *str){

	Result result;
	result.flag = -1;
	Double_array *da = _handle_digit(str);
	if(da != NULL){
		if(da->len == 1){
			result.result = exp(da->array[0]);
			result.flag = 1;
			free(da);
		}else{
			/* 函数参数个数不对 */
			error_flag = 4;	
			free(da);
		}
	}

	return result;
}
