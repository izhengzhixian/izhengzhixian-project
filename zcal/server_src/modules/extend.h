#ifndef _EXTEND_H
#define _EXTEND_H

/********************************************************
 *                  扩展函数区                          *
 * ******************************************************/


/* 打印程序启动欢迎信息 */
void welcome();

/* 解释error_flag的函数，返回解释 */
char * explain_error();

/* 申请初始化0的堆空间，并判断是否成功 */
void * my_calloc(int size);

extern int error_flag;
extern char *pos_flag;
extern double cal_result;

#endif
