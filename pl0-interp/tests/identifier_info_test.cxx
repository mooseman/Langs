#include "catch.hpp"
#include "identifier_info.hpp"

using namespace pl0::identifier_info;

TEST_CASE("Variable symbol contains info like name and value ", "[identifier_info]") {
	variable_symbol v{"Var_name", 1234};

	REQUIRE(v.get_value() == 1234);

	v.set_value(1);
	REQUIRE(v.get_value() == 1);

	REQUIRE(v.get_name() == std::string{"Var_name"});
	REQUIRE(v.is(ident_type::variable));

}

TEST_CASE("Procedure symbol contains info like name and level", "[identifier_info]") {
	procedure_symbol p{"Procedure_name", 1};

	REQUIRE(p.get_name() == std::string{"Procedure_name"});
	REQUIRE(p.get_level() == 1);
	REQUIRE(p.is(ident_type::procedure) == true);

	REQUIRE_FALSE(p.is(ident_type::variable));
	REQUIRE_FALSE(p.is(ident_type::constant));
}