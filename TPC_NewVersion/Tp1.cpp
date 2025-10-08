#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <iostream>

using namespace std;

char token_buffer[50];
int ind_buffer = 0 , ligneCourant = 1 ;

// tableau des unites lexicale
string tab_tokens[20] = { 
    "BEGIN", "END", "READ", "WRITE", 
    "ID", "INTLITTERAL", "REALLITTERAL", 
    "LPAREN", "RPAREN", "SEMICOLON", "COMMA", 
    "ASSIGNOP", "PLUSOP", "MINUSOP", "SCANEOF"
};



// enumeration
enum token {
    Begin,
    End,
    read,
    write,
    id,
    intLetteral,
    ReelLetteral ,
    lparent,
    rparent,
    semicolon,
    comma,
    assignop,
    plusop,
    minusop,
    SCANEOF
};


void buffer_char(int car) {
    if (ind_buffer < 49) { // éviter débordement
        token_buffer[ind_buffer++] = car;
        token_buffer[ind_buffer] = '\0';
    }
}

token check_reserved() {
    if (!strcmp(token_buffer, "begin"))
        return Begin;
    if (!strcmp(token_buffer, "end"))
        return End;
    if (!strcmp(token_buffer, "read"))
        return read;
    if (!strcmp(token_buffer, "write"))
        return write;

    return id;
}

// detecter les erreur lexicale
void lexical_error(int numLigne , FILE *FSource , char * token_buffer = nullptr) {
    int c ;
    cout << "error in line " << numLigne;
    if(token_buffer )
       cout << " : unknow token " << token_buffer ;

    cout << endl ;
    // while((c=fgetc(FSource)) && c != '\n')  { }
}

// cree les token valide dans une fichier 
void Write_Token(FILE *fdes , token token_ind) {
    if(token_ind == -1) 
      fprintf(fdes  , "\n" );
    else
    {
        fprintf(fdes , "%s  " , tab_tokens[token_ind].c_str() ); 
    }

}

// scanner
token scanner( FILE* FSource , FILE* FDes ) {
    int in_char, c;

    if (FSource == NULL)
        return SCANEOF;
    
    for (int i = 0; i < 50; i++) token_buffer[i] = '\0';

        ind_buffer = 0;

    while ((in_char = fgetc(FSource)) != EOF) {
        

      if (isspace(in_char)) {
        if (in_char == '\n') 
        {   
             ligneCourant++;
             Write_Token(FDes , (token)-1);
        }
          continue;

      }else if (isalpha(in_char)) {

            buffer_char(in_char);
            while ((c = fgetc(FSource)), (isalnum(c) || c == '_')) {
                buffer_char(c);
            }

            ungetc(c, FSource);
            return check_reserved();

        } else if (isdigit(in_char) || in_char=='.') {

            bool exit_PD =false;
            buffer_char(in_char);
            while ((c = fgetc(FSource)), isdigit(c)) {
                exit_PD = true ;
                buffer_char(c);
            }
            
            // exemple . 
            // exit_PD variable pour virifier si il y'a des chiffres apres le point ou non
            if(in_char == '.' && !exit_PD) {
                ungetc(c , FSource);
                lexical_error(ligneCourant , FSource , token_buffer);
                continue ;
            }

            if(c == '.' || c=='e' || c=='E') {

                if(c == '.')
                {
                    // lire les chiffres
                    buffer_char(c);
                   while ((c = fgetc(FSource)), isdigit(c)) { buffer_char(c); } 
                }


            // si il existe une exp
               if( c=='e' || c=='E' ){
                
                //virifier si le car suivant et + ou -  
                buffer_char(c);
                c =  fgetc(FSource);
                buffer_char(c);

                if(c=='+' || c=='-') {
                
                    // passer a le car suivant pour virifier si il est une chiffre
                    c = fgetc(FSource);
                    buffer_char(c);
 
                } 

                // verifier le car qui est apres le exp ou apres le + ou -
                 if(isdigit(c)) {
                    while ((c = fgetc(FSource)), isdigit(c)) { 
                        buffer_char(c);  
                     }
                 }else {
                  // si apres l'exp ou apres le + ou - n'est pas une chiffre
                    ungetc(c , FSource);                 
                    lexical_error(ligneCourant , FSource , token_buffer);
                    continue ;
                 }
               }

             ungetc(c, FSource);
             return ReelLetteral;
           }

           ungetc(c, FSource);
           return intLetteral;

        } else if (in_char == '(') {
            return lparent;

        } else if (in_char == ')') {

            return rparent;
        } else if (in_char == ';') {
            return semicolon;
        } else if (in_char == ',') {
            return comma;
        } else if (in_char == '+') {
            return plusop;
        } else if (in_char == ':') {
            buffer_char(c);
            c = fgetc(FSource);
            if (c == '=')
                return assignop;
            else {
                buffer_char(c);
                ungetc(c, FSource);
                lexical_error(ligneCourant , FSource , token_buffer);
            }
        } else if (in_char == '-') {
            c = fgetc(FSource);
            if (c == '-') {
                while ((c = fgetc(FSource)) != '\n' && c != EOF) {}
                ligneCourant++ ;
                continue ;
            } else {
                ungetc(c, FSource);
                return minusop;
            }
        } else {
            buffer_char(c);
            lexical_error(ligneCourant , FSource , token_buffer);
            // continue ;
        }
    }

    return SCANEOF;
}

int main() {
    token t;
      FILE *FSource = fopen("fSource.txt", "r");
      FILE *FDes= fopen("fDes.txt", "w");
    while ((t = scanner(FSource , FDes)) != SCANEOF) {
        Write_Token(FDes , t);
        // cout << "Token " << t << "  : "<< tab_tokens[t] << endl;
    }
    Write_Token(FDes , SCANEOF);
        fclose(FSource);
         fclose(FDes);

    return 0;
}
