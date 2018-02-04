#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"define.h"
#include"extend.h"
#include"command.h"
#include"command/help.h"
#include"command/cal_exit.h"


/* 已存在的命令 */
char *cmd[] = {"help","exit" };
/* 命令函数指针数组 */
char* (*cmd_func[])(char * str_arg)={ help,cal_exit };


char *command(char * str){

	/* 用户输入命令左括号位置 */
	int l_bracket = -1;

	int i;
	int str_len = strlen(str);
	for(i=0; i<str_len; i++)
		if( (str[i] == '(') ){
			l_bracket = i;
			break;
		}

	/* 命令名 */
	char str_command[MAX_USER_INPUT];
	memcpy(str_command,str,l_bracket);
	/* 因为str_command是栈上的数组，所有存在垃圾数据
	 * memcpy不会在结尾添加'\0'，需要我们手动添加 */
	str_command[l_bracket]=0;

	/* 得到命令参数 */
	char *str_arg = my_calloc(MAX_USER_INPUT);
	memcpy(str_arg, str+l_bracket+1, str_len-l_bracket-2);

	int cmd_count = get_cmd_count();

	for(i=0; i<cmd_count; i++)
		if( strcmp(cmd[i],str_command) == 0 ){
			/* str_arg传入参数后，就失去了它的作用，刚好用来接收命令执行后的结果
			 * 如果在命令执行过程中产生错误，str_arg将会被free掉，然后执行结果为NULL */
			str_arg = cmd_func[i](str_arg);
			break;
		}


	return str_arg;

}

int get_cmd_count(){
	return sizeof(cmd) / sizeof(*cmd);
}
