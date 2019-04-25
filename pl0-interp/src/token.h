#ifndef PL0_TOKEN_H
#define PL0_TOKEN_H

#include <string.h>  


//namespace pl0::lexer {

enum token_type {
  equal,
  not_equal,
  less_equal,
  less,
  greater,
  greater_equal,
  odd,
  integer,
  real,
  plus,
  minus,
  mul,
  div,
  lparen,
  rparen,
  identifier,
  eof,
  unary_minus,
  unary_plus,
  program,
  id,
  empty,
  assign,
  write,
  if_cond,
  if_then,
  while_cond,
  while_do,
  begin,
  end,
  semicolon,
  call,
  read,
  constant,
  var,
  procedure,
  comma,
};


// Definition of token. 
typedef struct token {
  token(token_type type, char *lexeme)            

  token() = default;
  token(token &) = default;
  token operator=(token &) = default;
  token(token &) = default;
  token operator=(token &) = default;

  bool operator_eq(token& rhs) ;
  bool operator_ne(token& rhs) ;

  bool is(token_type type) ;

  bool is_one_of(token_type t1, token_type t2) ;
  bool is_one_of(token_type ilist) ;       
    
  char *get_lexeme() ;
  token_type get_type() ;  

  char *get_name() ;
  int get_integer() ;

  token_type type_;
  char *lexeme_;
};

  void operator_outstream(token t);


#endif


