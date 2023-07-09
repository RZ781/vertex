#include "lexer.h"
#include "debug.h"

// token
token_t* new_token(int pos, int line) {
    token_t* token = malloc(sizeof(token_t));
    token->type = START;
    token->data = new_str();
    token->pos  = pos;
    token->line = line;
    token->next = NULL;
    token->whitespace = 0;
    return token;
}

// check keywords
char *kws[9] = {"func", "import", "if", "else", "do", "while", "for", "type", "let"};
char *lkws[6] = {"raise", "return", "yield", "await", "break", "continue"};
char *typenames[] = {"eof", "str", "int", "float", "id", "lkw"};

int is_kw(char* s) {
    for (int i=0; i<9; i++)
        if (strcmp(kws[i], s) == 0) {
            return i+KW;
        }
    return 0;
}
int is_lkw(char* s) {
    for (int i=0; i<6; i++)
        if (strcmp(lkws[i], s) == 0) {
            return 1;
        }
    return 0;
}
char* typestr(int type) {
    if (type >= SYMBOL) {
        char* s = malloc(2);
        NO_NULL(s);
        s[0] = type - SYMBOL;
        s[1] = 0;
        return s;
    }
    if (type >= KW) {
        return kws[type-KW];
    }
    return typenames[type];
}

token_t* lex(FILE* file) {
    int state = START;
    int line = 0;
    int pos = 0;
    int esc = 0;
    int c;
    token_t* token = NULL;
    token_t* start = NULL;
    token_t* temp;
    while (!feof(file)) {
        if (esc != NO_READ)
            c = fgetc(file);
        else
            esc = 0;
        if (c == EOF)
            break;
        pos += 1;
        if (c == '\n') {
            line += 1;
            pos = 0;
        }
        switch (state) {
            case START:
                if (isspace(c)) {
                    if (token != NULL)
                        token->whitespace = 1;
                    continue;
                }
                else if (c == '/') {
                    state = SLASH;
                    continue;
                }
                temp = token;
                token = new_token(pos, line);
                if (start == NULL)
                    start = token;
                else
                    temp->next = token;
                if (c == '"') {
                    state = STR_D;
                    token->type = STR;
                } else if (c == '\'') {
                    state = STR_S;
                    token->type = STR;
                } else if (isdigit(c)) {
                    state = INT_ST;
                    token->type = INT;
                    append(token->data, c);
                } else if (isalpha(c) || c == '_') {
                    state = ID_ST;
                    token->type = ID;
                    append(token->data, c);
                } else {
                    token->type = SYMBOL + c;
                    append(token->data, c);
                }
                break;
            case STR_D:
                if (esc) {
                    append(token->data, '\\');
                    append(token->data, c);
                    esc = 0;
                }
                else if (c == '\\')
                    esc = 1;
                else if (c == '"') {
                    state = START;
                } else if (c == '\n') {
                    fprintf(stderr, "syntax error line %i pos %i: unclosed string\n", line, pos);
                    exit(UNCLOSED_STR);
                } else
                    append(token->data, c);
                break;
            case STR_S:
                if (esc) {
                    append(token->data, '\\');
                    append(token->data, c);
                    esc = 0;
                }
                else if (c == '\\')
                    esc = 1;
                else if (c == '\'') {
                    state = START;
                } else if (c == '\n') {
                    fprintf(stderr, "syntax error line %i pos %i: unclosed string\n", line, pos);
                    exit(UNCLOSED_STR);
                } else
                    append(token->data, c);
                break;
            case INT_ST:
                if (c == '.') {
                    state = FLOAT_ST;
                    token->type = FLOAT;
                    append(token->data, '.');
                } else if (isdigit(c))
                    append(token->data, c);
                else {
                    esc = NO_READ;
                    state = START;
                }
                break;
            case FLOAT_ST:
                if (isdigit(c))
                    append(token->data, c);
                else {
                    esc = NO_READ;
                    state = START;
                }
                break;
            case ID_ST:
                if (c == '_' || isalnum(c))
                    append(token->data, c);
                else {
                    esc = NO_READ;
                    state = START;
                    if (is_lkw(token->data->data))
                        token->type = LKW;
                    else if (is_kw(token->data->data))
                        token->type = is_kw(token->data->data);
                }
                break;
            case COMMENT:
                if (c == '\n')
                    state = START;
                break;
            case MULTILINE:
                if (c == '*')
                    state = MULTI_STAR;
                break;
            case MULTI_STAR:
                if (c == '/')
                    state = START;
                else if (c != '*')
                    state = MULTILINE;
                break;
            case SLASH:
                if (c == '*')
                    state = MULTILINE;
                else if (c == '/')
                    state = COMMENT;
                else {
                    temp = token;
                    token = new_token(pos, line);
                    if (start == NULL)
                        start = token;
                    else
                        temp->next = token;
                    token->type = SYMBOL + '/';
                    append(token->data, '/');
                    state = START;
                    esc = NO_READ;
                }
                break;
        }
    }
    switch (state) {
        case STR_D:
        case STR_S:
        case MULTILINE:
        case MULTI_STAR:
            fprintf(stderr, "syntax error: unexpected eof\n");
            exit(UNEXPECTED_EOF);
    }
    return start;
}
void print_tokens(token_t* start) {
    putchar('[');
    for (token_t* token=start; token != NULL; token = token->next) {
        printf("[\"%s\",\"%s\",%i,%i]", typestr(token->type), token->data->data, token->pos, token->line);
        if (token->next != NULL)
            putchar(',');
    }
    putchar(']');
}
