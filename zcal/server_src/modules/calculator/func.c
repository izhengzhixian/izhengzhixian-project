#include<ctype.h>
#include"cal_tree.h"
#include"func/func_head.h"

/* 已存在的函数 */
char *func[]={"abs","exp","pow","sqrt"};

Result (*func_func[])(char *str) = {
	_func_abs,_func_exp,_func_pow,_func_sqrt};

/* 函数的说明,help函数的输出 */
char *help_str[] = {"abs(x)是求绝对值函数",
	"exp(x)是求自然数e的x次方",
	"pow(x,y)是求x到y次方",
	"sqrt(x)是对x开2次方，sqrt(x,y)是对x开y次方"};

/* 函数参数中可以存在的特殊字符 */
int is_func_arg_char(char ch){
	if(isalpha(ch))
		return 1;
	char avail_punc[]={'.',',','(',')','[',']','{','}'};
	int i;
	for(i=0; i<sizeof(avail_punc); i++)
		if(ch == avail_punc[i])
			return 1;
	return 0;
}

int get_func_count(){
	return sizeof(func) / sizeof(*func) ;
}

