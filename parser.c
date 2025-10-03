#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* -------------------- flux et buffer -------------------- */
static FILE *fin = NULL;           /* ouvrira "output.txt" produit par le scanner */
static char lexeme[512];           /* pour stocker ID(x) ou INT(123) ... */
static token lookahead = UNKNOWN_TOK;
static int error_count = 0;

/* forward */
static token read_token_from_file(void);

/* Convertit une ligne de output.txt en token enum et met à jour lexeme */
static token map_line_to_token(const char *line) {
    if (line == NULL) return SCANEOF;

    /* supprimer espaces de début/fin */
    while (*line == ' ' || *line == '\t') line++;
    size_t len = strlen(line);
    while (len > 0 && (line[len-1] == '\n' || line[len-1] == '\r' || line[len-1] == ' ' || line[len-1] == '\t'))
        len--;
    char tmp[512];
    if (len >= sizeof(tmp)) len = sizeof(tmp)-1;
    strncpy(tmp, line, len);
    tmp[len] = '\0';

    /* Comparaison pour tokens simples */
    if (strcmp(tmp, "BEGIN") == 0) return BEGIN;
    if (strcmp(tmp, "END") == 0) return END;
    if (strcmp(tmp, "READ") == 0) return READ;
    if (strcmp(tmp, "WRITE") == 0) return WRITE;
    if (strcmp(tmp, "ASSIGNOP") == 0) return ASSIGNOP;
    if (strcmp(tmp, "PLUSOP") == 0) return PLUSOP;
    if (strcmp(tmp, "MINUSOP") == 0) return MINUSOP;
    if (strcmp(tmp, "SEMICOLON") == 0) return SEMICOLON;
    if (strcmp(tmp, "COMMA") == 0) return COMMA;
    if (strcmp(tmp, "LPAREN") == 0) return LPAREN;
    if (strcmp(tmp, "RPAREN") == 0) return RPAREN;
    if (strcmp(tmp, "SCANEOF") == 0) return SCANEOF;

    /* ID(...)  INT(...)  REAL(...) */
    if (strncmp(tmp, "ID(", 3) == 0 && tmp[len-1] == ')') {
        /* copier le contenu entre parenthèses dans lexeme sans ID( ) */
        size_t content_len = len - 4; /* len minus "ID(" and ")" */
        if (content_len > sizeof(lexeme)-1) content_len = sizeof(lexeme)-1;
        strncpy(lexeme, tmp + 3, content_len);
        lexeme[content_len] = '\0';
        return ID;
    }
    if (strncmp(tmp, "INT(", 4) == 0 && tmp[len-1] == ')') {
        size_t content_len = len - 5;
        if (content_len > sizeof(lexeme)-1) content_len = sizeof(lexeme)-1;
        strncpy(lexeme, tmp + 4, content_len);
        lexeme[content_len] = '\0';
        return INTLITERAL;
    }
    if (strncmp(tmp, "REAL(", 5) == 0 && tmp[len-1] == ')') {
        size_t content_len = len - 6;
        if (content_len > sizeof(lexeme)-1) content_len = sizeof(lexeme)-1;
        strncpy(lexeme, tmp + 5, content_len);
        lexeme[content_len] = '\0';
        return REALLITERAL;
    }

    /* Si on arrive ici, on n'a pas reconnu la ligne -> UNKNOWN_TOK */
    strcpy(lexeme, tmp);
    return UNKNOWN_TOK;
}

/* Lit la prochaine ligne de output.txt et transforme en token */
static token read_token_from_file(void) {
    static char line[512];
    if (fin == NULL) return SCANEOF;
    if (fgets(line, sizeof(line), fin) == NULL) {
        return SCANEOF;
    }
    /* transforme la ligne en token et stocke lexeme si besoin */
    token t = map_line_to_token(line);
    return t;
}

/* next_token : renvoie le token courant (lookahead) sans le consommer */
token next_token(void) {
    return lookahead;
}

/* match : si token courant == t -> consomme (lit le suivant), sinon syntax_error */
void match(token t) {
    if (lookahead == t) {
        /* consommer : lire le token suivant */
        lookahead = read_token_from_file();
    } else {
        syntax_error(t);
        /* tentative de récupération : si le prochain token est t, on le consomme */
        if (lookahead != SCANEOF) lookahead = read_token_from_file();
    }
}

