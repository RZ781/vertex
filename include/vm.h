#ifndef VM_H
#define VM_H
#include "parser.h"

#define VOID_TYPE    0
#define INT_TYPE     1
#define CHAR_TYPE    2
#define BUILTIN_FUNC 3
#define USER_FUNC    4

typedef struct _value {
    int type;
    int pointer;
    void* data;
} value_t;

typedef struct _builtin {
    value_t (*f0)();
    value_t (*f1)(value_t);
    value_t (*f2)(value_t, value_t);
} *builtin_t;

typedef struct _frame {
    char* name;
    value_t value;
    struct _frame* next;
} *frame_t;


value_t exec(expr_t e);
frame_t push_global();
frame_t pop_global();
void error(int code, char* msg, ...);

#endif