#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

/* -------------------- flux et buffer -------------------- */
FILE *fin = NULL; /* ouvrira "output.txt" produit par le scanner */
char lexeme[512]; /* pour stocker ID(x) ou INT(123) ... */
token lookahead = UNKNOWN_TOK;
int error_count = 0 , ligne_courant = 1;

/* forward declarations */
token read_token_from_file(void);
token map_line_to_token();

/* Convertit une ligne de output.txt en token enum et met à jour lexeme */
token map_line_to_token()
{
    if (fin == NULL)
        return SCANEOF;
    int car;
    char tmp[100];
    int ind = 0;

    /* initialiser tmp */
    tmp[0] = '\0';

    /* sauter les espaces de debut et du fin de l'UL */
    while ((car = fgetc(fin)) != EOF && isspace(car))
    {
        if(car == (int)'\n')
            ligne_courant++ ;
    }

    if (car == EOF)
        return SCANEOF;

    /* lire le mot (jusqu'au prochain espace ou EOF) */
    while (car != EOF && !isspace(car) && ind < (int)sizeof(tmp) - 1)
    {
        tmp[ind++] = (char)car;
        car = fgetc(fin);
    }
    tmp[ind] = '\0';
    printf("mot lue %s \n", tmp);

    /* Comparaison pour tokens simples */
    if (strcmp(tmp, "BEGIN") == 0)
        return BEGIN;
    if (strcmp(tmp, "END") == 0)
        return END;
    if (strcmp(tmp, "READ") == 0)
        return READ;
    if (strcmp(tmp, "WRITE") == 0)
        return WRITE;
    if (strcmp(tmp, "ASSIGNOP") == 0)
        return ASSIGNOP;
    if (strcmp(tmp, "PLUSOP") == 0)
        return PLUSOP;
    if (strcmp(tmp, "MINUSOP") == 0)
        return MINUSOP;
    if (strcmp(tmp, "SEMICOLON") == 0)
        return SEMICOLON;
    if (strcmp(tmp, "COMMA") == 0)
        return COMMA;
    if (strcmp(tmp, "LPAREN") == 0)
        return LPAREN;
    if (strcmp(tmp, "RPAREN") == 0)
        return RPAREN;
    if (strcmp(tmp, "SCANEOF") == 0)
        return SCANEOF;

    if (strcmp(tmp, "ID") == 0)
    {
        // size_t content_len = len - 4;
        // if (content_len > sizeof(lexeme)-1) content_len = sizeof(lexeme)-1;
        // strncpy(lexeme, tmp + 3, content_len);
        // lexeme[content_len] = '\0';
        return ID;
    }
    if (strcmp(tmp, "INTLITTERAL"))
    {
        // size_t content_len = len - 5;
        // if (content_len > sizeof(lexeme)-1) content_len = sizeof(lexeme)-1;
        // strncpy(lexeme, tmp + 4, content_len);
        // lexeme[content_len] = '\0';
        return INTLITERAL;
    }
    if (strcmp(tmp, "REALLITTERAL"))
    {
        // size_t content_len = len - 6;
        // if (content_len > sizeof(lexeme)-1) content_len = sizeof(lexeme)-1;
        // strncpy(lexeme, tmp + 5, content_len);
        // lexeme[content_len] = '\0';
        return REALLITERAL;
    }
    strcpy(lexeme, tmp);
    return UNKNOWN_TOK;
}

/* Lit la prochaine ligne de output.txt et transforme en token */
token read_token_from_file(void)
{

    return map_line_to_token();
}

/* next_token : renvoie le token courant (lookahead) sans le consommer */
token next_token(void)
{
    return lookahead;
}

/* match : si token courant == t -> consomme (lit le suivant), sinon syntax_error */
void match(token t)
{
    if (lookahead == t && lookahead != SCANEOF)
    {
        lookahead = read_token_from_file();
    }
    else
    {
        syntax_error(t);
        // if (lookahead != SCANEOF) lookahead = read_token_from_file();
    }
}

