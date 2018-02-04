#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"extend.h"
#include"calculator/analyze_unit.h"
#include"calculator/generate_tree.h"
#include"calculator/cal_tree.h"

int calculator(char *str){
	/* 每次计算都要清空期望单元值 */
	expect_unit = 0;
	/* 结果也清空 */
	cal_result = 0;
	/* 对计算器表达式解析，并生成一个栈 */
	Ustack * pus = analyze_unit(str, strlen(str));

	/* 将栈转化成树 */
	Unit * unit_tree = generate_tree(pus);

	/* 计算树 */
	return cal_tree(unit_tree);

	/* 需要注意的是，如果上面的函数运行中，出现任何错误，
	 * 都会直接返回NULL，然后下一个函数接收到NULL，
	 * 或者运行错误，也会返回NULL。 */

}
