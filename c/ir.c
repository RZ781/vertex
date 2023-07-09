#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "parser.h"
#include "debug.h"
#include "ir.h"

// n N

list_t code;

void error(int code, char* msg, ...) {
    va_list args;
    fprintf(stderr, "error: ");
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fputc('\n', stderr);
    exit(code);
}
void generate(list_t stmts) {
    code = new_list(16, 16);
    for (int i=0; i<(stmts->pointer); i++) {
        gen_e(indexl(stmts, i), NULL);
    }
}
void gen_e(expr_t e, list_t f) {
    stmt_t* stmt;
    switch (e->type) {
        case INT_LIT:
            stmt = malloc(sizeof(stmt_t));
            stmt->type = LOAD_INT;
            append_l(f, stmt);
            break;
        case 
        default:
            error(NOT_IMPLEMENTED, "not implemented");
    }
}
void print_ir() {
    for (int i=0; i<code->pointer; i++) {
        func_t f = *(func_t*) code->data[i];
        printf("%s\n", f.name);
    }
}