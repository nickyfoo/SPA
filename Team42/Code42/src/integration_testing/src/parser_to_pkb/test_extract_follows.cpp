#include "catch.hpp"
#include <vector>
#include <chrono>

#include "../test_utils.hpp"
#include <parse.h>
#include <pkb.h>
#include "entities/statement.h"

TEST_CASE("PkbFollows_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<int, std::vector<int>> follows_ans = {
      {1, {2}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {}}, {6, {7}}, {7, {8}}, {8, {9}}, {9, {}},
      {10, {11}}, {11, {12}}, {12, {13}}, {13, {14}}, {14, {19}}, {15, {16}}, {16, {17}},
      {17, {18}}, {18, {}}, {19, {23}}, {20, {}}, {21, {22}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto &follower : followers) {
      REQUIRE(stmt_followers->find(follower) != stmt_followers->end());
    }
  }

  std::map<int, std::vector<int>> followees_ans = {
      {1, {}}, {2, {1}}, {3, {2}}, {4, {}}, {5, {4}}, {6, {}}, {7, {6}}, {8, {7}}, {9, {8}},
      {10, {}}, {11, {10}}, {12, {11}}, {13, {12}}, {14, {13}}, {15, {}}, {16, {15}}, {17, {16}},
      {18, {17}}, {19, {14}}, {20, {}}, {21, {}}, {22, {21}}, {23, {19}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto &followee : followees) {
      REQUIRE(stmt_followees->find(followee) != stmt_followees->end());
    }
  }
}

TEST_CASE("PkbFollowsStar_SampleProgram_Correct") {
  PKB pkb = InitialisePKB(kSampleSource);

  std::map<int, std::vector<int>> follows_star_ans = {
      {1, {2, 3}}, {2, {3}}, {3, {}}, {4, {5}}, {5, {}}, {6, {7, 8, 9}}, {7, {8, 9}}, {8, {9}},
      {9, {}}, {10, {11, 12, 13, 14, 19, 23}}, {11, {12, 13, 14, 19, 23}}, {12, {13, 14, 19, 23}},
      {13, {14, 19, 23}}, {14, {19, 23}}, {15, {16, 17, 18}}, {16, {17, 18}}, {17, {18}},
      {18, {}}, {19, {23}}, {20, {}}, {21, {22}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto &follower_star : followers_star) {
      REQUIRE(stmt_followers_star->find(follower_star) != stmt_followers_star->end());
    }
  }

  std::map<int, std::vector<int>> followees_star_ans = {
      {1, {}}, {2, {1}}, {3, {1, 2}}, {4, {}}, {5, {4}}, {6, {}}, {7, {6}}, {8, {6, 7}},
      {9, {6, 7, 8}}, {10, {}}, {11, {10}}, {12, {10, 11}}, {13, {10, 11, 12}},
      {14, {10, 11, 12, 13}}, {15, {}}, {16, {15}}, {17, {15, 16}}, {18, {15, 16, 17}},
      {19, {10, 11, 12, 13, 14}}, {20, {}}, {21, {}}, {22, {21}},
      {23, {10, 11, 12, 13, 14, 19}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees_star = stmt->get_followees_star();
    std::vector<int> followees_star = followees_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees_star->size() == followees_star.size());
    for (auto &followee_star : followees_star) {
      REQUIRE(stmt_followees_star->find(followee_star) != stmt_followees_star->end());
    }
  }
}

TEST_CASE("PkbFollows_NestedSource_Correct") {
  PKB pkb = InitialisePKB(kNestedSource);

  std::map<int, std::vector<int>> follows_ans = {
      {1, {2}}, {2, {3}}, {3, {4}}, {4, {10}}, {5, {6}}, {6, {}}, {7, {}}, {8, {}}, {9, {}},
      {10, {}}, {11, {12}}, {12, {}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}, {17, {}},
      {18, {}}, {19, {}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers = stmt->get_followers();
    std::vector<int> followers = follows_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers->size() == followers.size());
    for (auto &follower : followers) {
      REQUIRE(stmt_followers->find(follower) != stmt_followers->end());
    }
  }

  std::map<int, std::vector<int>> followees_ans = {
      {1, {}}, {2, {1}}, {3, {2}}, {4, {3}}, {5, {}}, {6, {5}}, {7, {}}, {8, {}}, {9, {}},
      {10, {4}}, {11, {}}, {12, {11}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}, {17, {}},
      {18, {}}, {19, {}}, {20, {}}, {21, {}}, {22, {}}, {23, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees = stmt->get_followees();
    std::vector<int> followees = followees_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees->size() == followees.size());
    for (auto &followee : followees) {
      REQUIRE(stmt_followees->find(followee) != stmt_followees->end());
    }
  }
}

TEST_CASE("PkbFollowsStar_Nested_Correct") {
  PKB pkb = InitialisePKB(kNestedSource);

  std::map<int, std::vector<int>> follows_star_ans = {
      {1, {2, 3, 4, 10}}, {2, {3, 4, 10}}, {3, {4, 10}}, {4, {10}}, {5, {6}}, {6, {}}, {7, {}},
      {8, {}}, {9, {}}, {10, {}}, {11, {12}}, {12, {}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}
  };

  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followers_star = stmt->get_followers_star();
    std::vector<int> followers_star = follows_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followers_star->size() == followers_star.size());
    for (auto &follower_star : followers_star) {
      REQUIRE(stmt_followers_star->find(follower_star) != stmt_followers_star->end());
    }
  }

  std::map<int, std::vector<int>> followees_star_ans = {
      {1, {}}, {2, {1}}, {3, {1, 2}}, {4, {1, 2, 3}}, {5, {}}, {6, {5}}, {7, {}}, {8, {}}, {9, {}},
      {10, {1, 2, 3, 4}}, {11, {}}, {12, {11}}, {13, {}}, {14, {}}, {15, {}}, {16, {}}
  };
  for (auto stmt : pkb.get_all_statements()) {
    std::set<int> *stmt_followees_star = stmt->get_followees_star();
    std::vector<int> followees_star = followees_star_ans[stmt->get_stmt_no()];
    REQUIRE(stmt_followees_star->size() == followees_star.size());
    for (auto &followee_star : followees_star) {
      REQUIRE(stmt_followees_star->find(followee_star) != stmt_followees_star->end());
    }
  }
}
