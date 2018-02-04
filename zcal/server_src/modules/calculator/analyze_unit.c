#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include"../extend.h"
#include"operator.h"
#include"generate_tree.h"
#include"analyze_unit.h"
#include"analyze_unit/analyze_unit_bracket.h"
#include"analyze_unit/analyze_unit_alpha.h"
#include"analyze_unit/analyze_unit_digit.h"
#include"analyze_unit/analyze_unit_operator.h"

int expect_unit = 0;


/* 本程序核心部分，接收一个计算器表达式，并将其分解成多个单元
 * 单元分成两类，数据单元和符号单元，数据单元包括
 * 括号单元，函数单元，数字单元，符号单元包括各种运算符
 * 括号单元把括号去掉，然后交给本函数递归
 * 最终生成的树只包括函数单元，数字单元，符号单元 */

Ustack * analyze_unit(char *str, size_t len){


	if (len < 1){
	/* 正常情况下，len<1的情况是不会出现的
	 * 因为这种情况在main函数中就被拦截了，而只会在递归时出现
	 * 此时str指向'('字符，所以pos_flag = str - 1 */
		error_flag = 9;
		pos_flag = str -1;

		return NULL;
	}

	/* 注意：在本函数递归过程中，expect_unit这个标志会被覆盖
	 * 所以，需要把这个标志储存下来，然后清空expect_unit
	 * 函数调用完成之后恢复expect_unit的值，函数出错不用恢复 */
	int expect_unit_store = expect_unit;
	expect_unit = 0;

	
	Ustack * pus = createUstack();
	Ustack * qus;
	Unit *u;
	int unit_end_pos;
	int i,j;
	for(i=0; i<len; i++){
		
		if(  get_bracket_level(str[i]) ){
			/* 右括号字符也会使用本分支 */

			unit_end_pos = analyze_unit_bracket(str, i, len);
			if(unit_end_pos == -1){
				destroyUstack(pus);
				return NULL;
			}
			qus = analyze_unit(str+i+1,unit_end_pos-i-1) ;
			u = generate_tree(qus);
			/* 如果运行没出错误的话，会生成一棵树
			 * 把这棵树的根放入栈，按普通数据单元对待 */
			if(u){
				u->level = 0;
				pushUstack(pus,u);
			}else{
				destroyUstack(pus);
				return NULL;
			}
			i = unit_end_pos;
			expect_unit = 2;
		}else if(isalpha( str[i]) || str[i] == '_'){
			
			unit_end_pos = analyze_unit_alpha(str, i, len);
			if(unit_end_pos == -1){
				destroyUstack(pus);
				return NULL;
			}
			u = my_calloc(sizeof(Unit));
			u->str = str+i;
			u->len = unit_end_pos -i +1;
			u->unit_flag = -3;
			u->level = 0;
			pushUstack(pus,u);
			i = unit_end_pos;
			expect_unit = 2;
		}else if( isdigit_dot(str[i]) ){

			unit_end_pos = analyze_unit_digit(str, i, len);
			if(unit_end_pos == -1){
				destroyUstack(pus);
				return NULL;
			}
			u = my_calloc(sizeof(Unit));
			u->str = str+i;
			u->len = unit_end_pos -i +1;
			u->unit_flag = -2;
			u->level = 0;
			pushUstack(pus,u);
			i = unit_end_pos;
			expect_unit = 2;
		}else if( is_operator_char(str[i]) ){

			unit_end_pos = analyze_unit_operator(str, i, len);
			if(unit_end_pos == -1){
				destroyUstack(pus);
				return NULL;
			}
			char *ch_p;
			u = my_calloc(sizeof(Unit));
			u->str = str+i;
			u->len = unit_end_pos -i +1;
			u->unit_flag = get_operator_pos(str+i,unit_end_pos-i+1);
			u->num_flag = expect_unit == 2? 2: 1;
			if(u->num_flag == 1){
				ch_p = get_operator_level("~",1);
				u->level = ch_p[0];
				free(ch_p);
			}else{
				ch_p = get_operator_level(u->str, u->len);
				u->level = ch_p[0];
				free(ch_p);
			}

			pushUstack(pus,u);
			i = unit_end_pos;
			expect_unit = 3;
		}else{
			/* 无效字符 */
			destroyUstack(pus);
			error_flag = 7;
			pos_flag = str + i;
			return NULL;
		}


	}

	if(expect_unit == 3){
		/* 运算符后没有跟操作数 */
		destroyUstack(pus);
		error_flag = 10;
		pos_flag = str + i -1;
		return NULL;
	}

	/* 恢复expect_unit的值 */
	expect_unit = expect_unit_store;
	
	return pus;

}


Ustack *createUstack(){
	Ustack *pus = my_calloc(sizeof(Ustack));
	pus->len = 0;
	pus->head = my_calloc(sizeof(Unit));
	pus->head->pre == NULL;
	pus->head->next == NULL;

	return pus;
}

void pushUstack(Ustack *pus, Unit *en_unit){
	Unit *u_test = pus->head;
	while(u_test->next != NULL)
		u_test = u_test->next;
	u_test->next = en_unit;
	en_unit->pre = u_test;
	en_unit->next = NULL;
	(pus->len)++;

}

Unit *popUstack(Ustack *pus){

	Unit *u_end = pus->head;


	if(pus->len == 0 ){
		return NULL;
	}else{

		while(u_end->next != NULL)
			u_end = u_end->next;

		u_end->pre->next = NULL;
		(pus->len)--;

		return u_end;
	}
}

void destroyUstack(Ustack * pus){
	while(pus->len != 0)
		popUstack(pus);
	free(pus->head);
	free(pus);
}
