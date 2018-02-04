#include<stdio.h>
#include<math.h>
#include"../../extend.h"
#include"../cal_tree.h"
#include"../operator.h"

Result _operator_not(Result left, Result right){

	Result result;

	if(left.flag == 0  &&  right.flag == 1){
		if(judge_int(right.result)){
			result.result = ~(int)right.result;
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
