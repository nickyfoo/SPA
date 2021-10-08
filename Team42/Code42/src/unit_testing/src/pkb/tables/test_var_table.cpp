#include <vector>
#include "var_table.h"
#include "catch.hpp"

TEST_CASE("GetNumVariables_DifferentSizeTables_ReturnsNumVariables", "[variable][table]") {
  SECTION("Table size: 1") {
    std::string var_name = "x";
    VarTable var_table;
    var_table.AddVariable(var_name);

    REQUIRE(var_table.get_num_variables() == 1);
  }

  SECTION("Table size: 5") {
    std::set<std::string> var_names{"apple", "banana", "carrot", "donut", "either"};
    VarTable var_table;
    for (const std::string &var_name: var_names) var_table.AddVariable(var_name);

    REQUIRE(var_table.get_num_variables() == var_names.size());
  }
}

TEST_CASE("AddGetAllVariables_DifferentSizeTables_ReturnsVectorVariables", "[variable][table]") {
  SECTION("Table size: 1") {
    std::string var_name = "x";
    VarTable var_table;
    var_table.AddVariable(var_name);
    std::vector<Variable *> var_vec = var_table.get_all_variables();

    REQUIRE(var_vec.size() == 1);
    REQUIRE(std::any_of(var_vec.begin(),
                        var_vec.end(),
                        [&](Variable *elem) { return elem->get_name() == var_name; }));
  }

  SECTION("Table size: 5") {
    std::set<std::string> var_names{"apple", "banana", "carrot", "donut", "either"};
    VarTable var_table;
    for (const std::string &var_name: var_names) var_table.AddVariable(var_name);
    std::vector<Variable *> var_vec = var_table.get_all_variables();

    REQUIRE(var_vec.size() == var_names.size());
    for (std::string var_name: var_names) {
      REQUIRE(std::any_of(var_vec.begin(),
                          var_vec.end(),
                          [&](Variable *elem) { return elem->get_name() == var_name; }));
    }
  }
}

TEST_CASE("GetVariable_DifferentSizeTables_ReturnsVariable", "[variable][table]") {
  SECTION("Table size: 1") {
    std::string var_name = "s";
    VarTable var_table;
    var_table.AddVariable(var_name);

    REQUIRE(var_table.get_variable(var_name)->get_name() == var_name);
  }

  SECTION("Table size: 5") {
    std::set<std::string> var_names{"apple", "banana", "carrot", "donut", "either"};
    VarTable var_table;
    for (const std::string &var_name: var_names) var_table.AddVariable(var_name);

    for (const std::string &var_name: var_names) {
      REQUIRE(var_table.get_variable(var_name)->get_name() == var_name);
    }
  }
}
