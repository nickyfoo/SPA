#include <vector>
#include "statement.h"
#include "catch.hpp"

TEST_CASE("GetStmtNo_ValidStatement_ReturnsNumber", "[statement][entity]") {
  NodeType stmt_type = NodeType::Assign;

  SECTION("Test values up to 10") {
    for (int i = 1; i < 10; i++) {
      Statement s(i, stmt_type);
      REQUIRE(s.get_stmt_no() == i);
    }
  }

  SECTION("Test values multiples of 10 up till 100") {
    for (int i = 10; i < 100; i += 10) {
      Statement s(i, stmt_type);
      REQUIRE(s.get_stmt_no() == i);
    }
  }
}

TEST_CASE("GetKind_ValidStatement_ReturnsNodeType", "[statement][entity]") {
  std::vector<NodeType> stmt_kinds = {
      NodeType::Assign, NodeType::If, NodeType::While,
      NodeType::Read, NodeType::Print, NodeType::Call
  };

  for (auto it = begin(stmt_kinds); it != end(stmt_kinds); ++it) {
    Statement s(rand(), *it);
    REQUIRE(s.get_kind() == *it);
  }
}

TEST_CASE("SetGetExprString_SetsBeforeGet_ReturnsExprString", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  std::string expr_string = "x y +";
  s.set_expr_string(expr_string);
  REQUIRE(s.get_expr_string() == expr_string);
}

TEST_CASE("SetGetCalledProcName_SetsBeforeGet_ReturnsProcName", "[statement][entity]") {
  SECTION("Call statement") {
    Statement s(rand(), NodeType::Call);
    std::string proc_name = "main";
    s.set_called_proc_name(proc_name);
    REQUIRE(s.get_called_proc_name() == proc_name);
  }

  SECTION("Non call statement") {
    Statement s(rand(), NodeType::Assign);
    REQUIRE(s.get_called_proc_name() == "");
  }
}

TEST_CASE("AddGetFollower_SingleFollower_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int follower_stmt = rand();
  s.AddFollower(follower_stmt);
  auto follower_set = s.get_followers();

  REQUIRE(follower_set->size() == 1);
  REQUIRE(follower_set->count(follower_stmt) == 1);
}

TEST_CASE("AddGetFollowerStar_SingleFollowerStar_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int follower_star_stmt = rand();
  s.AddFollowerStar(follower_star_stmt);
  auto follower_set = s.get_followers_star();

  REQUIRE(follower_set->size() == 1);
  REQUIRE(follower_set->count(follower_star_stmt) == 1);
}

TEST_CASE("AddGetFollowee_SingleFollowee_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int followee_stmt = rand();
  s.AddFollowee(followee_stmt);
  auto followee_set = s.get_followees();

  REQUIRE(followee_set->size() == 1);
  REQUIRE(followee_set->count(followee_stmt) == 1);
}

TEST_CASE("AddGetFolloweeStar_SingleFolloweeStar_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int followee_star_stmt = rand();
  s.AddFolloweeStar(followee_star_stmt);
  auto followee_set = s.get_followees_star();

  REQUIRE(followee_set->size() == 1);
  REQUIRE(followee_set->count(followee_star_stmt) == 1);
}

TEST_CASE("AddGetParents_SingleParent_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int parent_stmt = rand();
  s.AddParent(parent_stmt);
  auto parents_set = s.get_parents();

  REQUIRE(parents_set->size() == 1);
  REQUIRE(parents_set->count(parent_stmt) == 1);
}

TEST_CASE("AddGetParentsStar_SingleParentStar_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int parent_star_stmt = rand();
  s.AddParentStar(parent_star_stmt);
  auto parents_set = s.get_parents_star();

  REQUIRE(parents_set->size() == 1);
  REQUIRE(parents_set->count(parent_star_stmt) == 1);
}

TEST_CASE("AddGetChildren_SingleChild_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int child_stmt = rand();
  s.AddChild(child_stmt);
  auto children_set = s.get_children();

  REQUIRE(children_set->size() == 1);
  REQUIRE(children_set->count(child_stmt) == 1);
}

TEST_CASE("AddGetChildrenStar_SingleChildStar_ReturnsIntSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  int child_star_stmt = rand();
  s.AddChildStar(child_star_stmt);
  auto children_star_set = s.get_children_star();

  REQUIRE(children_star_set->size() == 1);
  REQUIRE(children_star_set->count(child_star_stmt) == 1);
}

TEST_CASE("AddGetUses_SingleUsesVariable_ReturnsUsesSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  std::string uses_variable = "x";
  s.AddUses(uses_variable);
  auto uses_set = s.get_uses();

  REQUIRE(uses_set->size() == 1);
  REQUIRE(uses_set->count(uses_variable) == 1);
  REQUIRE(uses_set->count("X") == 0);
  REQUIRE(uses_set->count("y") == 0);
}

TEST_CASE("AddGetModifies_SingleModifies_ReturnsUsesSet", "[statement][entity]") {
  Statement s(rand(), NodeType::Assign);
  std::string modifies_variable = "x";
  s.AddModifies(modifies_variable);
  auto modifies_set = s.get_modifies();

  REQUIRE(modifies_set->size() == 1);
  REQUIRE(modifies_set->count(modifies_variable) == 1);
  REQUIRE(modifies_set->count("X") == 0);
  REQUIRE(modifies_set->count("y") == 0);
}
