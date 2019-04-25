#ifndef PL0_AST_H
#define PL0_AST_H

#include "scope.hpp"
#include "identifier_info.hpp"
#include <string>




typedef struct ast_node {
  
  
};


class expression : public ast_node {
public:
  enum expression_type {
    equal,
    not_equal,
    less_equal,
    less,
    greater,
    greater_equal,
    odd,
    add,
    substract,
    multiply,
    divide,
    plus,
    minus,
    number,
    variable,
  };

  expression_type type;

  expression(expression_type t) : type{t} {}
  virtual ~expression() = default;
};

using expression_ptr = std::unique_ptr<expression>;


class binary_expression : public expression {
public:
  binary_expression(expression_ptr lhs, expression_ptr rhs,
                    expression_type type)
      : left_{std::move(lhs)}, right_{std::move(rhs)}, expression{type} {}

  expression &get_left() const { return *left_; }
  expression &get_right() const { return *right_; }

private:
  expression_ptr left_;
  expression_ptr right_;
};


class number_expression : public expression {
public:
  number_expression(int num) : number_{num}, expression{expression::number} {}

  int get_number() const { return number_; }

private:
  int number_;
};


class unary_expression : public expression {
public:
  unary_expression(expression_ptr expr, expression_type type)
      : expr{std::move(expr)}, expression{type} {}

  expression &get_expr() const { return *expr; }

private:
  expression_ptr expr;
};


class variable : public expression {
public:
  variable(ident_info *target)
      : target_{target}, expression{expression::variable} {}

  ident_info *get_target() const { return target_; }

private:
  ident_info *target_;
};


class statement : public ast_node {
public:
  enum statement_type {
    empty,
    assign,
    call,
    if_cond,
    if_then,
    while_cond,
    while_do,
    read,
    write,
    block,
    begin_end,
    procedure,
  };

  statement_type type;

  statement(statement_type t) : type{t} {}
  virtual ~statement() = default;
};

using statement_ptr = std::unique_ptr<statement>;


class write_statement : public statement {
public:
  write_statement(expression_ptr expr)
      : statement{statement::write}, expr_{std::move(expr)} {}

  expression &get_expr() const { return *expr_; }

private:
  expression_ptr expr_;
};


class assign_statement : public statement {
public:
  assign_statement(expression_ptr lhs, expression_ptr rhs)
      : statement{statement::assign}, left_{std::move(lhs)}, right_{std::move(
                                                                 rhs)} {}

  expression &get_left() const { return *left_; }
  expression &get_right() const { return *right_; }

private:
  expression_ptr left_;
  expression_ptr right_;
};


class empty_statement : public statement {
public:
  empty_statement() : statement{statement::empty} {}
};


class call_statement : public statement {
public:
  call_statement(std::string_view &&proc_name)
      : statement{statement::call}, proc_name_{std::move(proc_name)} {}

private:
  std::string_view proc_name_;
};


class read_statement : public statement {
public:
  read_statement(std::string_view &&var_name)
      : statement{statement::read}, var_name_{std::move(var_name)} {}
  std::string_view get_name() { return var_name_; }

private:
  std::string_view var_name_;
};


class if_statement : public statement {
public:
  if_statement(expression_ptr cond, statement_ptr body_true)
      : statement{statement::if_cond}, condition_{std::move(cond)},
        body_true_{std::move(body_true)} {}

  expression &get_condition() const { return *condition_; }

  statement &get_body() const { return *body_true_; }

private:
  expression_ptr condition_;
  statement_ptr body_true_;
};


class begin_end_statement : public statement {
public:
  begin_end_statement(std::vector<statement_ptr> &&vec)
      : statements_{std::move(vec)}, statement{statement::begin_end} {}

  const std::vector<statement_ptr> &get_statements() const {
    return statements_;
  }

private:
  std::vector<statement_ptr> statements_;
};


class while_statement : public statement {
public:
  while_statement(expression_ptr cond, statement_ptr body)
      : statement{statement::while_cond},
        condition_{std::move(cond)}, body_{std::move(body)} {}

  expression &get_condition() const { return *condition_; }

  statement &get_body() const { return *body_; }

private:
  expression_ptr condition_;
  statement_ptr body_;
};


class procedure_declaration : public statement {
public:
  procedure_declaration(ident_info_ptr ident,
                        statement_ptr bl)
      : statement{statement::procedure},
        procedure_symbol{std::move(ident)}, block{std::move(bl)} {}

private:
  ident_info_ptr procedure_symbol;
  statement_ptr block;
};


class block : public statement {
public:
  block(std::shared_ptr<scope::scope> belonging_scope,
        std::vector<statement_ptr> body)
      : statement{statement::block}, top_{belonging_scope}, body_{std::move(
                                                                body)} {}

  auto get_scope() { return top_; }

  const std::vector<statement_ptr> &get_body() const { return body_; }

private:
  std::vector<statement_ptr> body_;
  std::shared_ptr<scope::scope> top_;
};


#endif





