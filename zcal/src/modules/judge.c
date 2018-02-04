#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include"extend.h"
#include"define.h"
#include"command.h"

char * str_filter(char *str){

	char *sec_str = my_calloc(MAX_USER_INPUT);

	int len = strlen(str);
	int i;
	int sec_len;

	/* 把字符串所有大写变成小写 */
	for(i=0; i<len; i++)
		str[i] = tolower(str[i]);
	/* 把字符串中空格字符删去 
	 * 新字符串挪到sec_str中 
	 * 注意:fgets得到到字符串是'\n','\0'两个字符结尾
	 * '\n'属于空格字符,所以'\n'也会被过滤掉 */
	sec_len=0;
	for(i=0; i<len; i++)
		if( isspace(str[i])==0 ){
			sec_str[sec_len] = str[i];
			sec_len++;
		}
	memset(str, 0, MAX_USER_INPUT);
	strcpy(str, sec_str);
			
	free(sec_str);
	
	return str;
}


int judge( char *str){

	int i;
	int cmd_count = get_cmd_count();
	int cmd_len;

	/* 判断是否与存在命令相同 */
	for(i=0; i < cmd_count; i++){
		if( memcmp(str, cmd[i], cmd_len = strlen( cmd[i] ) ) == 0
			&& str[cmd_len] == '(' && str[strlen(str)-1] == ')')  {
			
			return 0;
		}
	}
	return 1;
}