/* error printing : affiche l'erreur avec token trouvé et attendu */
void syntax_error(token expected)
{
    error_count++;
    const char *expected_s;
    switch (expected)
    {
    case BEGIN:
        expected_s = "BEGIN";
        break;
    case END:
        expected_s = "END";
        break;
    case READ:
        expected_s = "READ";
        break;
    case WRITE:
        expected_s = "WRITE";
        break;
    case ID:
        expected_s = "ID";
        break;
    case INTLITERAL:
        expected_s = "INT";
        break;
    case REALLITERAL:
        expected_s = "REAL";
        break;
    case ASSIGNOP:
        expected_s = "ASSIGNOP";
        break;
    case PLUSOP:
        expected_s = "PLUSOP";
        break;
    case MINUSOP:
        expected_s = "MINUSOP";
        break;
    case SEMICOLON:
        expected_s = "SEMICOLON";
        break;
    case COMMA:
        expected_s = "COMMA";
        break;
    case LPAREN:
        expected_s = "LPAREN";
        break;
    case RPAREN:
        expected_s = "RPAREN";
        break;
    case SCANEOF:
        expected_s = "SCANEOF";
        break;
    default:
        expected_s = "UNKNOWN";
        break;
    }


     fprintf(stderr, "Syntax error in line %d : expected %s \n", ligne_courant , expected_s);

}

/* ---------------------- Fonctions de la grammaire ---------------------- */
void system_goal(void)
{
    lookahead = read_token_from_file();
    program();
    match(SCANEOF);
}

void program(void)
{
    match(BEGIN);
    inst_list();
    match(END);
}

void inst_list(void)
{
    inst();
    while (1)
    {
        token t = next_token();
        if (t == ID || t == READ || t == WRITE)
        {
            inst();
        }
        else
        {
            return;
        }
    }
}

void inst(void)
{
    token t = next_token();
    switch (t)
    {
    case ID:
        match(ID);
        match(ASSIGNOP);
        expression();
        match(SEMICOLON);
        break;
    case READ:
        match(READ);
        match(LPAREN);
        id_list();
        match(RPAREN);
        match(SEMICOLON);
        break;
    case WRITE:
        match(WRITE);
        match(LPAREN);
        expr_list();
        match(RPAREN);
        match(SEMICOLON);
        break;
    default:
        syntax_error(ID);
        // if (lookahead != SCANEOF)
        //     lookahead = read_token_from_file();
        break;
    }
}

void id_list(void)
{
    match(ID);
    while (next_token() == COMMA)
    {
        match(COMMA);
        match(ID);
    }
}

void expression(void)
{
    prim();
    while (1)
    {
        token t = next_token();
        if (t == PLUSOP || t == MINUSOP)
        {
            add_op();
            prim();
        }
        else
            break;
    }
}

void expr_list(void)
{
    expression();
    while (next_token() == COMMA)
    {
        match(COMMA);
        expression();
    }
}

void add_op(void)
{
    token t = next_token();
    if (t == PLUSOP || t == MINUSOP)
        match(t);
    else
        syntax_error(PLUSOP);
}

void prim(void)
{
    token t = next_token();
    switch (t)
    {
    case LPAREN:
        match(LPAREN);
        expression();
        match(RPAREN);
        break;
    case ID:
        match(ID);
        break;
    case INTLITERAL:
        match(INTLITERAL);
        break;
    case REALLITERAL:
        match(REALLITERAL);
        break;
    default:
        syntax_error(ID);
        // if (lookahead != SCANEOF)
        //     lookahead = read_token_from_file();
        break;
    }
}

/* ---------------------- main du parser ---------------------- */
int main(void)
{
    fin = fopen("fDes.txt", "r");
    if (!fin)
    {
        fprintf(stderr, "Impossible d'ouvrir fDes.txt (scanner doit le générer d'abord).\n");
        return 1;
    }

    system_goal();

    if (error_count == 0)
    {
        printf("Parsing termine : aucune erreur de syntaxe detectee.\n");
    }
    else
    {
        printf("Parsing termine : %d erreur(s) de syntaxe detectee(s).\n", error_count);
    }

    fclose(fin);
    return (error_count == 0) ? 0 : 2 ;
}
