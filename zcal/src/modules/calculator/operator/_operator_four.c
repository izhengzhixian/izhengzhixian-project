#include<stdio.h>
#include"../cal_tree.h"

Result _operator_add(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		result.result = left.result + right.result;
		result.flag = 1;
	}else if(left.flag == 0 && right.flag == 1){
		result.result = right.result;
		result.flag = 1;
	}else{
		result.flag = -1;
	}
	return result;
}

Result _operator_sub(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		result.result = left.result - right.result;
		result.flag = 1;
	}else if(left.flag == 0 && right.flag == 1){
		result.result = -right.result;
		result.flag = 1;
	}else{
		result.flag = -1;
	}
	return result;
}

