#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <parse.h>
#include <pkb.h>
#include "entities/statement.h"

TEST_CASE("PkbCalls_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<std::string, std::vector<std::string>> calls_ans = {
      {"main", {"computeCentroid", "printResults"}},
      {"readPoint", {}},
      {"printResults", {}},
      {"computeCentroid", {"readPoint"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *proc_calls = proc->get_calls();
    std::vector<std::string> calls = calls_ans[proc->get_name()];
    REQUIRE(proc_calls->size() == calls.size());
    for (auto &call : calls) {
      REQUIRE(proc_calls->find(call) != proc_calls->end());
    }
  }

  std::map<std::string, std::vector<std::string>> callers_ans = {
      {"main", {}},
      {"readPoint", {"computeCentroid"}},
      {"printResults", {"main"}},
      {"computeCentroid", {"main"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *callers_procs = proc->get_callers();
    std::vector<std::string> callers = callers_ans[proc->get_name()];
    REQUIRE(callers_procs->size() == callers.size());
    for (auto &caller : callers) {
      REQUIRE(callers_procs->find(caller) != callers_procs->end());
    }
  }
}

TEST_CASE("PkbCallsStar_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<std::string, std::vector<std::string>> calls_star_ans = {
      {"main", {"computeCentroid", "printResults", "readPoint"}},
      {"readPoint", {}},
      {"printResults", {}},
      {"computeCentroid", {"readPoint"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *procs_calls_star = proc->get_calls_star();
    std::vector<std::string> calls_star = calls_star_ans[proc->get_name()];
    REQUIRE(procs_calls_star->size() == calls_star.size());
    for (auto &call_star : calls_star) {
      REQUIRE(procs_calls_star->find(call_star) != procs_calls_star->end());
    }
  }

  std::map<std::string, std::vector<std::string>> callers_star_ans = {
      {"main", {}},
      {"readPoint", {"computeCentroid", "main"}},
      {"printResults", {"main"}},
      {"computeCentroid", {"main"}}
  };
  for (auto proc : pkb.get_all_procedures()) {
    std::set<std::string> *callers_star_procs = proc->get_callers_star();
    std::vector<std::string> callers_star = callers_star_ans[proc->get_name()];
    REQUIRE(callers_star_procs->size() == callers_star.size());
    for (auto &caller_star : callers_star) {
      REQUIRE(callers_star_procs->find(caller_star) != callers_star_procs->end());
    }
  }
}

TEST_CASE("PkbCfg_AdvancedSample_Correct") {
  PKB pkb = InitialisePKB(kAdvancedSample);

  std::map<int, std::vector<int>> ans = {
      {1, {2}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {6}}, {6, {7, 10}}, {7, {8}}, {8, {9}}, {9, {6}},
      {10, {11, 12}}, {11, {13}}, {12, {13}}, {13, {14}}, {14, {15}}, {16, {17}}, {17, {18}},
  };
  std::map<int, std::set<int>> cfg_al = *pkb.get_cfg_al();
  REQUIRE(cfg_al.size() == ans.size());
  for (auto &[k, vals] : ans) {
    REQUIRE(cfg_al.find(k) != cfg_al.end());
    REQUIRE(cfg_al[k].size() == vals.size());
    for (auto &val : vals) {
      REQUIRE(cfg_al[k].find(val) != cfg_al[k].end());
    }
  }
}

TEST_CASE("PkbCfg_NestedIf_Correct") {
  PKB pkb = InitialisePKB(kNestedIf);

  std::map<int, std::vector<int>> ans = {
      {1, {2}}, {2, {3, 6}}, {3, {4, 5}}, {4, {9}}, {5, {9}}, {6, {7, 8}}, {7, {9}}, {8, {9}}
  };
  std::map<int, std::set<int>> cfg_al = *pkb.get_cfg_al();
  REQUIRE(cfg_al.size() == ans.size());
  for (auto &[k, vals] : ans) {
    REQUIRE(cfg_al.find(k) != cfg_al.end());
    REQUIRE(cfg_al[k].size() == vals.size());
    for (auto &val : vals) {
      REQUIRE(cfg_al[k].find(val) != cfg_al[k].end());
    }
  }
}

TEST_CASE("PkbCfgBip_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kCfgBipSample);

  std::map<int, std::vector<std::pair<int, int>>> ans = {
      {1, {{2, PKB::kNoBranch}}},
      {2, {{6, 2}}},
      {3, {{4, PKB::kNoBranch}}},
      {4, {{9, 4}}},
      {5, {}},
      {6, {{7, PKB::kNoBranch}}},
      {7, {{9, 7}}},
      {8, {{3, -2}}},
      {9, {{10, PKB::kNoBranch}, {11, PKB::kNoBranch}}},
      {10, {{8, -7}, {5, -4}}},
      {11, {{8, -7}, {5, -4}}}
  };

  std::map<int, std::set<std::pair<int, int>>> cfg_bip_al = *pkb.get_cfg_bip_al();
  REQUIRE(cfg_bip_al.size() == ans.size());
  for (auto&[u, vals] : ans) {
    REQUIRE(cfg_bip_al.find(u) != cfg_bip_al.end());
    REQUIRE(cfg_bip_al[u].size() == vals.size());
    for (auto &v: vals) {
      REQUIRE(cfg_bip_al[u].find(v) != cfg_bip_al[u].end());
    }
  }
}

TEST_CASE("PkbCFGBip_SecondSampleProgram_Correct") {
  PKB pkb = InitialisePKB(kCfgBipSecondSample);

  std::map<int, std::vector<std::pair<int, int>>> ans = {
      {1, {{4, 1}}},
      {2, {{4, 2}}},
      {3, {{4, 3}}},
      {4, {{5, PKB::kNoBranch}}},
      {5, {{6, PKB::kNoBranch}}},
      {6, {{7, PKB::kNoBranch}}},
      {7, {{2, -1}, {3, -2}}},
  };
  std::map<int, std::set<std::pair<int, int>>> cfg_bip_al = *pkb.get_cfg_bip_al();
  REQUIRE(cfg_bip_al.size() == ans.size());
  for (auto&[u, vals] : ans) {
    REQUIRE(cfg_bip_al.find(u) != cfg_bip_al.end());
    REQUIRE(cfg_bip_al[u].size() == vals.size());
    for (auto &v : vals) {
      REQUIRE(cfg_bip_al[u].find(v) != cfg_bip_al[u].end());
    }
  }
}