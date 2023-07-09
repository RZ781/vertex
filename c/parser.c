#include <stdarg.h>
#define DEBUG_LINE
#include "parselib.h"
#include "debug.h"

token_t* eat_err(int type) {
    return TRY_FAIL(eat(type), UNEXPECTED_TOKEN, "expected %s, got %s", typestr(type), typestr(peek()));
}
type_t type_err() {
    return TRY_FAIL(parse_type(), UNEXPECTED_TOKEN, "unexpected %s", typestr(peek()));
}

list_t parse() {
    list_t stmts = new_list(8, 8);
    while (peek() != 0) {
        append_l(stmts, stmt());
    }
    return stmts;
} 

expr_t stmt() {
    TRACE("stmt");
    if (peek() == SYMBOL + ';') {
        eat_err(SYMBOL + ';');
        return NULL;
    }
    expr_t e = expr(-1);
    if (semicolon)
        eat_err(SYMBOL + ';');
    return e;
}

expr_t expr(int prio) {
    TRACE("expr");
    if (prio == -1)
        prio = 12;
    if (prio == 0)
        return TRY_FAIL(l_expr(), UNEXPECTED_TOKEN, "unexpected %s", typestr(peek()));
    expr_t e1 = expr(prio-1);
    int a = peek();
    int b = 0;
    if (peek1() == SYMBOL + '=') {
        b = OP_EQUALS;
    } else if (doubled(a) && a == peek1()) {
        b = OP_DOUBLE;
    }
    if (priority(a, b) == prio) {
        next();
        expr_t ret = new_expr(4);
        if (b & OP_DOUBLE)
            eat_err(a);
        if (b & OP_EQUALS)
            eat_err(SYMBOL + '=');
        int *data = malloc(sizeof(int)*2);
        NO_NULL(data);
        data[0] = a;
        data[1] = b;
        append_l(ret->list, data);
        append_l(ret->list, data+1);
        append_l(ret->list, e1);
        append_l(ret->list, expr(prio));
        ret->type = OP_EXPR;
        semicolon = 1;
        return ret;
    } 
    return e1;
}
type_t parse_type() {
    TRACE("parse_type");
    type_t t = malloc(sizeof(struct _type));
    NO_NULL(t);
    token_t* e = next();
    switch (e->type) {
        case ID:
            t->params = CLASS_TYPE;
            t->data = e;
            break;
        case SYMBOL + '(':
            free(t);
            t = parse_type();
            eat_err(SYMBOL + ')');
            break;
        case SYMBOL + '*':
            t->params = POINTER_TYPE;
            t->data = parse_type();
            break;
        default:
            return NULL;
    }
    while (peek() == SYMBOL + '<') {
        type_t temp = malloc(sizeof(struct _type));
        temp->data = new_list(2, 2);
        append_l(temp->data, t);
        t = temp;
        eat_err(SYMBOL + '<');
        do {append_l(t->data, type_err());} while (eat(SYMBOL + ','));
        eat_err(SYMBOL + '>');
    }
    return t;
}

expr_t l_expr() {
    TRACE("l_expr");
    token_t* e = next();
    if (e == NULL)
        return NULL;
    expr_t ret;
    type_t t = NULL;
    switch (e->type) {
        case FUNC:
            ret = new_expr(4);
            list_t args = new_list(3, 1);
            append_l(ret->list, args);
            append_l(ret->list, eat(ID));
            eat_err(SYMBOL + '(');
            while (peek() != SYMBOL + ')') {
                append_l(args, eat_err(ID));
                if (peek() != SYMBOL + ')') {
                    eat_err(SYMBOL + ',');
                }
            }
            eat_err(SYMBOL + ')');
            if (peek() == SYMBOL + ':') {
                eat_err(SYMBOL + ':');
                t = type_err();
            }
            append_l(ret->list, t);
            append_l(ret->list, expr(-1));
            ret->type = FUNC_DEF;
            break;
        case LET:
            ret = new_expr(2);
            token_t* id = eat_err(ID);
            if (peek() == SYMBOL + ':') {
                eat_err(SYMBOL + ':');
                t = type_err();
            }
            append_l(ret->list, id);
            append_l(ret->list, t);
            if (peek() == SYMBOL + '=') {
                eat_err(SYMBOL + '=');
                expr_t val = expr(-1);
                ret->type = LET_VALUE;
                append_l(ret->list, val);
            } else
                ret->type = LET_STMT;
            semicolon = 1;
            break;
        case IF:
            ret = new_expr(2);
            expr_t cond = expr(-1);
            expr_t body = expr(-1);
            append_l(ret->list, cond);
            append_l(ret->list, body);
            if (peek() == ELSE) {
                eat_err(ELSE);
                expr_t else_ = expr(-1);
                append_l(ret->list, else_);
                ret->type = IF_ELSE;
            } else
                ret->type = IF_EXPR;
            break;
        case WHILE:
            ret = new_expr(2);
            append_l(ret->list, expr(-1)); // cond
            append_l(ret->list, expr(-1)); // body
            ret->type = WHILE_LOOP;
            break;
        case DO:
            ret = new_expr(2);
            append_l(ret->list, expr(-1)); // body
            eat_err(WHILE);
            append_l(ret->list, expr(-1)); // cond
            ret->type = DO_WHILE;
            semicolon = 1;
            break;
        case SYMBOL + '{':
            ret = new_expr(3);
            ret->type = BLOCK;
            while (peek() != SYMBOL + '}')
                append_l(ret->list, stmt());
            eat_err(SYMBOL + '}');
            semicolon = 0;
            break;
        case SYMBOL + '(':
            ret = expr(-1);
            eat_err(SYMBOL + ')');
            semicolon = 1;
            break;
        case LKW:
            ret = new_expr(2);
            ret->type = LKW_EXPR;
            append_l(ret->list, e);
            append_l(ret->list, expr(-1));
            break;
        case ID:
            ret = new_expr(1);
            ret->type = ID_EXPR;
            append_l(ret->list, e);
            semicolon = 1;
            break;
        case STR:
            ret = new_expr(1);
            ret->type = STR_LIT;
            append_l(ret->list, e);
            semicolon = 1;
            break;
        case INT:
            ret = new_expr(1);
            ret->type = INT_LIT;
            append_l(ret->list, e);
            semicolon = 1;
            break;
        default:
            return NULL;
    }
    while (peek() == SYMBOL + '(' || peek() == SYMBOL + '.') {
        expr_t temp = new_expr(2);
        append_l(temp->list, ret);
        if (peek() == SYMBOL + '.') {
            temp->type = ATTRIBUTE;
            eat_err(SYMBOL + '.');
            append_l(temp->list, eat_err(ID));
            semicolon = 1;
        } else {
            list_t args = new_list(1, 1);
            append_l(temp->list, args);
            temp->type = FUNC_CALL;
            eat_err(SYMBOL + '(');
            while (peek() != SYMBOL + ')') {
                append_l(args, expr(-1));
                if (peek() != SYMBOL + ')') {
                    eat_err(SYMBOL + ',');
                }
            }
            eat_err(SYMBOL + ')');
            semicolon = 1;
        }
        ret = temp;
    }
    return ret;
}