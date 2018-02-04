#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"modules/define.h"
#include"modules/extend.h"

char * zcal(char *str);

int main(int argc, char **argv){

	char *str = my_calloc(MAX_USER_INPUT);
    char *out;
	size_t length;
	welcome();

	while(1){
		printf(">> ");	
		fgets(str,MAX_USER_INPUT,stdin);
        out = zcal(str);
        printf("%s\n", out);
        free(out);
	}

	return 0;
}
