#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <pkb.h>
#include "entities/statement.h"
#include <design_extractor.h>

TEST_CASE("PkbExtractEntities_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  SECTION("Test procedures") {
    std::set<std::string> correct_procedures =
        {"main", "readPoint", "printResults", "computeCentroid"};
    std::vector<Procedure *> proc_vec = pkb.get_all_procedures();

    REQUIRE(pkb.get_num_procedures() == correct_procedures.size());
    for (auto proc: proc_vec) {
      REQUIRE(correct_procedures.count(proc->get_name()) == 1);
    }
  }

  SECTION("Test statements") {
    int correct_num_statements = 23;
    REQUIRE(pkb.get_num_statements() == correct_num_statements);
  }

  SECTION("Test procedures") {
    std::set<std::string> correct_variables =
        {"flag", "x", "y", "cenX", "cenY", "normSq", "count"};
    std::vector<Variable *> var_vec = pkb.get_all_variables();

    REQUIRE(var_vec.size() == correct_variables.size());
    for (auto var: var_vec) {
      REQUIRE(correct_variables.count(var->get_name()) == 1);
    }
  }

  SECTION("Test constants") {
    std::set<std::string> correct_constants = {"0", "1"};
    std::vector<Constant *> const_vec = pkb.get_all_constants();

    REQUIRE(const_vec.size() == correct_constants.size());
    for (auto constant: const_vec) {
      REQUIRE(correct_constants.count(constant->get_value()) == 1);
    }
  }
}

TEST_CASE("PkbInitialisation_UndefinedProcCalled_ThrowsException") {
  ProgramNode *p = BuildProgAst(kUndefinedProc);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);

  REQUIRE_THROWS_AS(design_extractor.ExtractDesigns(), PKBException);
}

TEST_CASE("PkbInitialisation_CyclicProcCalls_ThrowsException") {
  ProgramNode *p = BuildProgAst(kCyclicProcs);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);

  REQUIRE_THROWS_AS(design_extractor.ExtractDesigns(), PKBException);
}
