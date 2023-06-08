#include "parselib.h"
#include "vm.h"
#include "debug.h"

value_t print_f(value_t x) {
    value_t ret;
    ret.type = VOID_TYPE;
    ret.data = NULL;
    ret.pointer = 0;
    switch (x.type) {
        case CHAR_TYPE:
            printf("%s ", (char*) x.data);
            break;
        case VOID_TYPE:
            printf("(void) ");
            break;
        case INT_TYPE:
            printf("%i ", *(int*) x.data);
            break;
        default:
            printf("(not implemented) ");
            break;
    }
    putchar('\n');
    return ret;
}
value_t error0() {
    value_t ret;
    ret.type = VOID_TYPE;
    error(ARGUMENT_MISMATCH, "argument mismatch");
    return ret;
}
value_t error1(value_t) {
    value_t ret;
    ret.type = VOID_TYPE;
    error(ARGUMENT_MISMATCH, "argument mismatch");
    return ret;
}
value_t error2(value_t, value_t) {
    value_t ret;
    ret.type = VOID_TYPE;
    error(ARGUMENT_MISMATCH, "argument mismatch");
    return ret;
}

int main() {
    token_t* x = lex();
    init(x);
    list_t stmts = parse();
    frame_t print = push_global();
    print->name = "print";
    print->value.type = BUILTIN_FUNC;
    print->value.data = malloc(sizeof(builtin_t));
    ((builtin_t)(print->value.data))->f0 = error0;
    ((builtin_t)(print->value.data))->f1 = print_f;
    ((builtin_t)(print->value.data))->f2= error2;
    print->value.pointer = 0;
    for (int i=0; i<stmts->pointer; i++) {
        exec(stmts->data[i]);
    }
}