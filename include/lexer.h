#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdint.h>
#define NO_READ 2

#define START      0
#define STR_D      1
#define STR_S      2
#define INT_ST     3
#define FLOAT_ST   4
#define ID_ST      5
#define COMMENT    6
#define MULTILINE  7
#define MULTI_STAR 8
#define SLASH      9

#define STR 1
#define INT 2
#define FLOAT 3
#define ID 4
#define LKW 5
#define WHITESPACE 80
#define SYMBOL 100
#define KW 6

typedef struct _str {
    int size;
    int pointer;
    char* data;
} *str;

typedef struct _token {
    uint16_t type;
    str data;
    uint8_t whitespace;
    uint16_t pos;
    uint16_t line;
    struct _token* next;
} token_t;

str new_str();
void append(str s, char c);
char* typestr(int type);
int is_lkw(char* s);
int is_kw(char* s);
token_t* lex();
void print_tokens(token_t* start);

#endif