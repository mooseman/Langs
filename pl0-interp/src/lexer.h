#ifndef PL0_LEXER_H
#define PL0_LEXER_H

#include "token.h"


struct lexer {
  lexer(char *buffer) ;

  token current_token;
  void match(token_type type);
  token get_next_token();
  char peek(int n = 1) ;  

  char *buffer_;
  char *cursor_;
  char *end_;
  token scan_token_();
  token scan_identifier_() ; 
  token scan_integer_() ; 
  void advance_(int n = 1) ; 
  void skip_whitespaces_() ;  
};

#endif






