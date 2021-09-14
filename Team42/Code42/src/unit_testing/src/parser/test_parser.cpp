#include <iostream>
#include <string>
#include <vector>

#include "ast.h"
#include "ast_utils.hpp"
#include "catch.hpp"
#include "lexer.h"
#include "parse.h"

TEST_CASE("ParseExpression_Constant_TreeHeightOne") {
  std::string source = "102";
  auto *expected = new ConstantNode("102", LocInfo{.line_no = 1, .col_no = 0});

  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  Node *res = ParseExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseExpression_AdditionMultiplication_TreeHeightTwo") {
  std::string source = "1 * 1 + x * y";
  auto *expected = new ExpressionNode(
      ExprOp::Plus,
      new ExpressionNode(ExprOp::Times, new ConstantNode("1", LocInfo{.line_no = 1, .col_no = 0}),
                         new ConstantNode("1", LocInfo{.line_no = 1, .col_no = 4}), "1 1 *",
                         LocInfo{.line_no = 1, .col_no = 0}),
      new ExpressionNode(ExprOp::Times, new IdentifierNode("x", LocInfo{.line_no = 1, .col_no = 8}),
                         new IdentifierNode("y", LocInfo{.line_no = 1, .col_no = 12}), "x y *",
                         LocInfo{.line_no = 1, .col_no = 8}),
      "1 1 * x y * +", LocInfo{.line_no = 1, .col_no = 0});

  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  Node *res = ParseExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseExpression_AdditionMultiplicationSubtractoin_TreeHeightThree") {
  std::string source = "1 + x * 4 - b";
  auto *expected = new ExpressionNode(
      ExprOp::Minus,
      new ExpressionNode(
          ExprOp::Plus, new ConstantNode("1", LocInfo{.line_no = 1, .col_no = 0}),
          new ExpressionNode(ExprOp::Times,
                             new IdentifierNode("x", LocInfo{.line_no = 1, .col_no = 4}),
                             new ConstantNode("4", LocInfo{.line_no = 1, .col_no = 8}), "x 4 *",
                             LocInfo{.line_no = 1, .col_no = 4}),
          "1 x 4 * +", LocInfo{.line_no = 1, .col_no = 0}),
      new IdentifierNode("b", LocInfo{.line_no = 1, .col_no = 12}), "1 x 4 * + b -",
      LocInfo{.line_no = 1, .col_no = 0});

  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  Node *res = ParseExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseExpression_AdditionMultiplicationDivision_TreeHeightThree") {
  std::string source = "3 * 1 / a + 5";
  auto *expected = new ExpressionNode(
      ExprOp::Plus,
      new ExpressionNode(
          ExprOp::Divide,
          new ExpressionNode(ExprOp::Times,
                             new ConstantNode("3", LocInfo{.line_no = 1, .col_no = 0}),
                             new ConstantNode("1", LocInfo{.line_no = 1, .col_no = 4}), "3 1 *",
                             LocInfo{.line_no = 1, .col_no = 0}),
          new IdentifierNode("a", LocInfo{.line_no = 1, .col_no = 8}), "3 1 * a /",
          LocInfo{.line_no = 1, .col_no = 0}),
      new ConstantNode("5", LocInfo{.line_no = 1, .col_no = 12}), "3 1 * a / 5 +",
      LocInfo{.line_no = 1, .col_no = 0});

  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  Node *res = ParseExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseExpression_Parenthesis_TreeHeightThree") {
  std::string source = "(3 + b) * 2 + 4";
  auto *expected = new ExpressionNode(
      ExprOp::Plus,
      new ExpressionNode(
          ExprOp::Times,
          new ExpressionNode(ExprOp::Plus,
                             new ConstantNode("3", LocInfo{.line_no = 1, .col_no = 1}),
                             new IdentifierNode("b", LocInfo{.line_no = 1, .col_no = 5}), "3 b +",
                             LocInfo{.line_no = 1, .col_no = 1}),
          new ConstantNode("2", LocInfo{.line_no = 1, .col_no = 10}), "3 b + 2 *",
          LocInfo{.line_no = 1, .col_no = 1}),
      new ConstantNode("4", LocInfo{.line_no = 1, .col_no = 14}), "3 b + 2 * 4 +",
      LocInfo{.line_no = 1, .col_no = 1});

  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  Node *res = ParseExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseExpression_MissingEndParenthesis_ThrowsException") {
  std::string source = "(3 + 1";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseExpression_MissingStartParenthesis_ThrowsException") {
  std::string source = "x + 3)";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseExpression_Missing_Operator_ThrowsException") {
  std::string source = "1 + 1 3 * 2";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseRelExpression_Gt_TreeHeightTwo") {
  std::string source = "x > 1";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("x", {1, 0}),
                                         new ConstantNode("1", {1, 4}), "x 1 >", {1, 0});
  Node *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseRelExpression_Gte_TreeHeightTwo") {
  std::string source = "Y >= Z";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new RelExpressionNode(RelExprOp::Gte, new IdentifierNode("Y", {1, 0}),
                                         new IdentifierNode("Z", {1, 5}), "Y Z >=", {1, 0});
  Node *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseRelExpression_Lt_TreeHeightTwo") {
  std::string source = "103 < 1";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new RelExpressionNode(RelExprOp::Lt, new ConstantNode("103", {1, 0}),
                                         new ConstantNode("1", {1, 6}), "103 1 <", {1, 0});
  Node *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseRelExpression_Lte_TreeHeightTwo") {
  std::string source = "50 <= foo";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new RelExpressionNode(RelExprOp::Lte, new ConstantNode("50", {1, 0}),
                                         new IdentifierNode("foo", {1, 6}), "50 foo <=", {1, 0});
  Node *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseRelExpression_Eq_TreeHeightTwo") {
  std::string source = "FoO == BaR";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new RelExpressionNode(RelExprOp::Eq, new IdentifierNode("FoO", {1, 0}),
                                         new IdentifierNode("BaR", {1, 7}), "FoO BaR ==", {1, 0});
  Node *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseRelExpression_Neq_TreeHeightTwo") {
  std::string source = "ast != tsa";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new RelExpressionNode(RelExprOp::Neq, new IdentifierNode("ast", {1, 0}),
                                         new IdentifierNode("tsa", {1, 7}), "ast tsa !=", {1, 0});
  Node *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseRelExpression_MissingOperator_ThrowsException") {
  std::string source = "x 1";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseRelExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseCondExpression_Not_TreeHeightTwo") {
  std::string source = "(!(foo > bar))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::Not,
      new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 3}),
                            new IdentifierNode("bar", {1, 9}), "foo bar >", {1, 3}),
      nullptr, "foo bar > !", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseCondExpression_And_TreeHeightTwo") {
  std::string source = "((foo > bar) && (1 == 1))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::And,
      new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 2}),
                            new IdentifierNode("bar", {1, 8}), "foo bar >", {1, 2}),
      new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 17}),
                            new ConstantNode("1", {1, 22}), "1 1 ==", {1, 17}),
      "foo bar > 1 1 == &&", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseCondExpression_Or_TreeHeightTwo") {
  std::string source = "((foo > bar) || (1 == 1))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::Or,
      new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 2}),
                            new IdentifierNode("bar", {1, 8}), "foo bar >", {1, 2}),
      new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 17}),
                            new ConstantNode("1", {1, 22}), "1 1 ==", {1, 17}),
      "foo bar > 1 1 == ||", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseCondExpression_Or_And_TreeHeightThree") {
  std::string source = "(((foo > bar) || (1 == 1)) && (1 <= x))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::And,
      new CondExpressionNode(
          CondExprOp::Or,
          new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 3}),
                                new IdentifierNode("bar", {1, 9}), "foo bar >", {1, 3}),
          new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 18}),
                                new ConstantNode("1", {1, 23}), "1 1 ==", {1, 18}),
          "foo bar > 1 1 == ||", {1, 1}),
      new RelExpressionNode(RelExprOp::Lte, new ConstantNode("1", {1, 31}),
                            new IdentifierNode("x", {1, 36}), "1 x <=", {1, 31}),
      "foo bar > 1 1 == || 1 x <= &&", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseCondExpression_And_Or_TreeHeightThree") {
  std::string source = "(((foo > bar) && (1 == 1)) || (1 <= x))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::Or,
      new CondExpressionNode(
          CondExprOp::And,
          new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 3}),
                                new IdentifierNode("bar", {1, 9}), "foo bar >", {1, 3}),
          new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 18}),
                                new ConstantNode("1", {1, 23}), "1 1 ==", {1, 18}),
          "foo bar > 1 1 == &&", {1, 1}),
      new RelExpressionNode(RelExprOp::Lte, new ConstantNode("1", {1, 31}),
                            new IdentifierNode("x", {1, 36}), "1 x <=", {1, 31}),
      "foo bar > 1 1 == && 1 x <= ||", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseCondExpression_Not_Or_TreeHeightThree") {
  std::string source = "(!((foo > bar) || (1 == 1)))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::Not,
      new CondExpressionNode(
          CondExprOp::Or,
          new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 4}),
                                new IdentifierNode("bar", {1, 10}), "foo bar >", {1, 4}),
          new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 19}),
                                new ConstantNode("1", {1, 24}), "1 1 ==", {1, 19}),
          "foo bar > 1 1 == ||", {1, 2}),
      nullptr, "foo bar > 1 1 == || !", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseCondExpression_Not_And_TreeHeightThree") {
  std::string source = "(!((foo > bar) && (1 == 1)))";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());

  auto *expected = new CondExpressionNode(
      CondExprOp::Not,
      new CondExpressionNode(
          CondExprOp::And,
          new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 4}),
                                new IdentifierNode("bar", {1, 10}), "foo bar >", {1, 4}),
          new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 19}),
                                new ConstantNode("1", {1, 24}), "1 1 ==", {1, 19}),
          "foo bar > 1 1 == &&", {1, 2}),
      nullptr, "foo bar > 1 1 == && !", {1, 0});

  auto *res = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseProgram_ValidProgram_CorrectProcedureCount") {
  std::string source =
      "procedure main {"
      "flag = 0;"
      "call computeCentroid;"
      "call printResults;"
      "}"
      "procedure readPoint {"
      "read x;"
      "read y;"
      "}"
      "procedure printResults {"
      "print flag;"
      "print cenX;"
      "print cenY;"
      "print normSq;"
      "}"
      "procedure computeCentroid {"
      "count = 0;"
      "cenX = 0;"
      "cenY = 0;"
      "call readPoint;"
      "while ((x != 0) && (y != 0)) {"
      "count = count + 1;"
      "cenX = cenX + x;"
      "cenY = cenY + y;"
      "call readPoint;"
      "}"
      "if (count == 0) then { flag = 1; }"
      "else {"
      "cenX = cenX / count;"
      "cenY = cenY / count;"
      "}"
      "normSq = cenX * cenX + cenY * cenY;"
      "}";

  auto *lexer = new BufferedLexer(source.c_str());
  ProgramNode *res = ParseProgram(lexer, new ParseState{});

  REQUIRE(res->get_procedures().size() == 4);
}
