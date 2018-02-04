#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"../../extend.h"
#include"../analyze_unit.h"
#include"analyze_unit_bracket.h"

int analyze_unit_bracket(char *str,int current_pos,size_t len){

	if(expect_unit == 2){
		error_flag = 6;
		pos_flag = str+current_pos;
		return -1;
	}

	Bstack * pbs = createBstack();
	Bracket * bra;
	int bracket_level;

	for(;current_pos < len; current_pos++){

		bracket_level = get_bracket_level(str[current_pos]);
		if(bracket_level){
			/* 如果是括号，执行本部分 */

			if(bracket_level % 2 != 0){
				/* 如果是左括号 */
				bra = my_calloc( sizeof(Bracket) );
				bra->pos = current_pos;
				bra->level = bracket_level;
				pushBstack(pbs, bra);
			}else{
				/* 如果是右括号,执行本部分 */
				bra = popBstack(pbs);
				/* 判断弹出的括号是不是与本右括号对应的左括号
				 * 如果不是，则报错
				 * 如果括号栈为空，本字符是右括号，则弹出NULL */
				if( bra != NULL && bra->level == bracket_level-1){
					free(bra);
					if(pbs->len == 0){
						/* 正确执行之后free本函数申请的空间 */
						destroyBstack(pbs);
						return current_pos;
					}

				}else{
					/* 出现括号匹配错误，跳出循环，报错 */
					break;
				}
			}
		}
	}

	/* 本函数出错，错误为括号匹配错误 
	 * 出错之后free本函数申请的空间 */
	destroyBstack(pbs);
	error_flag = 3;
	pos_flag = str + current_pos;
	return -1;
}


int get_bracket_level(char ch){
	switch(ch){
		case '{':
			return 1;
		case '}':
			return 2;
		case '[':
			return 3;
		case ']':
			return 4;
		case '(':
			return 5;
		case ')':
			return 6;
		default:
			return 0;
	}
}

Bstack * createBstack(){
	Bstack * pbs= my_calloc( sizeof(Bstack) );
	pbs->len=0;
	pbs->head = my_calloc( sizeof(Bracket) );
	pbs->head->next = NULL;
	pbs->head->pre = NULL;
	pbs->head->pos = -1;
	pbs->head->level = -1;

	return pbs;
}
void pushBstack(Bstack * pbs, Bracket *push_bra){

	Bracket * bra = pbs->head;
	/* 让bra指向最后一个结点 */
	while(bra->next != NULL)
		bra = bra->next;
	bra->next = push_bra;
	push_bra->next = NULL;
	push_bra->pre = bra;
	(pbs->len)++;

}
Bracket * popBstack(Bstack * pbs){
	if(pbs->len == 0)
		return NULL;
	Bracket * bra = pbs->head;
	/* 让bra指向最后一个结点 */
	while(bra->next != NULL)
		bra = bra->next;
	bra->pre->next = NULL;
	(pbs->len)--;

	return bra;
}
void destroyBstack(Bstack *pbs){
	while(pbs->len != 0)
		free( popBstack(pbs) );
	free(pbs->head);
	free(pbs);
}


