#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <parse.h>
#include <pkb.h>
#include "entities/statement.h"
#include <design_extractor.h>

TEST_CASE("PkbUses_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<int, std::vector<std::string>> stmt_uses_ans = {
      {1, {}}, {2, {"x", "y", "count", "cenX", "cenY"}}, {3, {"flag", "cenX", "cenY", "normSq"}},
      {4, {}}, {5, {}}, {6, {"flag"}}, {7, {"cenX"}}, {8, {"cenY"}}, {9, {"normSq"}}, {10, {}},
      {11, {}}, {12, {}}, {13, {}}, {14, {"x", "y", "count", "cenX", "cenY"}}, {15, {"count"}},
      {16, {"cenX", "x"}}, {17, {"cenY", "y"}}, {18, {}}, {19, {"count", "cenX", "cenY"}},
      {20, {}}, {21, {"cenX", "count"}}, {22, {"cenY", "count"}}, {23, {"cenX", "cenY"}},
      };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_uses = stmt->get_uses();
    std::vector<std::string> uses = stmt_uses_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(stmt_uses->find(use) != stmt_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_uses_ans = {
      {"main", {"x", "y", "count", "cenX", "cenY", "flag", "normSq"}},
      {"readPoint", {}},
      {"printResults", {"flag", "cenX", "cenY", "normSq"}},
      {"computeCentroid", {"x", "y", "count", "cenX", "cenY"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_uses = proc->get_uses();
    std::vector<std::string> uses = proc_uses_ans[proc->get_name()];
    REQUIRE(proc_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(proc_uses->find(use) != proc_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_used_by_procs = {
      {"flag", {"printResults", "main"}},
      {"cenX", {"printResults", "computeCentroid", "main"}},
      {"cenY", {"printResults", "computeCentroid", "main"}},
      {"normSq", {"printResults", "main"}},
      {"x", {"computeCentroid", "main"}},
      {"y", {"computeCentroid", "main"}},
      {"count", {"computeCentroid", "main"}},
      };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_using = var->get_procs_using();
    std::vector<std::string> procs = var_used_by_procs[var->get_name()];
    REQUIRE(procs_using->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_using->find(proc) != procs_using->end());
    }
  }

  std::map<std::string, std::vector<int>> var_used_by_stmts = {
      {"flag", {3, 6}},
      {"cenX", {2, 3, 7, 14, 16, 19, 21, 23}},
      {"cenY", {2, 3, 8, 14, 17, 19, 22, 23}},
      {"normSq", {3, 9}},
      {"x", {2, 14, 16}},
      {"y", {2, 14, 17}},
      {"count", {2, 14, 15, 19, 21, 22}},
      };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_using = var->get_stmts_using();
    std::vector<int> stmts = var_used_by_stmts[var->get_name()];
    REQUIRE(stmts_using->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_using->find(stmt) != stmts_using->end());
    }
  }
}

TEST_CASE("PkbModifies_SampleProgram_Correct") {
  ProgramNode *p = BuildProgAst(kSampleSource);
  PKB pkb;
  DesignExtractor design_extractor(&pkb, p);
  design_extractor.ExtractDesigns();

  std::map<int, std::vector<std::string>> stmt_modifies_ans = {
      {1, {{"flag"}}}, {2, {"count", "cenX", "cenY", "x", "y", "flag", "normSq"}}, {3, {}},
      {4, {"x"}}, {5, {"y"}}, {6, {}}, {7, {}}, {8, {}}, {9, {}}, {10, {"count"}}, {11, {"cenX"}},
      {12, {"cenY"}}, {13, {"x", "y"}}, {14, {"count", "cenX", "cenY", "x", "y"}}, {15, {"count"}},
      {16, {"cenX"}}, {17, {"cenY"}}, {18, {"x", "y"}}, {19, {"flag", "cenX", "cenY"}},
      {20, {"flag"}}, {21, {"cenX"}}, {22, {"cenY"}}, {23, {"normSq"}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_modifies = stmt->get_modifies();
    std::vector<std::string> modifies = stmt_modifies_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(stmt_modifies->find(modify) != stmt_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_modifies_ans = {
      {"main", {"count", "cenX", "cenY", "x", "y", "flag", "normSq"}},
      {"readPoint", {"x", "y"}},
      {"printResults", {}},
      {"computeCentroid", {"count", "cenX", "cenY", "x", "y", "flag", "normSq"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_modifies = proc->get_modifies();
    std::vector<std::string> modifies = proc_modifies_ans[proc->get_name()];
    REQUIRE(proc_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(proc_modifies->find(modify) != proc_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_modified_by_procs = {
      {"flag", {"main", "computeCentroid"}},
      {"cenX", {"computeCentroid", "main"}},
      {"cenY", {"computeCentroid", "main"}},
      {"normSq", {"main", "computeCentroid"}},
      {"x", {"readPoint", "computeCentroid", "main"}},
      {"y", {"readPoint", "computeCentroid", "main"}},
      {"count", {"computeCentroid", "main"}},
      };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_modifying = var->get_procs_modifying();
    std::vector<std::string> procs = var_modified_by_procs[var->get_name()];
    REQUIRE(procs_modifying->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_modifying->find(proc) != procs_modifying->end());
    }
  }

  std::map<std::string, std::vector<int>> var_modified_by_stmts = {
      {"flag", {1, 2, 19, 20}},
      {"cenX", {2, 11, 14, 16, 19, 21}},
      {"cenY", {2, 12, 14, 17, 19, 22}},
      {"normSq", {2, 23}},
      {"x", {2, 4, 13, 14, 18}},
      {"y", {2, 5, 13, 14, 18}},
      {"count", {2, 10, 14, 15}}
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_modifying = var->get_stmts_modifying();
    std::vector<int> stmts = var_modified_by_stmts[var->get_name()];
    REQUIRE(stmts_modifying->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_modifying->find(stmt) != stmts_modifying->end());
    }
  }
}

TEST_CASE("PkbUses_ContainerStmt_Correct") {
  PKB pkb = InitialisePKB(kSimpleContainer);

  std::map<int, std::vector<std::string>> stmt_uses_ans = {
      {1, {"x", "y", "b", "c", "e", "f"}},
      {2, {"b", "c"}},
      {3, {"e", "f"}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_uses = stmt->get_uses();
    std::vector<std::string> uses = stmt_uses_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(stmt_uses->find(use) != stmt_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_uses_ans = {
      {"main", {"x", "y", "b", "c", "e", "f"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_uses = proc->get_uses();
    std::vector<std::string> uses = proc_uses_ans[proc->get_name()];
    REQUIRE(proc_uses->size() == uses.size());
    for (auto &use : uses) {
      REQUIRE(proc_uses->find(use) != proc_uses->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_used_by_procs = {
      {"a", {}}, {"b", {"main"}}, {"c", {"main"}}, {"d", {}}, {"e", {"main"}},
      {"f", {"main"}}, {"x", {"main"}}, {"y", {"main"}}
  };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_using = var->get_procs_using();
    std::vector<std::string> procs = var_used_by_procs[var->get_name()];
    REQUIRE(procs_using->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_using->find(proc) != procs_using->end());
    }
  }

  std::map<std::string, std::vector<int>> var_used_by_stmts = {
      {"a", {}}, {"b", {1, 2}}, {"c", {1, 2}}, {"d", {}}, {"e", {1, 3}},
      {"f", {1, 3}}, {"x", {1}}, {"y", {1}},
      };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_using = var->get_stmts_using();
    std::vector<int> stmts = var_used_by_stmts[var->get_name()];
    REQUIRE(stmts_using->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_using->find(stmt) != stmts_using->end());
    }
  }
}

TEST_CASE("PkbModifies_ContainerStmt_Correct") {
  PKB pkb = InitialisePKB(kSimpleContainer);

  std::map<int, std::vector<std::string>> stmt_modifies_ans = {
      {1, {"a", "d"}},
      {2, {"a"}},
      {3, {"d"}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<std::string> *stmt_modifies = stmt->get_modifies();
    std::vector<std::string> modifies = stmt_modifies_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(stmt_modifies->find(modify) != stmt_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> proc_modifies_ans = {
      {"main", {"a", "d"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_modifies = proc->get_modifies();
    std::vector<std::string> modifies = proc_modifies_ans[proc->get_name()];
    REQUIRE(proc_modifies->size() == modifies.size());
    for (auto &modify : modifies) {
      REQUIRE(proc_modifies->find(modify) != proc_modifies->end());
    }
  }

  std::map<std::string, std::vector<std::string>> var_modified_by_procs = {
      {"a", {"main"}}, {"b", {}}, {"c", {}}, {"d", {"main"}}, {"e", {}},
      {"f", {}}, {"x", {}}, {"y", {}},
      };
  for (auto var : pkb.get_all_variables()) {
    std::set<std::string> *procs_modifying = var->get_procs_modifying();
    std::vector<std::string> procs = var_modified_by_procs[var->get_name()];
    REQUIRE(procs_modifying->size() == procs.size());
    for (auto &proc : procs) {
      REQUIRE(procs_modifying->find(proc) != procs_modifying->end());
    }
  }

  std::map<std::string, std::vector<int>> var_modified_by_stmts = {
      {"a", {1, 2}}, {"b", {}}, {"c", {}}, {"d", {1, 3}}, {"e", {}},
      {"f", {}}, {"x", {}}, {"y", {}},
      };
  for (auto var : pkb.get_all_variables()) {
    std::set<int> *stmts_modifying = var->get_stmts_modifying();
    std::vector<int> stmts = var_modified_by_stmts[var->get_name()];
    REQUIRE(stmts_modifying->size() == stmts.size());
    for (auto &stmt : stmts) {
      REQUIRE(stmts_modifying->find(stmt) != stmts_modifying->end());
    }
  }
}
