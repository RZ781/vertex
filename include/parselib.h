#ifndef PARSELIB_H
#define PARSELIB_H
#include "parser.h"
#define FUNC   6  /* is_kw("func");   */
#define IMPORT 7  /* is_kw("import"); */
#define IF     8  /* is_kw("if");     */
#define ELSE   9  /* is_kw("else");   */
#define DO     10 /* is_kw("do");     */
#define WHILE  11 /* is_kw("while");  */
#define FOR    12 /* is_kw("for");    */
#define TYPE   13 /* is_kw("type");   */
#define LET    14 /* is_kw("let");    */

extern int semicolon;
expr_t expr();
expr_t stmt();
expr_t new_expr(int size);
int priority(int type, int b);
int doubled(int type);

void* try_fail(char* file, int line, void* x, int code, char* msg, ...);
type_t parse_type();
void init(token_t* first);
int peek();
int peek1();
token_t* eat(int type);
token_t* eat_err(int type);
token_t* next();
expr_t l_expr();

#endif