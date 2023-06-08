#include <stdio.h>
#include <stdarg.h>
#include "parser.h"
#include "vm.h"
#include "debug.h"

frame_t globals = NULL;
frame_t locals = NULL;

void error(int code, char* msg, ...) {
    va_list args;
    fprintf(stderr, "error: ");
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);
    fputc('\n', stderr);
    exit(code);
}

frame_t get_frame(char* var) {
    frame_t frame = locals;
    while (frame != NULL) {
        if (frame->name != NULL)
            if (strcmp(frame->name, var) == 0)
                return frame;
        frame = frame->next;
    }
    frame = globals;
    while (frame != NULL) {
        if (frame->name != NULL)
            if (strcmp(frame->name, var) == 0)
                return frame;
        frame = frame->next;
    }
    error(UNDEFINED_VAR, "undefined variable %s", var);
    return frame;
}
frame_t push_global() {
    frame_t frame = malloc(sizeof(struct _frame));
    frame->next = globals;
    frame->name = NULL;
    globals = frame;
    return frame;
}
frame_t push_local() {
    frame_t frame = malloc(sizeof(struct _frame));
    frame->next = locals;
    frame->name = NULL;
    locals = frame;
    return frame;
}
frame_t pop_global() {
    frame_t frame = globals;
    globals = frame->next;
    return frame;
}
frame_t pop_local() {
    frame_t frame = locals;
    locals = frame->next;
    return frame;
}
builtin_t push_builtin(char* name) {
    push_global();
    globals->name = name;
    globals->value.type = BUILTIN_FUNC;
    globals->value.pointer = 0;
    globals->value.data = malloc(sizeof(struct _builtin));
    NO_NULL(globals->value.data);
    return globals->value.data;
}
void* indexl(expr_t e, int i) {
    return e->list->data[i];
}
char* data(token_t* t) {
    return t->data->data;
}

value_t exec(expr_t e) {
    value_t ret;
    void* p;
    switch (e->type) {
        case INT_LIT:
            p = malloc(sizeof(int));
            *(int*) p = atoi(data(indexl(e, 0)));
            ret.type = INT_TYPE;
            ret.pointer = 0;
            ret.data = p;
            break;
        case STR_LIT:
            ret.type = CHAR_TYPE;
            ret.pointer = 0;
            ret.data = data(indexl(e, 0));
            break;
        case ID_EXPR:
            ret = get_frame(data(indexl(e, 0)))->value;
            break;
        case FUNC_CALL:
            value_t func = exec(indexl(e, 0));
            if (func.type != BUILTIN_FUNC && func.type != USER_FUNC) {
                error(NOT_CALLABLE, "object is not callable");
            }
            list_t pre_args = indexl(e, 1);
            list_t args = new_list(pre_args->pointer, 1);
            for (int i=0; i<pre_args->pointer; i++) {
                value_t* result = malloc(sizeof(value_t));
                *result = exec(pre_args->data[i]);
                append_l(args, result);
            }
            if (func.type == BUILTIN_FUNC) {
                switch (args->pointer) {
                    case 0:
                        ret = ((builtin_t)(func.data))->f0();
                        break;
                    case 1:
                        ret = ((builtin_t)(func.data))->f1(*(value_t*) args->data[0]);
                        break;
                    case 2:
                        ret = ((builtin_t)(func.data))->f2(*(value_t*) args->data[0], *(value_t*) args->data[1]);
                        break;
                    default:
                        error(ARGUMENT_MISMATCH, "too many arguments");
                }
            } else {
                push_local();
                push_local();
                list_t arg_names = (list_t) indexl(func.data, 0);
                if (arg_names->pointer != args->pointer)
                    error(ARGUMENT_MISMATCH, "incorrect number of arguments");
                for (int i=0; i<args->pointer; i++) {
                    push_local();
                    locals->name = data(arg_names->data[i]);
                    locals->value = *(value_t*) args->data[i];
                }
                ret = exec(indexl(func.data, 3));
                while (locals->name != NULL)
                    free(pop_local());
                free(pop_local());
                free(pop_local());
            }
            break;
        case FUNC_DEF:
            char* name = data(indexl(e, 1));
            ret.type = USER_FUNC;
            ret.pointer = 0;
            ret.data = e;
            if (name == NULL)
                break;
            if (locals == NULL) { // push to globals
                push_global();
                globals->name = name;
                globals->value = ret;
            } else { // push to locals
                push_local();
                locals->name = name;
                locals->value = ret;
            }
            break;
        case BLOCK:
            push_local();
            for (int i=0; i<e->list->pointer; i++)
                exec(indexl(e, i));
            while (locals->name != NULL)
                free(pop_local());
            free(pop_local());
            break;
        case LET_STMT:
            if (locals == NULL) {
                push_global();
                globals->name = data(indexl(e, 0));
            } else {
                push_local();
                locals->name = data(indexl(e, 0));
            }
            break;
        case LET_VALUE:
            value_t value = exec(indexl(e, 2));
            if (locals == NULL) {
                push_global();
                globals->name = data(indexl(e, 0));
                globals->value = value;
            } else {
                push_local();
                locals->name = data(indexl(e, 0));
                locals->value = value;
            }
            break;
        default:
            error(255, "not implemented");
            exit(255);
    }
    return ret;
}