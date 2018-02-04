#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"../../extend.h"
#include"../../calculator.h"
#include"../../define.h"
#include"../cal_tree.h"
#include"_handle_digit.h"

Double_array *_handle_digit(char *str){
	Double_array *da = my_calloc(sizeof(Double_array));
	da->len = 0;

	int i;
	int start = 0;
	char *sub_str;
	int len = strlen(str);
	for(i=0; i<len; i++){
		if(str[i] == ','){
			sub_str = my_calloc(i-start+1);
			memcpy(sub_str,str,i-start);
			if( calculator(sub_str) ){
				da = realloc(da,sizeof(Double_array) + 
						sizeof(double) * (da->len+1) );
				if(!da)
					exit(1);
				da->array[da->len] = cal_result;
				(da->len)++;
				free(sub_str);
			}else{
				free(sub_str);
				free(da);
				return NULL;
			}
			start = i + 1;
		}
	}

	sub_str = my_calloc(i-start+1);
	memcpy(sub_str,str+start,i-start);
	if( calculator(sub_str) ){
		da = realloc(da, sizeof(Double_array) + 
				sizeof(double) * (da->len+1) );
		if(!da)
			exit(1);
		da->array[da->len] = cal_result;
		(da->len)++;
	}else{
		free(sub_str);
		free(da);
		return NULL;
	}

	return da;
}
