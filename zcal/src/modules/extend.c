#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"define.h"
#include"extend.h"


/* 计算之后的结果 */
double cal_result=0;

/* error_flag用户输入错误标志
 * pos_flag代表出错位置，是辅助标志
 * error_offset代表数组码与错误码的偏移距离
 * 如果输入有多处错误，只显示第一个错误
 * -3代表命令格式错误
 * -2代表命令参数无效
 * -1代表找不到该命令
 * 0代表无错误
 * 1代表用户输入错误函数
 * 2代表用户输入错误数字
 * 3代表括号匹配错误
 * 4代表函数格式错误
 * 5代表小数点错误 
 * 6代表解析单元错误
 * 7代表无效字符
 * 8代表无效运算符
 * 9代表空括号错误
 * 10代表运算符后没有跟操作数
 * 11代表除以0错误
 * 12代表~,&,^,|,操作数必须是整数
 * 13代表对0取余错误 */

int error_flag=0;
char *pos_flag=NULL;
int error_offset=3;


char *str_error[]={"命令格式错误","命令参数无效",
	"找不到该命令","无错误","用户输入错误函数",
	"用户输入错误数字","括号匹配错误","函数格式错误",
	"小数点错误","解析单元错误","无效字符","无效运算符",
	"空括号错误","运算符后没有跟操作数","除以0错误",
	"~,&,^,|,操作数必须是整数","对0取余错误"};


void welcome(){

	printf("========================================");
	printf("========================================\n");
	printf("========================================");
	printf("========================================\n");
	printf("====================");
	printf("+--------------------------------------+");
	printf("====================\n");
	printf("====================");
	printf("|         Welcome to use zcal          |");
	printf("====================\n");
	printf("====================");
	printf("+--------------------------------------+");
	printf("====================\n");
	printf("========================================");
	printf("========================================\n");
	printf("========================================");
	printf("========================================\n\n");
	printf("                    ");
	printf("Enter help() to get bulit-in function\n");
	printf("                ");
	printf("Enter help(func_name) to get function manual\n");
	printf("                         ");
	printf("Enter exit() to exit zcal\n\n");

}

char *explain_error(){

	char *explain_error=my_calloc(MAX_USER_INPUT);
	strcpy(explain_error, str_error[error_flag + error_offset] );

	return explain_error;
}


void * my_calloc(int size){

	void *v = calloc(size,1);
	if (v==NULL)
		exit(1);
	return v;
}
