#ifndef SCANNER_H
#define SCANNER_H
#include <stdio.h>

typedef enum {
    BEGIN, END, READ, WRITE,
    ID, INTLITERAL,
    ASSIGNOP, PLUSOP, MINUSOP,
    SEMICOLON, COMMA, LPAREN, RPAREN,
    SCANEOF
} token;

extern FILE *fin;
extern FILE *fout;
extern char token_buffer[256];

token scanner(void);

#endif
