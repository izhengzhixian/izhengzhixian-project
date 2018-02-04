#include<stdio.h>
#include"../../extend.h"
#include"../cal_tree.h"
#include"../operator.h"

Result _operator_and(Result left, Result right){
	Result result;
	if(left.flag == 1 && right.flag == 1){
		if(judge_int(left.result) && judge_int(right.result) ){
			result.result = (int)left.result & (int)left.result;
			result.flag = 1;
		}else{
			error_flag = 12;
			result.flag = -1;
		}
	}else{
		result.flag = -1;
	}

	return result;
}
