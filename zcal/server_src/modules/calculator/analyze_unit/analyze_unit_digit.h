#ifndef _ANALYZE_UNIT_DIGIT_H
#define _ANALYZE_UNIT_DIGIT_H

/* 匹配数字单元模块，出错返回-1 */
int analyze_unit_digit(char *str, int current_pos, size_t len);


/* 如果是数字返回1，如果是'.'返回2,否则返回0 */
int isdigit_dot(char ch);

#endif
