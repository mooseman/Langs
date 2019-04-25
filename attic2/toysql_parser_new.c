

/* toysql_parser.c  */
/* A lexer and parser for a very small subset of SQL.  */
/* This version reads the code from a file. */
 
/* This code is released to the public domain. */
/* "Share and enjoy......"  :)     */
 
 
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
 
 
/* The following code is from here - */ 
/* https://github.com/dorktype/LICE/blob/master/lexer.c  */  
 
 
#include "lexer.h"
#include "util.h"
#include "lice.h" 

 
static list_t *lexer_buffer = &SENTINEL_LIST;

static lexer_token_t *lexer_token_copy(lexer_token_t *token) {
    return memcpy(malloc(sizeof(lexer_token_t)), token, sizeof(lexer_token_t));
}


static lexer_token_t *lexer_identifier(string_t *str) {
    return lexer_token_copy(&(lexer_token_t){
        .type      = LEXER_TOKEN_IDENTIFIER,
        .string    = string_buffer(str)
    });
}


static lexer_token_t *lexer_strtok(string_t *str) {
    return lexer_token_copy(&(lexer_token_t){
        .type      = LEXER_TOKEN_STRING,
        .string    = string_buffer(str)
    });
}


static lexer_token_t *lexer_punct(int punct) {
    return lexer_token_copy(&(lexer_token_t){
        .type      = LEXER_TOKEN_PUNCT,
        .punct     = punct
    });
}


static lexer_token_t *lexer_number(char *string) {
    return lexer_token_copy(&(lexer_token_t){
        .type      = LEXER_TOKEN_NUMBER,
        .string    = string
    });
}


static lexer_token_t *lexer_char(char value) {
    return lexer_token_copy(&(lexer_token_t){
        .type      = LEXER_TOKEN_CHAR,
        .character = value
    });
}


static void lexer_skip_comment_line(void) {
    for (;;) {
        int c = getc(stdin);
        if (c == '\n' || c == EOF)
            return;
    }
}


static void lexer_skip_comment_block(void) {
    enum {
        comment_outside,
        comment_astrick
    } state = comment_outside;

    for (;;) {
        int c = getc(stdin);
        if (c == '*')
            state = comment_astrick;
        else if (state == comment_astrick && c == '/')
            return;
        else
            state = comment_outside;
    }
}
 
 
static int lexer_skip(void) {
    int c;
    while ((c = getc(stdin)) != EOF) {
        if (isspace(c) || c == '\n' || c == '\r')
            continue;
        ungetc(c, stdin);
        return c;
    }
    return EOF;
}


static lexer_token_t *lexer_read_number(int c) {
    string_t *string = string_create();
    string_cat(string, c);
    for (;;) {
        int p = getc(stdin);
        if (!isdigit(p) && !isalpha(p) && p != '.') {
            ungetc(p, stdin);
            return lexer_number(string_buffer(string));
        }
        string_cat(string, p);
    }
    return NULL;
}


static lexer_token_t *lexer_read_string(void) {
    string_t *string = string_create();
    for (;;) {
        int c = getc(stdin);
        if (c == EOF)
            compile_error("Expected termination for string literal");

        if (c == '"')
            break;
        if (c == '\\')
            c = lexer_read_character_escaped();
        string_cat(string, c);
    }
    return lexer_strtok(string);
}


static lexer_token_t *lexer_read_character(void) {
    int c = getc(stdin);
    int r = (c == '\\') ? lexer_read_character_escaped() : c;

    if (getc(stdin) != '\'')
        compile_error("unterminated character");

    return lexer_char((char)r);
}


static lexer_token_t *lexer_read_identifier(int c1) {
    string_t *string = string_create();
    string_cat(string, (char)c1);

    for (;;) {
        int c2 = getc(stdin);
        if (isalnum(c2) || c2 == '_' || c2 == '$') {
            string_cat(string, c2);
        } else {
            ungetc(c2, stdin);
            return lexer_identifier(string);
        }
    }
    return NULL;
}



/*  The main lexer itself. */ 
static lexer_token_t *lexer_read_token(void) {
    int c;
    lexer_skip();

    switch ((c = getc(stdin))) {
        case '0' ... '9':  return lexer_read_number(c);
        case '"':          return lexer_read_string();
        case '\'':         return lexer_read_character();
        case 'a' ... 'z':
        case 'A' ... 'Z':        
        case '_':
                           return lexer_read_identifier(c);

        case '(': case ')':
        case ',': case ';':
        case '[': case ']':
        case '{': case '}':
        case '?': case ':':
        case '~':
                           return lexer_punct(c);

        case EOF:
                           return NULL;

        default:
            compile_error("Unexpected character: `%c`", c);
    }
    return NULL;
}
          
          
           
           
int main(int argc, char **argv) { 
 
FILE *myfile; 
 
myfile = fopen(argv[1], "r"); 
 
if (myfile == NULL) 
   return -1;  
    
lex(myfile);   
 
fclose(myfile);
 
return 0; 
 
}



