#ifndef PARSER_H
#define PARSER_H

/* Même énum des tokens que la sortie de ton scanner */
typedef enum {
    BEGIN, END, READ, WRITE,
    ID, INTLITERAL, REALLITERAL,
    ASSIGNOP, PLUSOP, MINUSOP,
    SEMICOLON, COMMA, LPAREN, RPAREN,
    SCANEOF, UNKNOWN_TOK
} token;

/* prototypes */
void system_goal(void);
void program(void);
void inst_list(void);
void inst(void);
void id_list(void);
void expression(void);
void expr_list(void);
void add_op(void);
void prim(void);
void syntax_error(token expected);
void match(token t);
token next_token(void);



#endif
