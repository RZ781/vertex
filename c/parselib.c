#include <stdarg.h>
#define DEBUG_LINE
#include "parser.h"
#include "debug.h"

token_t* state;
int semicolon;

void* try_fail(char* file, int line, void* x, int code, char* msg, ...) {
    if (x == NULL) {
        va_list args;
        if (line != -1)
            fprintf(stderr, "file %s line %i:\n", file, line); 
        if (state == NULL)
            fprintf(stderr, "syntax error at eof: ");
        else
            fprintf(stderr, "syntax error line %i pos %i: ", state->line, state->pos);
        va_start(args, msg);
        vfprintf(stderr, msg, args);
        va_end(args);
        fputc('\n', stderr);
        exit(code);
    };
    return x;
}

void init(token_t* first) {
    state = first;
    semicolon = 1;
}
int peek() {
    if (state == NULL)
        return 0;
    return state->type;
}
int peek1() {
    if (state == NULL)
        return 0;
    if (state->next == NULL)
        return 0;
    return state->next->type;
}
token_t* next() {
    TRACE("next");
    if (state == NULL) {
        return NULL;
    }
    token_t* prev_ = state;
    state = state->next;
    return prev_;
}
token_t* eat(int type) {
    TRACE("eat");
    if (state == NULL)
        return NULL;
    if (state->type != type)
        return NULL;
    return next();
}