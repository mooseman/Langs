#ifndef PL0_IDENTIFIER_INFO_H
#define PL0_IDENTIFIER_INFO_H

#include <string.h>


enum ident_type { variable, procedure, constant };


typedef struct ident_info {

  ident_info(char *name, ident_type t); 
  char *get_name()       { return name_; }
  bool is(ident_type t)  { return type_ == t; }
  ident_type type_;
  char *name_;
};



// symbols
typedef struct variable_symbol {

  variable_symbol(char *name, int value)      
  int get_value() { return value_; }
  void set_value(int value) { value_ = value; }
  int value_;
};



typedef struct procedure_symbol {

  procedure_symbol(char *name, int level)      
  int get_level() const { return level_; }
  int level_;
};

#endif



