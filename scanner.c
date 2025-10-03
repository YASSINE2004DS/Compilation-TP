#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

FILE *fin;   // fichier d'entrée
FILE *fout;  // fichier de sortie
int line_number = 1;
char token_buffer[256];

typedef enum {
    BEGIN, END, READ, WRITE,
    ID, INTLITERAL,
    ASSIGNOP, PLUSOP, MINUSOP,
    SEMICOLON, COMMA, LPAREN, RPAREN,
    SCANEOF
} token;

/* --------------------- utilitaires --------------------- */
void clear_buffer() {
    token_buffer[0] = '\0';
}

void buffer_char(int c) {
    int len = strlen(token_buffer);
    token_buffer[len] = c;
    token_buffer[len+1] = '\0';
}

token check_reserved() {
    if (strcmp(token_buffer, "begin") == 0) return BEGIN;
    else if (strcmp(token_buffer, "end") == 0) return END;
    else if (strcmp(token_buffer, "read") == 0) return READ;
    else if (strcmp(token_buffer, "write") == 0) return WRITE;
    return ID;
}

void lexical_error(int c) {
    // Erreur envoyée dans la console
    fprintf(stderr, "Erreur lexicale ligne %d: caractere invalide '%c'\n", line_number, c);
}

/* --------------------- scanner --------------------- */
token scanner(void) {
    int in_char, c;
    clear_buffer();

    while ((in_char = fgetc(fin)) != EOF) {
        if (isspace(in_char)) {
            if (in_char == '\n') line_number++;
            continue;
        }
        else if (isalpha(in_char)) {
            buffer_char(in_char);
            for (c=fgetc(fin); isalnum(c) || c=='_'; c=fgetc(fin)) {
                buffer_char(c);
            }
            ungetc(c, fin);
            return check_reserved();
        }
        else if (isdigit(in_char)) {
            buffer_char(in_char);
            for (c=fgetc(fin); isdigit(c); c=fgetc(fin)) {
                buffer_char(c);
            }
            ungetc(c, fin);
            return INTLITERAL;
        }
        else if (in_char == '(') return LPAREN;
        else if (in_char == ')') return RPAREN;
        else if (in_char == ';') return SEMICOLON;
        else if (in_char == ',') return COMMA;
        else if (in_char == '+') return PLUSOP;
        else if (in_char == ':') {
            c = fgetc(fin);
            if (c == '=') return ASSIGNOP;
            else {
                ungetc(c, fin);
                lexical_error(in_char);
                continue;   // continuer malgré erreur
            }
        }
        else if (in_char == '-') {
            c = fgetc(fin);
            if (c == '-') { // commentaire
                while (c != '\n' && c != EOF) c = fgetc(fin);
                line_number++;
                continue;
            } else {
                ungetc(c, fin);
                return MINUSOP;
            }
        }
        else {
            lexical_error(in_char);
            continue;  // continuer malgré erreur
        }
    }
    return SCANEOF;
}

/* --------------------- main --------------------- */
int main() {
    fin = fopen("input.txt", "r");
    fout = fopen("output.txt", "w");
    if (!fin || !fout) {
        printf("Erreur ouverture fichier\n");
        return 1;
    }

    token t;
    while ((t = scanner()) != SCANEOF) {
        switch (t) {
            case BEGIN: fprintf(fout, "BEGIN\n"); break;
            case END: fprintf(fout, "END\n"); break;
            case READ: fprintf(fout, "READ\n"); break;
            case WRITE: fprintf(fout, "WRITE\n"); break;
            case ID: fprintf(fout, "ID(%s)\n", token_buffer); break;
            case INTLITERAL: fprintf(fout, "INT(%s)\n", token_buffer); break;
            case ASSIGNOP: fprintf(fout, "ASSIGNOP\n"); break;
            case PLUSOP: fprintf(fout, "PLUSOP\n"); break;
            case MINUSOP: fprintf(fout, "MINUSOP\n"); break;
            case SEMICOLON: fprintf(fout, "SEMICOLON\n"); break;
            case COMMA: fprintf(fout, "COMMA\n"); break;
            case LPAREN: fprintf(fout, "LPAREN\n"); break;
            case RPAREN: fprintf(fout, "RPAREN\n"); break;
            default: break;
        }
    }

    fclose(fin);
    fclose(fout);
    return 0;
}
