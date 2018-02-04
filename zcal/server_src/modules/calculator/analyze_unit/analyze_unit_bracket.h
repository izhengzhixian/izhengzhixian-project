#ifndef _ANALYZE_UNIT_BRACKET_H
#define _ANALYZE_UNIT_BRACKET_H

/* 用于解析表达式中括号用到的栈的结点 */
typedef struct cal_bracket Bracket;
struct cal_bracket{
	int pos;	//括号所在位置
	/* 括号等级,1，3，5分别代表左大括号，左中括号，左小括号
	 * 2，4，6分别代表右大括号，右中括号，右小括号 */
	int level;	
	Bracket * next;
	Bracket * pre;
};

/* 括号栈 */
typedef struct bracket_stack Bstack;
struct bracket_stack{
	int len;
	Bracket * head;
};




/* 获取括号字符的等级，括号等级按照括号结构体里的等级计算
 * 如果不是括号，返回0 */
int get_bracket_level(char ch);


/* 生成一个括号栈 */
Bstack * createBstack();
/* 压入括号结点到括号栈 */
void pushBstack(Bstack * pbs, Bracket *push_bra);
/* 弹出括号结点到括号栈 */
Bracket * popBstack(Bstack * pbs);
/* 销毁括号栈,返回值是参数 */
void destroyBstack(Bstack *pbs);



/* analyze_unit的子函数
 * 参数是analyze_unit的参数，和当前解析字符串的位置
 * 返回左括号匹配的右括号的位置，出错返回-1 */
int analyze_unit_bracket(char *str,int current_pos,size_t len);


#endif
