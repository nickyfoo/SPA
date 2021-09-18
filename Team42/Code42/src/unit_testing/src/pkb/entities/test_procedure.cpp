#include <vector>
#include "pkb.h"
#include "catch.hpp"

TEST_CASE("GetName_ValidNames_ReturnsName", "[procedure][entity]") {
  SECTION("Test names with lower case letters") {
    std::string proc_name = "computecentroid";
    Procedure p(proc_name);
    REQUIRE(p.get_name() == proc_name);
  }

  SECTION("Test names with upper case letters") {
    std::string proc_name = "COMPUTECENTROID";
    Procedure p(proc_name);
    REQUIRE(p.get_name() == proc_name);
  }

  SECTION("Test names with letters and numbers") {
    std::string proc_name = "c12C45v7";
    Procedure p(proc_name);
    REQUIRE(p.get_name() == proc_name);
  }
}

TEST_CASE("AddGetUses_DifferentSetSizes_ReturnsVariableSet", "[procedure][entity]") {
  std::string proc_name = "ComputeCentroid";
  Procedure p(proc_name);

  SECTION("1 Uses Variable") {
    std::string var_name = "a";
    p.AddUses(var_name);
    auto uses_set = p.get_uses();

    REQUIRE(uses_set->size() == 1);
    REQUIRE(uses_set->count(var_name) == 1);
    REQUIRE(uses_set->count("A") == 0);
    REQUIRE(uses_set->count("b") == 0);
  }

  SECTION("10 Uses Variables") {
    std::set<std::string> uses_variables {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};

    for (auto &var_name : uses_variables) p.AddUses(var_name);
    auto uses_set = p.get_uses();

    REQUIRE(uses_set->size() == 10);
    for (auto &var_name : uses_variables) {
      REQUIRE(uses_set->count(var_name) == 1);
    }
    // Invalid variables that procedure does not use
    REQUIRE(uses_set->count("k") == 0);
    REQUIRE(uses_set->count("z") == 0);
    // Invalid case sensitive variables
    REQUIRE(uses_set->count("A") == 0);
    REQUIRE(uses_set->count("B") == 0);
  }
}

TEST_CASE("AddGetModifies_DifferentSetSizes_ReturnsVariableSet", "[procedure][entity]") {
  std::string proc_name = "ComputeCentroid";
  Procedure p(proc_name);

  SECTION("1 Modifies Variable") {
    std::string var_name = "a";
    p.AddModifies(var_name);
    auto modifies_set = p.get_modifies();

    REQUIRE(modifies_set->size() == 1);
    REQUIRE(modifies_set->count(var_name) == 1);
    REQUIRE(modifies_set->count("A") == 0);
    REQUIRE(modifies_set->count("b") == 0);
  }

  SECTION("10 Modifies Variables") {
    std::set<std::string> modifies_variables {"a", "b", "c", "d", "e", "f", "g", "h", "i", "j"};

    for (auto &var_name : modifies_variables) p.AddModifies(var_name);
    auto modifies_set = p.get_modifies();

    REQUIRE(modifies_set->size() == 10);
    for (auto &var_name : modifies_variables) {
      REQUIRE(modifies_set->count(var_name) == 1);
    }
    // Invalid variables that procedure does not modify
    REQUIRE(modifies_set->count("k") == 0);
    REQUIRE(modifies_set->count("z") == 0);
    // Invalid case sensitive variables
    REQUIRE(modifies_set->count("A") == 0);
    REQUIRE(modifies_set->count("B") == 0);
  }
}

TEST_CASE("AddGetCalls_DifferentSetSizes_ReturnsProceduresSet", "[procedure][entity]") {
  std::string proc_name = "ComputeCentroid";
  Procedure p(proc_name);

  SECTION("1 Calls Procedure") {
    std::string calls_proc = "apple";
    p.AddCalls(calls_proc);
    auto calls_set = p.get_calls();

    REQUIRE(calls_set->size() == 1);
    REQUIRE(calls_set->count(calls_proc) == 1);
    REQUIRE(calls_set->count("Apple") == 0);
    REQUIRE(calls_set->count("banana") == 0);
  }

  SECTION("10 Calls Procedures") {
    std::set<std::string> calls_procs {"apple", "banana", "carrot", "donut", "either"};

    for (auto &calls_proc : calls_procs) p.AddCalls(calls_proc);
    auto calls_set = p.get_calls();

    REQUIRE(calls_set->size() == calls_procs.size());
    for (auto &calls_proc : calls_procs) {
      REQUIRE(calls_set->count(calls_proc) == 1);
    }
    // Invalid variables that procedure does not modify
    REQUIRE(calls_set->count("k") == 0);
    REQUIRE(calls_set->count("z") == 0);
    // Invalid case sensitive variables
    REQUIRE(calls_set->count("A") == 0);
    REQUIRE(calls_set->count("B") == 0);
  }
}

