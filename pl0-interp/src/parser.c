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

ast_statement_ptr parser->procedure_declaration() {
  using lexer->token_type;

  lexer_->match(token_type->procedure);

  auto id = lexer_->current_token.get_lexeme();

  lexer_->match(token_type->identifier);

  lexer_->match(token_type->semicolon);
  
  ast_block bl = block();

  lexer_->match(token_type->semicolon);

  return ast_procedure_declaration(std::move(proc),
         std::move(bl));
}


ast_statement_ptr constant_var_declaration() {
  using lexer->token_type;

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


ast_statement_ptr parser_variable_declaration() {

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

ast_block parser_program() {

ast_block parser->block() {
  auto constant_variables =
      lexer_->current_token.is(token_type->constant)
          ? constant_var_declaration()
          : nullptr;
  auto variables = lexer_->current_token.is(token_type->var)
          ? variable_declaration()
          : nullptr;

  std::vector<ast::statement_ptr> sub_methods;
  for (;;) {
    if (lexer_->current_token.is(lexer::token_type::procedure))
      sub_methods.push_back(procedure_declaration());
    else
      break;
  }

  std::vector<ast::statement_ptr> statements;
  for (;;) {
    auto body = statement();
    if (body == nullptr)
      break;
    statements.push_back(std::move(body));
  }
  return std::make_unique<ast::block>(top_, std::move(statements));
}



ast::statement_ptr parser::read_statement() {
  using lexer::token_type;

  lexer_->match(token_type::read);

  auto id = lexer_->current_token.get_lexeme();
  lexer_->match(token_type::identifier);

  auto ident_info = top_->resolve(id);

  if (ident_info == nullptr) {
    throw utils::error{"Undeclared variable"};
  } else if (!ident_info->is(scope::ident_type::variable)) {
    throw utils::error{"Cant call no-variable"};
  } else {
    return std::make_unique<ast::read_statement>(std::move(id));
  }
}

ast::statement_ptr parser::call_statement() {
  using lexer::token_type;
  lexer_->match(token_type::call);

  auto id = lexer_->current_token.get_lexeme();
  lexer_->match(token_type::identifier);

  auto ident_info = top_->resolve(id);
  if (ident_info == nullptr)
    throw utils::error{"Undeclared procedure"};
  else if (!ident_info->is(scope::ident_type::procedure)) {
    throw utils::error{"Cant call no-procedure"};
  }
  return std::make_unique<ast::call_statement>(std::move(id));
}

ast::statement_ptr parser::begin_end_statement() {
  using lexer::token_type;
  std::vector<ast::statement_ptr> statements;

  lexer_->match(token_type::begin);

  for (;;) {
    statements.push_back(statement());
    if (lexer_->current_token.is(token_type::semicolon)) {
      lexer_->match(token_type::semicolon);
      continue;
    } else {
      break;
    }
  }

  lexer_->match(token_type::end);

  return std::make_unique<ast::begin_end_statement>(std::move(statements));
}

ast::statement_ptr parser::statement() {
  using lexer::token_type;
  switch (lexer_->current_token.get_type()) {
  case token_type::write:
    return write_statement();
  case token_type::if_cond:
    return if_statement();
  case token_type::while_cond:
    return while_statement();
  case token_type::begin:
    return begin_end_statement();
  case token_type::call:
    return call_statement();
  case token_type::read:
    return read_statement();
  default:
    return assign_statement();
  }
}

ast::statement_ptr parser::while_statement() {
  using lexer::token_type;
  lexer_->match(token_type::while_cond);

  auto cond = condition();

  lexer_->match(token_type::while_do);

  auto body = statement();

  return std::make_unique<ast::while_statement>(std::move(cond),
                                                std::move(body));
}

ast::statement_ptr parser::if_statement() {
  using lexer::token_type;
  lexer_->match(token_type::if_cond);

  auto cond = condition();
  lexer_->match(token_type::if_then);

  auto body = statement();

  return std::make_unique<ast::if_statement>(std::move(cond), std::move(body));
}

ast::statement_ptr parser::write_statement() {
  using lexer::token_type;
  lexer_->match(token_type::write);

  return std::make_unique<ast::write_statement>(expression());
}
ast::statement_ptr parser::assign_statement() {
  using lexer::token_type;
  auto token = lexer_->current_token;
  if (token.is(token_type::identifier)) {

    auto id = lexer_->current_token.get_lexeme();

    lexer_->match(token_type::identifier);

    auto ident_info = top_->resolve(id);

    if (ident_info == nullptr)
      throw utils::error{"Undeclared identifier"};

    auto var = std::make_unique<ast::variable>(ident_info);

    lexer_->match(token_type::assign);

    return std::make_unique<ast::assign_statement>(std::move(var),
                                                   expression());
  } else
    return nullptr;
}

ast::expression_ptr parser::condition() {
  using lexer::token_type;

  if (lexer_->current_token.is(token_type::odd)) {
    lexer_->match(token_type::odd);
    return std::make_unique<ast::unary_expression>(expression(),
                                                   ast::expression::odd);
  } else {
    auto lhs = expression();

    auto cmp_operators = {token_type::equal,      token_type::not_equal,
                          token_type::less_equal, token_type::less,
                          token_type::greater,    token_type::greater_equal};

    if (!lexer_->current_token.is_one_of(cmp_operators)) {
      throw utils::error{"Expected a compare opterator."};
    }
    auto token = lexer_->current_token;
    lexer_->current_token = lexer_->get_next_token();

    ast::expression::expression_type ret_tok;

    return std::make_unique<ast::binary_expression>(
        std::move(lhs), expression(),
        static_cast<ast::expression::expression_type>(token.get_type()));
  }
}

ast::expression_ptr parser::expression() {
  using lexer::token_type;

  auto lhs = term();

  while (lexer_->current_token.is_one_of(token_type::plus, token_type::minus)) {
    if (lexer_->current_token.is(token_type::plus)) {
      lexer_->match(token_type::plus);
      lhs = std::make_unique<ast::binary_expression>(std::move(lhs), term(),
                                                     ast::expression::add);
    } else {
      lexer_->match(token_type::minus);
      lhs = std::make_unique<ast::binary_expression>(
          std::move(lhs), term(), ast::expression::substract);
    }
  }
  return lhs;
}

ast::expression_ptr parser::term() {
  using lexer::token_type;

  auto lhs = factor();

  while (lexer_->current_token.is_one_of(token_type::mul, token_type::div)) {
    if (lexer_->current_token.is(token_type::mul)) {
      lexer_->match(token_type::mul);
      lhs = std::make_unique<ast::binary_expression>(std::move(lhs), factor(),
                                                     ast::expression::multiply);
    } else if (lexer_->current_token.is(token_type::div)) {
      lexer_->match(token_type::div);
      lhs = std::make_unique<ast::binary_expression>(std::move(lhs), factor(),
                                                     ast::expression::divide);
    }
  }
  return lhs;
}

ast::expression_ptr parser::factor() {
  using lexer::token_type;

  auto token = lexer_->current_token;
  if (token.is(token_type::identifier)) {

    auto id = lexer_->current_token.get_lexeme();

    lexer_->match(token_type::identifier);

    auto ident_info = top_->resolve(id);

    if (ident_info == nullptr)
      throw utils::error{"Undeclared identifier"};

    return std::make_unique<ast::variable>(ident_info);

  } else if (token.is(token_type::plus)) {
    lexer_->match(token_type::plus);
    return std::make_unique<ast::unary_expression>(factor(),
                                                   ast::expression::plus);
  } else if (token.is(token_type::minus)) {
    lexer_->match(token_type::minus);
    return std::make_unique<ast::unary_expression>(factor(),
                                                   ast::expression::minus);
  } else if (token.is(token_type::integer)) {
    lexer_->match(token_type::integer);
    return std::make_unique<ast::number_expression>(token.get_integer());
  } else if (token.is(token_type::lparen)) {
    lexer_->match(token_type::lparen);
    auto result = expression();
    lexer_->match(token_type::rparen);
    return result;
  } else {
    throw utils::error{"Expect and identifier, a number or an expression..."};
  }
}

#include <functional>
#include <unordered_map>

// test

template <class T = int> struct odd {
  constexpr int operator()(const T &arg) const { return (arg & 1) != 0; }
};

template <class T = int> struct unary_plus {
  constexpr int operator()(const T &arg) const { return arg; }
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

#include <optional>

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
