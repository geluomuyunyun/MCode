/*
 * stack_apps.h - 栈的经典应用。
 *
 * 表达式支持：非负整数、+ - * /、圆括号、空格。整数除法向零取整。
 * 后缀表达式中每个记号（数字或运算符）之间用一个空格分隔，例如 "3 4 2 * +"。
 */
#ifndef DSA_C_STACK_APPS_H
#define DSA_C_STACK_APPS_H

#include <stddef.h>

/* 括号匹配，支持 () [] {}，匹配返回 1，否则返回 0 */
int bracket_match(const char *s);

/* 中缀转后缀（调度场算法）。成功返回 0；语法错误或 out 空间不足返回 -1 */
int infix_to_postfix(const char *infix, char *out, size_t out_size);

/* 计算后缀表达式。成功返回 0；语法错误或除零返回 -1 */
int eval_postfix(const char *postfix, int *result);

/* 中缀表达式求值 = 中缀转后缀 + 后缀求值 */
int eval_infix(const char *infix, int *result);

#endif
