#include "token.h"
//#include "utils.h"
#include <string.h>


char *token_type_names[]{
    "odd",        "equal",         "not_equal",   "less_equal", "less",
    "greater",    "greater_equal", "integer",     "real",       "plus",
    "minus",      "mul",           "div",         "lparen",     "rparen",
    "identifier", "eof",           "unary minus", "unary plus", "program",
    "id",         "empty",         "assign",      "write",      "if cond",
    "if then",    "while cond",    "while do",    "begin",      "end",
    "semicolon",  "call",          "read",        "const",      "var",
    "procedure",  "comma";
}


bool is_one_of(token_type ilist) {
  for (&tok_type in ilist)
    if (is(tok_type))
      return true;

  return false;
}


bool is(token_type type) { 
  return get_type() == type; 
}


bool is_one_of(token_type t1, token_type t2) {
  return is(t1) || is(t2);
}


char *get_lexeme() { 
  return lexeme_; 
}


token_type get_type() { 
  return type_; 
}


char *get_name() {
  return token_type_names[static_cast<int>(get_type())];
}


int get_integer() {
  if (is(token_type::integer))
    return atoi( get_lexeme() );
  else
    printf("Cannot cast token lexeme to int", %s);
}


void operator_outstream(token &tok) { 
  printf("token{type:", %s); 
  printf(tok.get_name() , %s) ; 
  printf("lexeme: " %s) ; 
  printf(tok.get_lexeme() , %s) ; 
}


bool operator_eq(token& rhs) {
  return (lexeme_, type_) == (rhs.lexeme_, rhs.type_);
}


bool operator_ne(const token& rhs) {
  return !(*this == rhs);
}



