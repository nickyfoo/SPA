#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <parse.h>
#include <pkb.h>
#include "entities/statement.h"

TEST_CASE("PkbParent_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<int, std::vector<int>> children_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {15, 16, 17, 18}}, {15, {}}, {16, {}},
      {17, {}}, {18, {}}, {19, {20, 21, 22}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children = stmt->get_children();
    std::vector<int> children = children_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children->size() == children.size());
    for (auto &child : children) {
      REQUIRE(stmt_children->find(child) != stmt_children->end());
    }
  }

  std::map<int, std::vector<int>> parents_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {}}, {15, {14}}, {16, {14}}, {17, {14}},
      {18, {14}}, {19, {}}, {20, {19}}, {21, {19}}, {22, {19}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents = stmt->get_parents();
    std::vector<int> parents = parents_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents->size() == parents.size());
    for (auto &parent : parents) {
      REQUIRE(stmt_parents->find(parent) != stmt_parents->end());
    }
  }
}

TEST_CASE("PkbParentStar_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<int, std::vector<int>> children_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {15, 16, 17, 18}}, {15, {}}, {16, {}}, {17, {}},
      {18, {}}, {19, {20, 21, 22}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children_star = stmt->get_children_star();
    std::vector<int> children_star = children_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children_star->size() == children_star.size());
    for (auto &child_star : children_star) {
      REQUIRE(stmt_children_star->find(child_star) != stmt_children_star->end());
    }
  }

  std::map<int, std::vector<int>> parents_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {}}, {12, {}}, {13, {}}, {14, {}}, {15, {14}}, {16, {14}}, {17, {14}},
      {18, {14}}, {19, {}}, {20, {19}}, {21, {19}}, {22, {19}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents_star = stmt->get_parents_star();
    std::vector<int> parents_star = parents_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents_star->size() == parents_star.size());
    for (auto &parent_star : parents_star) {
      REQUIRE(stmt_parents_star->find(parent_star) != stmt_parents_star->end());
    }
  }
}

TEST_CASE("PkbParent_Nested_Correct") {
  PKB pkb = InitialisePKB(kNestedSource);

  std::map<int, std::vector<int>> children_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {5, 6}}, {5, {}}, {6, {7}}, {7, {8, 9}}, {8, {}}, {9, {}},
      {10, {11, 12}}, {11, {}}, {12, {13, 16}}, {13, {14, 15}}, {14, {}}, {15, {}}, {16, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children = stmt->get_children();
    std::vector<int> children = children_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children->size() == children.size());
    for (auto &child : children) {
      REQUIRE(stmt_children->find(child) != stmt_children->end());
    }
  }

  std::map<int, std::vector<int>> parents_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {4}}, {6, {4}}, {7, {6}}, {8, {7}}, {9, {7}},
      {10, {}}, {11, {10}}, {12, {10}}, {13, {12}}, {14, {13}}, {15, {13}}, {16, {12}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents = stmt->get_parents();
    std::vector<int> parents = parents_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents->size() == parents.size());
    for (auto &parent : parents) {
      REQUIRE(stmt_parents->find(parent) != stmt_parents->end());
    }
  }
}

TEST_CASE("PkbParentStar_Nested_Correct") {
  PKB pkb = InitialisePKB(kNestedSource);

  std::map<int, std::vector<int>> children_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {5, 6, 7, 8, 9}}, {5, {}}, {6, {7, 8, 9}}, {7, {8, 9}},
      {8, {}}, {9, {}}, {10, {11, 12, 13, 14, 15, 16}}, {11, {}}, {12, {13, 14, 15, 16}},
      {13, {14, 15}}, {14, {}}, {15, {}}, {16, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_children_star = stmt->get_children_star();
    std::vector<int> children_star = children_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_children_star->size() == children_star.size());
    for (auto &child_star : children_star) {
      REQUIRE(stmt_children_star->find(child_star) != stmt_children_star->end());
    }
  }

  std::map<int, std::vector<int>> parents_star_ans = {
      {1, {}}, {2, {}}, {3, {}}, {4, {}}, {5, {4}}, {6, {4}}, {7, {4, 6}}, {8, {4, 6, 7}},
      {9, {4, 6, 7}}, {10, {}}, {11, {10}}, {12, {10}}, {13, {10, 12}}, {14, {10, 12, 13}},
      {15, {10, 12, 13}}, {16, {10, 12}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_parents_star = stmt->get_parents_star();
    std::vector<int> parents_star = parents_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_parents_star->size() == parents_star.size());
    for (auto &parent_star : parents_star) {
      REQUIRE(stmt_parents_star->find(parent_star) != stmt_parents_star->end());
    }
  }
}
