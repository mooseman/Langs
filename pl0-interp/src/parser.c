#include "ast.h"
#include "lexer.h"
#include <stdio.h>
#include <string.h>



struct parser {
  parser(lexer lex) lexer_(lex)

  lexer_->get_next_token();

  ast_block program();
  ast_block block();

  ast_statement_ptr procedure_declaration();
  ast_statement_ptr constant_var_declaration();
  ast_statement_ptr variable_declaration();

  ast_statement_ptr statement();

  ast_statement_ptr begin_end_statement();
  ast_statement_ptr while_statement();
  ast_statement_ptr if_statement();
  ast_statement_ptr call_statement();
  ast_statement_ptr read_statement();
  ast_statement_ptr write_statement();
  ast_statement_ptr assign_statement();

  ast_expression_ptr condition();
  ast_expression_ptr expression();
  ast_expression_ptr term();
  ast_expression_ptr factor();

  lexer lexer_;
  scope top_;
};


// const ident = number, ident2 = number;
// var ident, ident2, ident3;
// procedure ident; const ident3 = lol;

ast_statement_ptr procedure_declaration() {

  lexer_->match(token_type->procedure);

  auto id = lexer_->current_token.get_lexeme();

  lexer_->match(token_type->identifier);

  lexer_->match(token_type->semicolon);
  
  ast_block bl = block();

  lexer_->match(token_type->semicolon);

  return ast_procedure_declaration(std::move(proc),
         std::move(bl));
}


ast_statement_ptr  constant_var_declaration() {

  lexer_->match(token_type->constant);

  auto id = lexer_->current_token.get_lexeme();

  lexer_->match(token_type->identifier);

  lexer_->match(token_type->equal);

  auto tok = lexer_->current_token;

  lexer_->match(token_type->integer);

  auto var = std::make_unique<scope::variable_symbol>(id, tok.get_integer());

  top_->define(std::move(var));

  lexer_->match(token_type::semicolon);

  return nullptr;
}


ast_statement_ptr variable_declaration() {

  // zmienic petle ale ogolnie jest ok
  
  lexer_->match(token_type->var);

  for (;;) {
    auto id = lexer_->current_token.get_lexeme();

    lexer_->match(token_type->identifier);

    auto var = (id, 0);

    top_->define(var);

    if (lexer_->current_token.is(token_type->comma)) {
      lexer_->match(token_type->comma);
      continue;
    } else
      break;
  }
  lexer_->match(token_type->semicolon);

  return nullptr;
}


// rob push

