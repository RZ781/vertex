#ifndef PARSER_H
#define PARSER_H
 
#include "lexer.h"
#define IF_EXPR    2
#define IF_ELSE    3
#define WHILE_LOOP 4
#define DO_WHILE   5
#define FOR_LOOP   6
#define FUNC_DEF   7
#define FUNC_CALL  8
#define ID_EXPR    9
#define STR_LIT    10
#define INT_LIT    11
#define BLOCK      12
#define OP_EXPR    13
#define LET_STMT   14
#define LET_VALUE  15
#define ATTRIBUTE  16
#define LKW_EXPR   17

#define OP_DOUBLE  1
#define OP_EQUALS  2

#define CLASS_TYPE    1
#define TEMPLATE_TYPE 2
#define CONST_TYPE    3
#define POINTER_TYPE  4

typedef struct _list {
    int size;
    int incr;
    int pointer;
    void** data;
} *list_t;

typedef struct _expr {
    int type;
    list_t list;
} *expr_t;

typedef struct _type {
    void* data;
    int params;
} *type_t;

void parse_init(token_t* first);
list_t parse();
void append_l(list_t list, void* data);
list_t new_list(int size, int incr);
void* indexl(list_t l, int i);

#endif