#ifndef DEBUG_H
#define DEBUG_H

#define ERR_ALLOC         1
#define UNCLOSED_STR      2
#define UNEXPECTED_EOF    3
#define UNEXPECTED_TOKEN  4
#define NOT_IMPLEMENTED   5
#define DEFINITION_REQ    6

#ifdef DEBUG_MODE
extern token_t* state;
#define TRACE(s) if (state == NULL) printf(s " eof\n"); else printf(s " %i %i\n", state->line, state->pos)
#define NO_NULL(var) if (var == NULL) {fprintf(stderr, "error: allocation failed at file %s line %i\n", __FILE__, __LINE__); exit(ERR_ALLOC);}
#undef DEBUG_MODE
#define DEBUG_LINE

#else

#define TRACE(s) 
#define NO_NULL(var) if (var == NULL) {fprintf(stderr, "error: allocation failed\n"); exit(ERR_ALLOC);}
#endif

#ifdef DEBUG_LINE
#define TRY_FAIL(x, code, msg, ...) try_fail(__FILE__, __LINE__, x, code, msg, ##__VA_ARGS__)
#define BREAK(msg, ...) fprintf(stderr, "file %s line %i: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define TRY_FAIL(x, code, msg, ...) try_fail("", -1, x, code, msg, ##__VA_ARGS__)
#define BREAK(msg, ...) fprintf(stderr, msg, ##__VA_ARGS__)
#endif

#endif