/* error printing : on affiche l'erreur avec le token trouvé et le token attendu */
void syntax_error(token expected) {
    error_count++;
    /* afficher le token attendu lisible */
    const char *expected_s;
    switch (expected) {
        case BEGIN: expected_s = "BEGIN"; break;
        case END: expected_s = "END"; break;
        case READ: expected_s = "READ"; break;
        case WRITE: expected_s = "WRITE"; break;
        case ID: expected_s = "ID"; break;
        case INTLITERAL: expected_s = "INT"; break;
        case REALLITERAL: expected_s = "REAL"; break;
        case ASSIGNOP: expected_s = "ASSIGNOP"; break;
        case PLUSOP: expected_s = "PLUSOP"; break;
        case MINUSOP: expected_s = "MINUSOP"; break;
        case SEMICOLON: expected_s = "SEMICOLON"; break;
        case COMMA: expected_s = "COMMA"; break;
        case LPAREN: expected_s = "LPAREN"; break;
        case RPAREN: expected_s = "RPAREN"; break;
        case SCANEOF: expected_s = "SCANEOF"; break;
        default: expected_s = "UNKNOWN"; break;
    }

    /* trouver nom du token courant pour message */
    const char *found_s;
    switch (lookahead) {
        case BEGIN: found_s = "BEGIN"; break;
        case END: found_s = "END"; break;
        case READ: found_s = "READ"; break;
        case WRITE: found_s = "WRITE"; break;
        case ID: found_s = "ID"; break;
        case INTLITERAL: found_s = "INT"; break;
        case REALLITERAL: found_s = "REAL"; break;
        case ASSIGNOP: found_s = "ASSIGNOP"; break;
        case PLUSOP: found_s = "PLUSOP"; break;
        case MINUSOP: found_s = "MINUSOP"; break;
        case SEMICOLON: found_s = "SEMICOLON"; break;
        case COMMA: found_s = "COMMA"; break;
        case LPAREN: found_s = "LPAREN"; break;
        case RPAREN: found_s = "RPAREN"; break;
        case SCANEOF: found_s = "SCANEOF"; break;
        case UNKNOWN_TOK: found_s = "UNKNOWN"; break;
        default: found_s = "UNKNOWN"; break;
    }

    if (lookahead == ID || lookahead == INTLITERAL || lookahead == REALLITERAL || lookahead == UNKNOWN_TOK) {
        fprintf(stderr, "Syntax error: expected %s but found %s('%s')\n", expected_s, found_s, lexeme);
    } else {
        fprintf(stderr, "Syntax error: expected %s but found %s\n", expected_s, found_s);
    }
}

/* ---------------------- Fonctions de la grammaire ---------------------- */

/* system goal: <system goal> :: <program> SCANEOF */
void system_goal(void) {
    /* initialisation : lookahead = first token */
    lookahead = read_token_from_file();
    program();
    if (lookahead != SCANEOF) {
        syntax_error(SCANEOF);
    }
}

/* <program> :: begin <inst_list> end */
void program(void) {
    match(BEGIN);
    inst_list();
    match(END);
}

/* <inst_list> ::= <inst> { <inst> } */
void inst_list(void) {
    inst();
    /* répéter tant que le prochain token peut commencer une instruction */
    while (1) {
        token t = next_token();
        if (t == ID || t == READ || t == WRITE) {
            inst();
        } else {
            return;
        }
    }
}

/* <inst> ::= ID := <expr> ;  | READ(LIST) ; | WRITE(LIST) ; */
void inst(void) {
    token t = next_token();
    switch (t) {
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
            syntax_error(ID); /* on attendait une instruction */
            /* tentative de récupération : consommer un token pour avancer */
            if (lookahead != SCANEOF) lookahead = read_token_from_file();
            break;
    }
}

/* <id_list> ::= ID { , ID } */
void id_list(void) {
    match(ID);
    while (next_token() == COMMA) {
        match(COMMA);
        match(ID);
    }
}

/* <expr> ::= <prim> { <addop> <prim> } */
void expression(void) {
    prim();
    while (1) {
        token t = next_token();
        if (t == PLUSOP || t == MINUSOP) {
            add_op();
            prim();
        } else break;
    }
}

/* <expr_list> ::= <expr> { , <expr> } */
void expr_list(void) {
    expression();
    while (next_token() == COMMA) {
        match(COMMA);
        expression();
    }
}

/* <add_op> ::= PLUSOP | MINUSOP */
void add_op(void) {
    token t = next_token();
    if (t == PLUSOP || t == MINUSOP) match(t);
    else syntax_error(PLUSOP);
}

/* <prim> ::= ( <expr> ) | ID | INTLITERAL | REALLITERAL */
void prim(void) {
    token t = next_token();
    switch (t) {
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
            /* récupération: consommer un token */
            if (lookahead != SCANEOF) lookahead = read_token_from_file();
            break;
    }
}

/* ---------------------- main du parser ---------------------- */
int main(void) {
    fin = fopen("output.txt", "r"); /* fichier produit par le scanner */
    if (!fin) {
        fprintf(stderr, "Impossible d'ouvrir output.txt (généré par ton scanner). Generer d'abord le fichier avec le scanner.\n");
        return 1;
    }

    system_goal();

    if (error_count == 0) {
        printf("Parsing terminé : aucune erreur de syntaxe detectee.\n");
    } else {
        printf("Parsing termine : %d erreur(s) de syntaxe detectee(s).\n", error_count);
    }

    fclose(fin);
    return (error_count == 0) ? 0 : 2;
}