TEST_CASE("AddGetCallsStar_DifferentSetSizes_ReturnsProceduresSet", "[procedure][entity]") {
  std::string proc_name = "ComputeCentroid";
  Procedure p(proc_name);

  SECTION("1 Calls Star Procedure") {
    std::string calls_star_proc = "apple";
    p.AddCallsStar(calls_star_proc);
    auto calls_star_set = p.get_calls_star();

    REQUIRE(calls_star_set->size() == 1);
    REQUIRE(calls_star_set->count(calls_star_proc) == 1);
    REQUIRE(calls_star_set->count("Apple") == 0);
    REQUIRE(calls_star_set->count("banana") == 0);
  }

  SECTION("10 Calls Star Procedures") {
    std::set<std::string> calls_star_procs {"apple", "banana", "carrot", "donut", "either"};

    for (auto &calls_star_proc : calls_star_procs) p.AddCallsStar(calls_star_proc);
    auto calls_star_set = p.get_calls_star();

    REQUIRE(calls_star_set->size() == calls_star_procs.size());
    for (auto &calls_star_proc : calls_star_procs) {
      REQUIRE(calls_star_set->count(calls_star_proc) == 1);
    }
    // Invalid variables that procedure does not modify
    REQUIRE(calls_star_set->count("k") == 0);
    REQUIRE(calls_star_set->count("z") == 0);
    // Invalid case sensitive variables
    REQUIRE(calls_star_set->count("A") == 0);
    REQUIRE(calls_star_set->count("B") == 0);
  }
}

TEST_CASE("AddGetCallers_DifferentSetSizes_ReturnsProceduresSet", "[procedure][entity]") {
  std::string proc_name = "ComputeCentroid";
  Procedure p(proc_name);

  SECTION("1 Caller Procedure") {
    std::string caller_proc = "apple";
    p.AddCallers(caller_proc);
    auto callers_set = p.get_callers();

    REQUIRE(callers_set->size() == 1);
    REQUIRE(callers_set->count(caller_proc) == 1);
    REQUIRE(callers_set->count("Apple") == 0);
    REQUIRE(callers_set->count("banana") == 0);
  }

  SECTION("10 Caller Procedures") {
    std::set<std::string> caller_procs {"apple", "banana", "carrot", "donut", "either"};

    for (auto &caller_proc : caller_procs) p.AddCallers(caller_proc);
    auto callers_set = p.get_callers();

    REQUIRE(callers_set->size() == caller_procs.size());
    for (auto &caller_proc : caller_procs) {
      REQUIRE(callers_set->count(caller_proc) == 1);
    }
    // Invalid variables that procedure does not modify
    REQUIRE(callers_set->count("k") == 0);
    REQUIRE(callers_set->count("z") == 0);
    // Invalid case sensitive variables
    REQUIRE(callers_set->count("A") == 0);
    REQUIRE(callers_set->count("B") == 0);
  }
}

TEST_CASE("AddGetCallersStar_DifferentSetSizes_ReturnsProceduresSet", "[procedure][entity]") {
  std::string proc_name = "ComputeCentroid";
  Procedure p(proc_name);

  SECTION("1 Caller Star Procedure") {
    std::string caller_star_proc = "apple";
    p.AddCallersStar(caller_star_proc);
    auto callers_star_set = p.get_callers_star();

    REQUIRE(callers_star_set->size() == 1);
    REQUIRE(callers_star_set->count(caller_star_proc) == 1);
    REQUIRE(callers_star_set->count("Apple") == 0);
    REQUIRE(callers_star_set->count("banana") == 0);
  }

  SECTION("10 Caller Star Procedures") {
    std::set<std::string> caller_star_procs {"apple", "banana", "carrot", "donut", "either"};

    for (auto &caller_star_proc : caller_star_procs) p.AddCallersStar(caller_star_proc);
    auto callers_star_set = p.get_callers_star();

    REQUIRE(callers_star_set->size() == caller_star_procs.size());
    for (auto &caller_star_proc : caller_star_procs) {
      REQUIRE(callers_star_set->count(caller_star_proc) == 1);
    }
    // Invalid variables that procedure does not modify
    REQUIRE(callers_star_set->count("k") == 0);
    REQUIRE(callers_star_set->count("z") == 0);
    // Invalid case sensitive variables
    REQUIRE(callers_star_set->count("A") == 0);
    REQUIRE(callers_star_set->count("B") == 0);
  }
}
