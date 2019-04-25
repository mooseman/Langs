#include "catch.hpp"
#include "token.hpp"

#include <initializer_list>
#include <string_view>

using namespace pl0::lexer;

TEST_CASE("Type nad Lexeme can be passed through constructor", "[token]") {
  token tok{token_type::mul, "*"};

  REQUIRE(tok.get_type() == token_type::mul);
  REQUIRE(tok.get_lexeme() == std::string_view{"*"});

  SECTION("Empty lexeme") {
    token tok{token_type::eof, ""};

    REQUIRE(tok.get_type() == token_type::eof);
    REQUIRE(tok.get_lexeme() == std::string_view{""});
  }
}

TEST_CASE("Token type and lexeme can be checked", "[token]") {

  token tok{token_type::id, "Name"};

  REQUIRE(tok.get_type() == token_type::id);
  REQUIRE(tok.get_lexeme() == std::string_view{"Name"});

  SECTION("Check token type") {
    auto result = tok.is(token_type::id);
    REQUIRE(result == true);
  }

  SECTION("Check one token_type of many") {
    auto result =
        tok.is_one_of(token_type::constant, token_type::comma, token_type::id);
    REQUIRE(result == true);
  }

  SECTION("Check one token_type of many with std::initializer_list") {
    auto ilist = {token_type::while_cond, token_type::greater_equal,
                  token_type::id, token_type::empty};
    auto result = tok.is_one_of(ilist);
    REQUIRE(result == true);

    ilist = {token_type::equal, token_type::less, token_type::greater};
    CHECK_FALSE(tok.is_one_of(ilist));
  }
}

TEST_CASE("Lexeme can be converted to another types", "[token]") {
  token tok{token_type::integer, "1234"};

  SECTION("Lexeme from std::string_view to integer") {
    REQUIRE(tok.get_integer() == 1234);

    token tok2{token_type::integer, "-12345"};
    REQUIRE(tok2.get_integer() == -12345);

    token tok3{token_type::integer, "0000"};
    REQUIRE(tok3.get_integer() == 0);

    token tok4{token_type::integer, "-0"};
    REQUIRE(tok3.get_integer() == 0);
  }
}

TEST_CASE("Token_type can be converted to std::string", "[token]") {
  token tok{token_type::procedure, "func"};

  REQUIRE(tok.get_name() == "procedure");
}

TEST_CASE("Token objects can be compared", "[token]") {
  token tok1{token_type::integer, "12"};
  token tok2{token_type::integer, "12"};

  CHECK(tok1 == tok2);
  CHECK_FALSE(tok1 != tok2);

  token tok3{token_type::integer, "1234"};
  
  CHECK(tok1 != tok3);
  CHECK_FALSE(tok1 == tok3);
}