#include "catch.hpp"
#include "scope.hpp"

#include <memory>
#include <string_view>

using namespace pl0::scope;
using namespace pl0::identifier_info;

TEST_CASE("Create new Scope in program", "[scope]") {
  scope s{"Global Scope"};

  REQUIRE(s.get_scope_name() == std::string_view{"Global Scope"});
  REQUIRE(s.get_enclosing_scope() == nullptr);
  REQUIRE(s.get_level() == 0);

  SECTION("Enclosing scope") {
    scope s2{"Function Scope", std::make_shared<scope>("Inner Scope")};
    REQUIRE(s2.get_scope_name() == std::string_view{"Function Scope"});
    REQUIRE(s2.get_enclosing_scope()->get_scope_name() ==
                  std::string_view{"Inner Scope"});
  }
}

TEST_CASE("Define Identifier_info symbol in Scope", "[scope]") {
  scope s{"Global Scope"};

  auto var =
      std::make_unique<ident_info>(std::move(variable_symbol{"Var1", 12}));

  s.define(std::move(var));

  REQUIRE(s.resolve("Var1"));
  REQUIRE_FALSE(s.resolve("Var2"));
}