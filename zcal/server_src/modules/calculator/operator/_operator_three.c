#include<stdio.h>
#include<math.h>
#include"../../extend.h"
#include"../cal_tree.h"
#include"../operator.h"

Result _operator_multiply(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		result.result = left.result * right.result;
		result.flag = 1;
	}else{
		result.flag = -1;
	}
	return result;
}


Result _operator_divide(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		if(right.result != 0 ){
			result.result = left.result / right.result;
			result.flag = 1;
		}else{
			error_flag = 11;
			result.flag = -1;
		}
	}else{
		result.flag = -1;
	}

	return result;
}

Result _operator_remainder(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		if(right.result != 0){
			result.result = fmod(left.result, right.result);
			result.flag = 1;
		}else{
			error_flag = 13;
			result.flag = -1;
		}
	}else{
			result.flag = -1;
	}
	return result;
}

Result _operator_exact_divide(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		if(right.result != 0){
		result.result = ((int)left.result) / ( (int)right.result );
		result.flag = 1;
		}else{
			error_flag = 11;
			result.flag = -1;
		}
	}else{
		result.flag = -1;
	}
	return result;

}
