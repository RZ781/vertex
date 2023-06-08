#include "parser.h"
#include "debug.h"

// str
str new_str() {
    str s = malloc(sizeof(struct _str));
    NO_NULL(s);
    s->size = 16;
    s->pointer = 0;
    s->data = malloc(s->size);
    NO_NULL(s->data);
    s->data[0] = 0;
    return s;
}
void append(str s, char c) {
    s->data[s->pointer] = c;
    s->pointer++;
    if (s->pointer == s->size) {
        s->size += 16;
        char* data = realloc(s->data, s->size);
        NO_NULL(data);
        s->data = data;
    }
    s->data[s->pointer] = 0;
}

// list
list_t new_list(int size, int incr) {
    list_t list = malloc(sizeof(struct _list));
    NO_NULL(list);
    list->size = size;
    list->incr = incr;
    list->pointer = 0;
    list->data = calloc(size, sizeof(void*));
    NO_NULL(list->data);
    return list;
}
void append_l(list_t list, void* data) {
    list->data[list->pointer] = data;
    list->pointer++;
    if (list->pointer == list->size) {
        list->size += list->incr;
        void** data = realloc(list->data, list->size * sizeof(void*));
        NO_NULL(data);
        list->data = data;
    }
}
// token
int priority(int type, int b) {
    if (type < SYMBOL)
        return 0;
    if (b == OP_EQUALS)
        switch (type - SYMBOL) {
            case '<':
            case '>':
                return 5;
            case '=':
            case '!':
                return 6;
            case '*':
            case '/':
            case '%':
            case '+':
            case '-':
            case '&':
            case '^':
            case '|':
                return 12;
            default:  return 0;
        }
    if (b == OP_DOUBLE)
        switch (type - SYMBOL) {
            case '*': return 1;
            case '<':
            case '>':
                return 4;
            case '&':
                return 10;
            case '|':
                return 11;
            default:  return 0;
        }
    switch (type - SYMBOL) {
        case '*':
        case '/':
        case '%':
            return 2;
        case '+':
        case '-':
            return 3;
        case '<':
        case '>':
            return 5;
        case '&':
            return 7;
        case '^':
            return 8;
        case '|':
            return 9;
        case '=': return 12;
        default:  return 0;
    }
}
int doubled(int type) {
    if (type < SYMBOL)
        return 0;
    switch (type - SYMBOL) {
        case '*':
        case '<':
        case '>':
        case '&':
        case '|':
            return 1;
        default:  return 0;
    }
}

// expr
expr_t new_expr(int size) {
    expr_t expr = malloc(sizeof(struct _expr));
    NO_NULL(expr);
    expr->type = 0;
    expr->list = new_list(size, 1);
    return expr;
}