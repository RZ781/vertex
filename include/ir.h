#ifndef IR_H
#define IR_H
#include <stdint.h>

#define MOVE_STMT 1
#define OP_STMT   2
#define LOAD_INT  3

typedef uint64_t var_t;

typedef struct _stmt {
    int type;
    union {
        var_t move;
        struct {var_t a; var_t b; int op;} op;
        int load_int;
    } args;
} stmt_t;

typedef struct _func {
    char* name;
    list_t body;
} func_t;

void generate(list_t stmts);
void gen_e(expr_t e, list_t f);
void print_ir();

#endif