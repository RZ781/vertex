#include "parser.h"
#include "debug.h"
#include "ir.h"

int main() {
    FILE* file = fopen("main.vx", "r");
    token_t* x = lex(file);
    fclose(file);
    
    parse_init(x);
    list_t stmts = parse();
    
    generate(stmts);
    print_ir();
}