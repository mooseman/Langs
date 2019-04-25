#include "catch.hpp"
#include "lexer.hpp"
#include "token.hpp"

#include <string>
#include <string_view>

using namespace pl0::lexer;

TEST_CASE("State of current_token variable in Lexer", "[lexer]") {
  lexer lex{std::string{"var IDENT := 12; "}};

  SECTION("can be empty", "[lexer]") {
    auto tok = lex.current_token;

    REQUIRE(tok.get_lexeme() == std::string_view{""});

    // empty enum class is undefined behaviour
    // so current_token.get_type() = UB
    // fix it some day
  }

  SECTION(
      "After get_next_token() current_token gets next token in the stream") {
    lex.get_next_token();

    REQUIRE(lex.current_token == token{token_type::var, "var"});
  }

  SECTION("After match() current_token gets next token in the stream") {

    lexer lex{std::string{"2+2"}};
    lex.get_next_token();

    lex.match(token_type::integer);

    REQUIRE(lex.current_token == token{token_type::plus, "+"});

    SECTION("Or throw exception") {
      REQUIRE_THROWS_AS(lex.match(token_type::odd), pl0::utils::error);
    }
  }
}

TEST_CASE("Next token from the stream can be obtained by get_next_token()", "[lexer]") {
  lexer lex{std::string{"var IDENT := 12; "}};

  REQUIRE(lex.get_next_token() == token{token_type::var, "var"});
  REQUIRE(lex.get_next_token() == token{token_type::identifier, "ident"});
}

TEST_CASE("Token from the stream can be matched/eaten", "[lexer]") {
  lexer lex{std::string{"3*2"}};
  lex.get_next_token();

  lex.match(token_type::integer);
  REQUIRE(lex.current_token == token{token_type::mul, "*"});

  lex.match(token_type::mul);
  REQUIRE(lex.current_token == token{token_type::integer, "2"});

  SECTION("Match throws exception when failed") {
	  REQUIRE_THROWS_AS(lex.match(token_type::mul), pl0::utils::error);
	  REQUIRE_NOTHROW(lex.match(token_type::integer));
  }

}
