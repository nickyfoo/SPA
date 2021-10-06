#include "parse.h"
#include <vector>
#include "pkb.h"
#include "catch.hpp"
#include "entities/statement.h"

TEST_CASE("TestAssignmentPattern_OnlyOneVariable", "[pattern_manager]") {
  SECTION("Constant") {
    PKB pkb;
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("100");

    REQUIRE(pkb.TestAssignmentPattern(&s2, "100", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "100", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "1", true) == false);
  }

  SECTION("Variable") {
    PKB pkb;
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("count");

    REQUIRE(pkb.TestAssignmentPattern(&s2, "count", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "count", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "nt", true) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "coun", true) == false);
  }
}

TEST_CASE("TestAssignmentPattern_OnlyOneOperator", "[pattern_manager]") {
  SECTION("Variable PLUS constant") {
    PKB pkb;
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("count 1 +");

    REQUIRE(pkb.TestAssignmentPattern(&s2, "count + 1", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "count + 1", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "count", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "1", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "count", false) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "1", false) == false);
  }

  SECTION("Variable PLUS variable") {
    PKB pkb;
    Statement s2(1, NodeType::Assign);
    s2.set_expr_string("x y +");

    REQUIRE(pkb.TestAssignmentPattern(&s2, "x + y", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "x + y", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "x", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "y", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "x", false) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s2, "y", false) == false);
  }
}

TEST_CASE("TestAssignmentPattern_TwoDifferentPrecedenceOperators_Success", "[pattern_manager]") {
  SECTION("Expression PLUS Expression") {
    PKB pkb;
    Statement s1(1, NodeType::Assign);
    s1.set_expr_string("cenX cenX * cenY cenY * +");

    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenX * cenX + cenY * cenY", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenX * cenX + cenY * cenY", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenX * cenX", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenY * cenY", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenX * cenX", false) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenY * cenY", false) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "cenX + cenY", true) == false);
  }

  SECTION("Variable PLUS Expression PLUS Expression") {
    // Which of the patterns match this assignment statement?
    // x = v + x * y + z * t
    PKB pkb;
    Statement s1(1, NodeType::Assign);
    s1.set_expr_string("v x y * + z t * +");

    REQUIRE(pkb.TestAssignmentPattern(&s1, "v + x * y + z * t", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "v", false) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "v", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "x * y", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "v + x", true) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "v + x * y", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "y + z * t", true) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "x * y + z * t", true) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "v + x * y + z * t", true) == true);
  }
}

TEST_CASE("TestAssignmentPattern_MixedPrecedenceOperators_Success", "[pattern_manager]") {
    // Which of the patterns match this assignment statement? /
    // x = v + 3 / ((y - z) % n * t)
    PKB pkb;
    Statement s1(1, NodeType::Assign);
    s1.set_expr_string("v 3 y z - n % t * / +");

    REQUIRE(pkb.TestAssignmentPattern(&s1, "v + 3 / ((y - z) % n * t)", false) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "v + 3", true) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "y\t - \tz", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "((y - z) % n * t)", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "(y - z) % n", true) == true);
    REQUIRE(pkb.TestAssignmentPattern(&s1, " 3 / ((y - z)", true) == false);
    REQUIRE(pkb.TestAssignmentPattern(&s1, "3 / ((y - z) % n * t)", true) == true);
}

TEST_CASE("TestIfWhilePattern_SingleCondition_Success", "[pattern_manager]") {
  PKB pkb;
  Statement s1(1, NodeType::If);
  s1.set_expr_string("count i ==");

  REQUIRE(pkb.TestIfWhilePattern(&s1, "count") == true);
  REQUIRE(pkb.TestIfWhilePattern(&s1, "co") == false);
  REQUIRE(pkb.TestIfWhilePattern(&s1, "i") == true);
  REQUIRE(pkb.TestIfWhilePattern(&s1, "v") == false);
}

TEST_CASE("TestIfWhilePattern_MultipleConditions_Success", "[pattern_manager]") {
  PKB pkb;
  Statement s1(1, NodeType::While);
  s1.set_expr_string("7 x 0 != yellow 0 != &&");

  REQUIRE(pkb.TestIfWhilePattern(&s1, "yellow") == true);
  REQUIRE(pkb.TestIfWhilePattern(&s1, "y") == false);
  REQUIRE(pkb.TestIfWhilePattern(&s1, "x") == true);
  REQUIRE(pkb.TestIfWhilePattern(&s1, "a") == false);
}
