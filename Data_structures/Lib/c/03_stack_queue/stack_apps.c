#include "stack_apps.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "array_stack.h"

int bracket_match(const char *s)
{
    ArrayStack *st = as_create(16);
    if (!st)
        return 0;
    int ok = 1;
    for (; *s && ok; ++s) {
        char c = *s;
        if (c == '(' || c == '[' || c == '{') {
            as_push(st, c);
        } else if (c == ')' || c == ']' || c == '}') {
            int top;
            char want = c == ')' ? '(' : c == ']' ? '[' : '{';
            if (as_pop(st, &top) != 0 || top != want)
                ok = 0;
        }
    }
    if (!as_empty(st))
        ok = 0;
    as_destroy(st);
    return ok;
}

static int is_op(int c)
{
    return c == '+' || c == '-' || c == '*' || c == '/';
}

static int precedence(int op)
{
    return (op == '*' || op == '/') ? 2 : (op == '+' || op == '-') ? 1 : 0;
}

/* 往 out 追加一个字符，保留结尾 '\0' 的位置 */
static int emit(char *out, size_t out_size, size_t *pos, char c)
{
    if (*pos + 1 >= out_size)
        return -1;
    out[(*pos)++] = c;
    return 0;
}

/* 追加一个运算符记号 "op " */
static int emit_op(char *out, size_t out_size, size_t *pos, int op)
{
    if (emit(out, out_size, pos, (char)op) != 0)
        return -1;
    return emit(out, out_size, pos, ' ');
}

int infix_to_postfix(const char *infix, char *out, size_t out_size)
{
    if (out_size == 0)
        return -1;
    ArrayStack *ops = as_create(16);
    if (!ops)
        return -1;
    size_t pos = 0;
    int ok = 1;
    int expect_operand = 1; /* 用于检测 "1 + + 2"、"()" 之类的语法错误 */
    int top;

    for (const char *p = infix; *p && ok; ++p) {
        unsigned char c = (unsigned char)*p;
        if (isspace(c))
            continue;
        if (isdigit(c)) {
            if (!expect_operand) {
                ok = 0;
                break;
            }
            while (isdigit((unsigned char)*p) && ok) {
                ok = emit(out, out_size, &pos, *p) == 0;
                ++p;
            }
            --p;
            ok = ok && emit(out, out_size, &pos, ' ') == 0;
            expect_operand = 0;
        } else if (c == '(') {
            if (!expect_operand) {
                ok = 0;
                break;
            }
            as_push(ops, c);
        } else if (c == ')') {
            if (expect_operand) {
                ok = 0;
                break;
            }
            int found = 0;
            while (as_pop(ops, &top) == 0) {
                if (top == '(') {
                    found = 1;
                    break;
                }
                if (emit_op(out, out_size, &pos, top) != 0) {
                    ok = 0;
                    break;
                }
            }
            ok = ok && found;
        } else if (is_op(c)) {
            if (expect_operand) {
                ok = 0;
                break;
            }
            /* 弹出优先级不低于当前运算符的（左结合） */
            while (as_peek(ops, &top) == 0 && top != '(' && precedence(top) >= precedence(c)) {
                as_pop(ops, NULL);
                if (emit_op(out, out_size, &pos, top) != 0) {
                    ok = 0;
                    break;
                }
            }
            as_push(ops, c);
            expect_operand = 1;
        } else {
            ok = 0;
        }
    }
    if (ok && expect_operand)
        ok = 0; /* 空表达式或以运算符结尾 */
    while (ok && as_pop(ops, &top) == 0) {
        if (top == '(' || emit_op(out, out_size, &pos, top) != 0)
            ok = 0;
    }
    as_destroy(ops);
    if (!ok) {
        out[0] = '\0';
        return -1;
    }
    if (pos > 0 && out[pos - 1] == ' ')
        --pos;
    out[pos] = '\0';
    return 0;
}

int eval_postfix(const char *postfix, int *result)
{
    ArrayStack *st = as_create(16);
    if (!st)
        return -1;
    int ok = 1;
    const char *p = postfix;
    while (*p && ok) {
        unsigned char c = (unsigned char)*p;
        if (isspace(c)) {
            ++p;
        } else if (isdigit(c)) {
            int v = 0;
            while (isdigit((unsigned char)*p))
                v = v * 10 + (*p++ - '0');
            as_push(st, v);
        } else if (is_op(c)) {
            int a, b;
            if (as_pop(st, &b) != 0 || as_pop(st, &a) != 0) {
                ok = 0;
                break;
            }
            int r = 0;
            switch (c) {
            case '+': r = a + b; break;
            case '-': r = a - b; break;
            case '*': r = a * b; break;
            case '/':
                if (b == 0)
                    ok = 0;
                else
                    r = a / b;
                break;
            }
            as_push(st, r);
            ++p;
        } else {
            ok = 0;
        }
    }
    int v = 0;
    if (ok && (as_size(st) != 1 || as_pop(st, &v) != 0))
        ok = 0;
    as_destroy(st);
    if (ok)
        *result = v;
    return ok ? 0 : -1;
}

int eval_infix(const char *infix, int *result)
{
    /* 后缀表达式长度不超过中缀的 2 倍（每个记号后多一个空格） */
    size_t n = strlen(infix) * 2 + 2;
    char *buf = (char *)malloc(n);
    if (!buf)
        return -1;
    int rc = infix_to_postfix(infix, buf, n);
    if (rc == 0)
        rc = eval_postfix(buf, result);
    free(buf);
    return rc;
}
