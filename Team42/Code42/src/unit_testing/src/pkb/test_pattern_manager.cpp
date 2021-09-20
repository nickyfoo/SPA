#include "parse.h"
#include <vector>
#include "pkb.h"
#include "catch.hpp"
#include "entities/statement.h"

TEST_CASE("TestAssignmentPattern_OnlyOneVariable", "[pattern_manager]") {
  SECTION("Constant") {
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("100");
    REQUIRE(PKB::TestAssignmentPattern(&s2, "100", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "100", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "1", true) == false);
  }

  SECTION("Variable") {
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("count");
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "nt", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "coun", true) == false);
  }
}

TEST_CASE("TestAssignmentPattern_OnlyOneOperator", "[pattern_manager]") {
  SECTION("Variable PLUS constant") {
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("count 1 +");
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count + 1", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count + 1", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "1", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "count", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "1", false) == false);
  }

  SECTION("Variable PLUS variable") {
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("x y +");
    REQUIRE(PKB::TestAssignmentPattern(&s2, "x + y", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "x + y", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "x", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "y", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "x", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s2, "y", false) == false);
  }
}

TEST_CASE("TestAssignmentPattern_TwoDifferentPrecedenceOperators_Success", "[pattern_manager]") {
  SECTION("Expression PLUS Expression") {
    Statement s1(1, NodeType::Assign);
    s1.set_expr_string("cenX cenX * cenY cenY * +");

    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX + cenY * cenY", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX + cenY * cenY", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenY * cenY", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX * cenX", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenY * cenY", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "cenX + cenY", true) == false);
  }

  SECTION("Variable PLUS Expression PLUS Expression") {
    // Which of the patterns match this assignment statement?
    // x = v + x * y + z * t
    Statement s1(1, NodeType::Assign);
    s1.set_expr_string("v x y * + z t * +");

    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x * y + z * t", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v", false) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "x * y", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x * y", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "y + z * t", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "x * y + z * t", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + x * y + z * t", true) == true);
  }
}

TEST_CASE("TestAssignmentPattern_MixedPrecedenceOperators_Success", "[pattern_manager]") {
    // Which of the patterns match this assignment statement? /
    // x = v + 3 / ((y - z) % n * t)
    Statement s1(1, NodeType::Assign);
    s1.set_expr_string("v 3 y z - n % t * / +");

    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + 3 / ((y - z) % n * t)", false) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "v + 3", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "((y - z) % n * t)", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "(y - z) % n", true) == true);
    REQUIRE(PKB::TestAssignmentPattern(&s1, " 3 / ((y - z)", true) == false);
    REQUIRE(PKB::TestAssignmentPattern(&s1, "3 / ((y - z) % n * t)", true) == true);
}