ast_block program() {

ast_block parser->block() {
  auto constant_variables =
      lexer_->current_token.is(token_type->constant)
          ? constant_var_declaration()
          : nullptr;
  auto variables = lexer_->current_token.is(token_type->var)
          ? variable_declaration()
          : nullptr;

  // Array 
  ast_statement_ptr sub_methods;
  for (;;) {
    if (lexer_->current_token.is(token_type->procedure))
      sub_methods.push_back(procedure_declaration());
    else
      break;
  }

  // Array 
  ast_statement_ptr statements;
  for (;;) {
    auto body = statement();
    if (body == nullptr)
      break;
    statements.push_back((body));
  }
  return ast_block(top_, statements);
}



ast_statement_ptr  read_statement() {
 
  lexer_->match(token_type->read);

  auto id = lexer_->current_token.get_lexeme();
  lexer_->match(token_type->identifier);

  auto ident_info = top_->resolve(id);

  if (ident_info == nullptr) {
    printf("Undeclared variable", %s);
  } else if (!ident_info->is(ident_type->variable)) {
    printf("Cant call no-variable", %s) ;
  } else {
    return ast_read_statement(id);
  }
}


ast_statement_ptr  call_statement() {
 
  lexer_->match(token_type->call);

  auto id = lexer_->current_token.get_lexeme();
  lexer_->match(token_type->identifier);

  auto ident_info = top_->resolve(id);
  if (ident_info == nullptr)
    printf("Undeclared procedure", %s) ;
  else if (!ident_info->is(ident_type->procedure)) {
    printf("Cant call no-procedure") ;
  }
  return ast_call_statement(id);
}


ast_statement_ptr  begin_end_statement() {
 
  // Array  
  ast_statement_ptr statements;

  lexer_->match(token_type->begin);

  for (;;) {
    statements.push_back(statement());
    if (lexer_->current_token.is(token_type->semicolon)) {
      lexer_->match(token_type->semicolon);
      continue;
    } else {
      break;
    }
  }

  lexer_->match(token_type->end);

  return ast_begin_end_statement(statements);
}


ast_statement_ptr  statement() {
 
  switch (lexer_->current_token.get_type()) {
  case token_type->write:
    return write_statement();
  case token_type_>if_cond:
    return if_statement();
  case token_type->while_cond:
    return while_statement();
  case token_type->begin:
    return begin_end_statement();
  case token_type->call:
    return call_statement();
  case token_type->read:
    return read_statement();
  default:
    return assign_statement();
  }
}


ast_statement_ptr  while_statement() {
  
  lexer_->match(token_type->while_cond);

  auto cond = condition();

  lexer_->match(token_type->while_do);

  auto body = statement();

  return ast_while_statement(cond, body);
}


ast_statement_ptr  if_statement() {
 
  lexer_->match(token_type->if_cond);

  auto cond = condition();
  lexer_->match(token_type->if_then);

  auto body = statement();

  return ast_if_statement(cond, body);
}


ast_statement_ptr write_statement() {
  
  lexer_->match(token_type->write);

  return ast_write_statement(expression());
}


ast_statement_ptr assign_statement() {
  
  auto token = lexer_->current_token;
  if (token.is(token_type->identifier)) {

    auto id = lexer_->current_token.get_lexeme();

    lexer_->match(token_type->identifier);

    auto ident_info = top_->resolve(id);

    if (ident_info == nullptr)
       printf("Undeclared identifier", "%s");

    auto var = ast_variable(ident_info);

    lexer_->match(token_type->assign);

    return ast_assign_statement(var, expression());
  } else
    return nullptr;
}


ast_expression_ptr condition() {

  if (lexer_->current_token.is(token_type->odd)) {
    lexer_->match(token_type->odd);
    return ast_unary_expression(expression(), ast_expression->odd);
  } else {
    auto lhs = expression();

    auto cmp_operators = {token_type->equal,      token_type->not_equal,
                          token_type->less_equal, token_type->less,
                          token_type->greater,    token_type->greater_equal};

    if (!lexer_->current_token.is_one_of(cmp_operators)) {
       printf("Expected a compare opterator.", "%s") ;
    }
    auto token = lexer_->current_token;
    lexer_->current_token = lexer_->get_next_token();

    ast_expression->expression_type ret_tok;

    return ast_binary_expression(
        lhs, expression(), token.get_type());
  }
}


ast_expression_ptr  expression() {
 
  auto lhs = term();

  while (lexer_->current_token.is_one_of(token_type->plus, token_type->minus)) {
    if (lexer_->current_token.is(token_type->plus)) {
      lexer_->match(token_type->plus);
      lhs = ast_binary_expression(lhs, term(), ast_expression->add);
    } else {
      lexer_->match(token_type->minus);
      lhs = ast_binary_expression(lhs, term(), ast_expression->substract);
    }
  }
  return lhs;
}


ast_expression_ptr term() {
  
  auto lhs = factor();

  while (lexer_->current_token.is_one_of(token_type->mul, token_type->div)) {
    if (lexer_->current_token.is(token_type->mul)) {
      lexer_->match(token_type->mul);
      lhs = ast_binary_expression(lhs, factor(), ast_expression->multiply);
    } else if (lexer_->current_token.is(token_type->div)) {
      lexer_->match(token_type->div);
      lhs = ast_binary_expression(lhs, factor(), ast_expression->divide);
    }
  }
  return lhs;
}


ast_expression_ptr factor() {

  auto token = lexer_->current_token;
  if (token.is(token_type->identifier)) {

    auto id = lexer_->current_token.get_lexeme();

    lexer_->match(token_type->identifier);

    auto ident_info = top_->resolve(id);

    if (ident_info == nullptr)
       printf("Undeclared identifier", "%s") ;

    return ast_variable(ident_info);

  } else if (token.is(token_type->plus)) {
    lexer_->match(token_type->plus);
    return ast_unary_expression(factor(), ast_expression->plus);
  } else if (token.is(token_type->minus)) {
    lexer_->match(token_type->minus);
    return ast_unary_expression(factor(), ast_expression->minus);
  } else if (token.is(token_type->integer)) {
    lexer_->match(token_type->integer);
    return ast_number_expression(token.get_integer());
  } else if (token.is(token_type->lparen)) {
    lexer_->match(token_type->lparen);
    auto result = expression();
    lexer_->match(token_type->rparen);
    return result;
  } else {
       printf("Expect an identifier, a number or an expression...", "%s");
  }
}



// test
typedef struct odd {
   int operator()(const T &arg) { return (arg & 1) != 0; }
};


typedef struct unary_plus {
  int operator()(const T &arg) { return arg; }
};



class expression_visitor {
public:
  int visit(ast::expression &expr) {

    // to do poprawy
    if (expr.type == ast::expression::variable) {
      return static_cast<pl0::scope::variable_symbol *>(
                 static_cast<ast::variable &>(expr).get_target())
          ->get_value();
    } else if (expr.type == ast::expression::number) {
      return static_cast<ast::number_expression &>(expr).get_number();
    } else if (typeid(ast::unary_expression) == typeid(expr)) {
      int v1 = visit(static_cast<ast::unary_expression &>(expr).get_expr());
      return functors1[expr.type](v1);
    } else {
      int v1 = visit(static_cast<ast::binary_expression &>(expr).get_left());
      int v2 = visit(static_cast<ast::binary_expression &>(expr).get_right());

      return functors2[expr.type](v1, v2);
    }
  }

private:
  int visit_variable(ast::variable &var) {
    auto sym = var.get_target();
    if (sym->is(pl0::scope::ident_type::variable)) {

      return static_cast<pl0::scope::variable_symbol &>(*sym).get_value();
    }
  }
  std::unordered_map<ast::expression::expression_type,
                     std::function<int(int, int)>>
      functors2{
          {ast::expression::add, std::plus<>()},
          {ast::expression::substract, std::minus<>()},
          {ast::expression::divide, std::divides<>()},
          {ast::expression::multiply, std::multiplies<>()},
          {ast::expression::equal, std::equal_to<>()},
          {ast::expression::not_equal, std::not_equal_to<>()},
          {ast::expression::less_equal, std::less_equal<>()},
          {ast::expression::less, std::less<>()},
          {ast::expression::greater_equal, std::greater_equal<>()},
          {ast::expression::greater, std::greater<>()},
      };

  std::unordered_map<ast::expression::expression_type, std::function<int(int)>>
      functors1{
          {ast::expression::minus, std::negate<>()},
          {ast::expression::plus, ::unary_plus<>()},
          {ast::expression::odd, ::odd<>()},
      };
};




class compiler {
public:
  void generate(ast::block &program) {
    auto scope = program.get_scope();
    top_scope_ = scope;
    for (auto &n : program.get_body())
      visit_(*n);
    // visit_(program.get_body());
  }

private:
  expression_visitor e;
  std::shared_ptr<scope::scope> top_scope_ = nullptr;

  std::optional<int> visit_(ast::statement &stat) {
    switch (stat.type) {
    case ast::statement::assign: {
      auto &lhs = static_cast<ast::assign_statement &>(stat).get_left();
      auto &rhs = static_cast<ast::assign_statement &>(stat).get_right();

      auto lhs_ident_info = static_cast<ast::variable &>(lhs).get_target();

      auto ident_info = top_scope_->resolve(lhs_ident_info->get_name());
      if (ident_info == nullptr)
        throw utils::error{"Undeclared"};

      static_cast<scope::variable_symbol *>(ident_info)
          ->set_value(e.visit(rhs));

      break;
    }
    case ast::statement::write: {
      auto &expre = static_cast<ast::write_statement &>(stat).get_expr();
      std::cout << e.visit(expre) << "\n";
      break;
    }
    case ast::statement::if_cond: {
      auto &cond = static_cast<ast::if_statement &>(stat).get_condition();
      auto eval_cond = e.visit(cond);

      auto &body = static_cast<ast::if_statement &>(stat).get_body();

      if (eval_cond) {
        visit_(body);
        eval_cond = e.visit(cond);
      }
      break;
    }
    case ast::statement::while_cond: {
      auto &cond = static_cast<ast::while_statement &>(stat).get_condition();
      auto eval_cond = e.visit(cond);

      auto &body = static_cast<ast::while_statement &>(stat).get_body();

      while (eval_cond) {
        visit_(body);
        eval_cond = e.visit(cond);
      }
      break;
    }
    case ast::statement::begin_end: {
      for (auto &n :
           static_cast<ast::begin_end_statement &>(stat).get_statements()) {
        visit_(*n);
      }
      break;
    }
    case ast::statement::call: {
      std::cout << "Sorry call function not woring yet. To fix today\n";
      // generate(static_cast<ast::block&>(stat));
      break;
    }
    case ast::statement::read: {
      auto var_name = static_cast<ast::read_statement &>(stat).get_name();

      auto ident_info = top_scope_->resolve(var_name);

      int value;
      std::cin >> value;

      static_cast<scope::variable_symbol *>(ident_info)->set_value(value);

      break;
    }
    }
  }
};



int main() {
  std::string s;
  auto file = std::ifstream("Program.pl0");
  s.assign(std::istreambuf_iterator<char>{file},
           std::istreambuf_iterator<char>{});
  try {
    lexer::lexer lex{s};

    parser p{std::make_unique<lexer::lexer>(lex)};
    expression_visitor e;
    // std::cout << e.visit(*p.condition()) << "\n";
    compiler c;
    c.generate(*p.block());
  } catch (const utils::error &e) {
    std::cout << "oops: " << e.func << "\n";
  }
}


