#include<string.h>
#include<stdlib.h>
#include"../define.h"
#include"../extend.h"
#include"../calculator/func.h"

char * help(char *str_arg){

	int i;
	/* 获得程序内置函数个数 */
	int func_len = get_func_count();
	/* 命令参数判断 */
	for(i=0; i<func_len; i++)
		if( strcmp(str_arg,func[i]) == 0 ){
			/* str_cpy 是在command函数中从堆中申请的
			 * 比较完成之后就失去了意义，所以把返回字符串放到str_arg
			 * 能够跨函数传送，在main函数中free
			 * 如果出错，在出错处free。 */
			strcpy(str_arg,help_str[i] );
			return str_arg;
		}

	/* 参数为空，返回所有内置函数。 */
	if( strcmp(str_arg,"") == 0 ){
		memset(str_arg,0,MAX_USER_INPUT);
		for(i=0; i<func_len; i++){
			strcat(str_arg, func[i]);
			strcat(str_arg, " ");
		}

		return str_arg;

	}else{
		/* 所有参数都匹配不上，返回参数错误 */
		error_flag = -2;
		free(str_arg);
		return NULL;
	}

}
