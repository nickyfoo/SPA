#include "query_optimizer.h"
#include "query_preprocessor.h"
#include "pql_query.h"
#include "catch.hpp"

TEST_CASE("GroupingSuchThat_ClausesWithNoSynonyms_AddsOnlyToNoSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(4, 5)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 1);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(4, 5) and Uses(4, 'v') and Parent(3, 6)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 3);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingSuchThat_ClausesWithNoReturnSynonyms_AddsOnlyToNoReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1, s2;\n"
                     "Select s1 such that Follows(s2, 5)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 1);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1, s2; variable v; assign a;\n"
                     "Select s1 such that Follows(s2, a) and Uses(4, v) and Parent(s2, 6)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 3);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingSuchThat_ClausesWithReturnSynonyms_AddsOnlyToReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1;\n"
                     "Select s1 such that Follows(s1, 5)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 1);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1, s2; variable v; assign a;\n"
                     "Select <s1, s2> such that Follows(s1, s2) and Uses(s1, v) and Parent(s2, 6)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 3);
  }
}

TEST_CASE("GroupingPattern_ClausesWithNoReturnSynonyms_AddsOnlyToNoReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "stmt s1; assign a;\n"
                     "Select s1 pattern a('x', _)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 1);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "stmt s1; variable v; assign a;\n"
                     "Select s1 pattern a(v, _) and a('x', _) and a(v, _'x'_)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 3);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingPattern_ClausesWithReturnSynonyms_AddsOnlyToReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a;\n"
                     "Select a pattern a('x', _)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 1);
  }

  SECTION("3 Clauses") {
    std::string ss = "variable v; assign a;\n"
                     "Select <v, a> pattern a(v, _) and a('x', _) and a(v, _'x'_)";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 3);
  }
}

TEST_CASE("GroupingWith_ClausesWithNoSynonyms_AddsOnlyToNoSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a;\n"
                     "Select a with 2 = 3";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 1);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "variable v; assign a;\n"
                     "Select <v, a> with 3 = 3 and 2 = 4 and 5 = 5";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 3);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingWith_ClausesWithNoReturnSynonyms_AddsOnlyToNoReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select a with 2 = s1.stmt#";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 1);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }

  SECTION("3 Clauses") {
    std::string ss = "variable v; assign a;\n"
                     "Select BOOLEAN with 3 = a.stmt# and v.varName = 'x' and a.stmt# = 5";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 3);
    REQUIRE(clause_groups[2].get_clauses().size() == 0);
  }
}

TEST_CASE("GroupingWith_ClausesWithReturnSynonyms_AddsOnlyToReturnSynGroup") {
  SECTION("1 Clause") {
    std::string ss = "assign a; stmt s1;\n"
                     "Select s1 with a.stmt# = s1.stmt#";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 1);
  }

  SECTION("3 Clauses") {
    std::string ss = "assign a, a2;\n"
                     "Select <a, a2> with 3 = a.stmt# and a.stmt# = a2.stmt# and a.stmt# = 5";
    auto query_preprocessor = QueryPreprocessor(ss);
    PQLQuery *clause = query_preprocessor.get_pql_query();
    auto query_optimizer = QueryOptimizer(clause->get_query_relationships(),
                                          clause->get_query_patterns(),
                                          clause->get_query_withs(),
                                          clause->get_query_entities());
    std::vector<ClauseGroup> clause_groups = query_optimizer.CreateGroupings();
    REQUIRE(clause_groups.size() == 3);
    REQUIRE(clause_groups[0].get_clauses().size() == 0);
    REQUIRE(clause_groups[1].get_clauses().size() == 0);
    REQUIRE(clause_groups[2].get_clauses().size() == 3);
  }
}
