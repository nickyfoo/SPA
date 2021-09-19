#include <vector>
#include "variable.h"
#include "catch.hpp"

TEST_CASE("GetName_ValidVariable_ReturnsName", "[variable][entity]") {
  std::string var_name = "x";
  Variable v(rand(), var_name);

  REQUIRE(v.get_name() == var_name);
  REQUIRE(v.get_name() != "X");
  REQUIRE(v.get_name() != "y");
}

TEST_CASE("AddGetStmtUsing_SingleUsingStmt_ReturnsIntSet", "[variable][entity]") {
  std::string var_name = "x";
  Variable v(rand(), var_name);
  int using_stmt = rand();
  v.AddStmtUsing(using_stmt);

  auto stmts_using_set = v.get_stmts_using();
  REQUIRE(stmts_using_set->size() == 1);
  REQUIRE(stmts_using_set->count(using_stmt) == 1);
}

TEST_CASE("AddGetStmtModifying_SingleModifyingStmt_ReturnsIntSet", "[variable][entity]") {
  std::string var_name = "x";
  Variable v(rand(), var_name);
  int modifying_stmt = rand();
  v.AddStmtModifying(modifying_stmt);

  auto stmts_modifying_set = v.get_stmts_modifying();
  REQUIRE(stmts_modifying_set->size() == 1);
  REQUIRE(stmts_modifying_set->count(modifying_stmt) == 1);
}

TEST_CASE("AddGetProcsUsing_SingleUsingProc_ReturnsStringSet", "[variable][entity]") {
  std::string var_name = "x";
  Variable v(rand(), var_name);
  std::string using_proc = "y";
  v.AddProcUsing(using_proc);

  auto procs_using_set = v.get_procs_using();
  REQUIRE(procs_using_set->size() == 1);
  REQUIRE(procs_using_set->count(using_proc) == 1);
}

TEST_CASE("AddGetProcModifying_SingleModifyingProc_ReturnsStringSet", "[variable][entity]") {
  std::string var_name = "x";
  Variable v(rand(), var_name);
  std::string modifying_proc = "y";
  v.AddProcModifying(modifying_proc);

  auto procs_modifying_set = v.get_procs_modifying();
  REQUIRE(procs_modifying_set->size() == 1);
  REQUIRE(procs_modifying_set->count(modifying_proc) == 1);
}
