#include<stdio.h>
#include<math.h>
#include"../cal_tree.h"

Result _operator_pow(Result left, Result right){
	Result result;
	result.flag = -1;
	if(left.flag == 1 && right.flag == 1){
		result.result = pow(left.result, right.result);
		result.flag = 1;
	}
	return result;
}
