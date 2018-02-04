#include"../cal_tree.h"

/* 一个double型的柔性数组 */
typedef struct{
	int len;
	double array[]; 
}Double_array;


/* 接收函数参数字符串 */
Double_array *_handle_digit(char *str);
