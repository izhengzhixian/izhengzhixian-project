#include<stdlib.h>
char * cal_exit(char *str_arg){
	free(str_arg);
	exit(0);
	return NULL;
}
