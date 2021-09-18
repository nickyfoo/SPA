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
  auto *expected = new ConstantNode("102", {1, 0});

  BufferedLexer *lexer = new BufferedLexer(source);
  auto parse_result = ParseExpression(lexer, new ParseState{});

  REQUIRE(std::holds_alternative<ConstantNode *>(parse_result));
  auto res = std::get<ConstantNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("ParseExpression_AdditionMultiplication_TreeHeightTwo") {
  std::string source = "1 * 1 + x * y";
  auto *expected =
      new ExpressionNode(ExprOp::Plus,
                         new ExpressionNode(ExprOp::Times, new ConstantNode("1", {1, 0}),
                                            new ConstantNode("1", {1, 4}), {1, 0}),
                         new ExpressionNode(ExprOp::Times, new IdentifierNode("x", {1, 8}),
                                            new IdentifierNode("y", {1, 12}), {1, 8}),
                         {1, 0});

  BufferedLexer *lexer = new BufferedLexer(source);
  auto parse_result = ParseExpression(lexer, new ParseState{});

  REQUIRE(std::holds_alternative<ExpressionNode *>(parse_result));
  auto res = std::get<ExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "1 1 * x y * +");
}

TEST_CASE("ParseExpression_AdditionMultiplicationSubtractoin_TreeHeightThree") {
  std::string source = "1 + x * 4 - b";
  auto *expected = new ExpressionNode(
      ExprOp::Minus,
      new ExpressionNode(ExprOp::Plus, new ConstantNode("1", {1, 0}),
                         new ExpressionNode(ExprOp::Times, new IdentifierNode("x", {1, 4}),
                                            new ConstantNode("4", {1, 8}), {1, 4}),
                         {1, 0}),
      new IdentifierNode("b", {1, 12}), {1, 0});

  BufferedLexer *lexer = new BufferedLexer(source);
  auto parse_result = ParseExpression(lexer, new ParseState{});

  REQUIRE(std::holds_alternative<ExpressionNode *>(parse_result));
  auto res = std::get<ExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "1 x 4 * + b -");
}

TEST_CASE("ParseExpression_AdditionMultiplicationDivision_TreeHeightThree") {
  std::string source = "3 * 1 / a + 5";
  auto *expected = new ExpressionNode(
      ExprOp::Plus,
      new ExpressionNode(ExprOp::Divide,
                         new ExpressionNode(ExprOp::Times, new ConstantNode("3", {1, 0}),
                                            new ConstantNode("1", {1, 4}), {1, 0}),
                         new IdentifierNode("a", {1, 8}), {1, 0}),
      new ConstantNode("5", {1, 12}), {1, 0});

  BufferedLexer *lexer = new BufferedLexer(source);
  auto parse_result = ParseExpression(lexer, new ParseState{});

  REQUIRE(std::holds_alternative<ExpressionNode *>(parse_result));
  auto res = std::get<ExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "3 1 * a / 5 +");
}

TEST_CASE("ParseExpression_Parenthesis_TreeHeightThree") {
  std::string source = "(3 + b) * 2 + 4";
  auto *expected = new ExpressionNode(
      ExprOp::Plus,
      new ExpressionNode(ExprOp::Times,
                         new ExpressionNode(ExprOp::Plus, new ConstantNode("3", {1, 1}),
                                            new IdentifierNode("b", {1, 5}), {1, 1}),
                         new ConstantNode("2", {1, 10}), {1, 1}),
      new ConstantNode("4", {1, 14}), {1, 1});

  BufferedLexer *lexer = new BufferedLexer(source);
  auto parse_result = ParseExpression(lexer, new ParseState{});

  REQUIRE(std::holds_alternative<ExpressionNode *>(parse_result));
  auto res = std::get<ExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "3 b + 2 * 4 +");
}

TEST_CASE("ParseExpression_MissingEndParenthesis_ThrowsException") {
  std::string source = "(3 + 1";
  BufferedLexer *lexer = new BufferedLexer(source);
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseExpression_Missing_Operator_ThrowsException") {
  std::string source = "1 + 1 3 * 2";
  BufferedLexer *lexer = new BufferedLexer(source);
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseRelExpression_Gt_TreeHeightTwo") {
  std::string source = "x > 1";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected = new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("x", {1, 0}),
                                         new ConstantNode("1", {1, 4}), {1, 0});
  auto *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "x 1 >");
}

TEST_CASE("ParseRelExpression_Gte_TreeHeightTwo") {
  std::string source = "Y >= Z + 1";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new RelExpressionNode(RelExprOp::Gte, new IdentifierNode("Y", {1, 0}),
                            new ExpressionNode(ExprOp::Plus, new IdentifierNode("Z", {1, 5}),
                                               new ConstantNode("1", {1, 9}), {1, 5}),
                            {1, 0});
  auto *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "Y Z 1 + >=");
}

TEST_CASE("ParseRelExpression_Lt_TreeHeightTwo") {
  std::string source = "103 + 3 < 1 - 2";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new RelExpressionNode(RelExprOp::Lt,
                            new ExpressionNode(ExprOp::Plus, new ConstantNode("103", {1, 0}),
                                               new ConstantNode("3", {1, 6}), {1, 0}),
                            new ExpressionNode(ExprOp::Minus, new ConstantNode("1", {1, 10}),
                                               new ConstantNode("2", {1, 14}), {1, 10}),
                            {1, 0});
  auto *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "103 3 + 1 2 - <");
}

TEST_CASE("ParseRelExpression_Lte_TreeHeightTwo") {
  std::string source = "50 <= foo";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected = new RelExpressionNode(RelExprOp::Lte, new ConstantNode("50", {1, 0}),
                                         new IdentifierNode("foo", {1, 6}), {1, 0});
  auto *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "50 foo <=");
}

TEST_CASE("ParseRelExpression_Eq_TreeHeightTwo") {
  std::string source = "FoO /3== BaR";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new RelExpressionNode(RelExprOp::Eq,
                            new ExpressionNode(ExprOp::Divide, new IdentifierNode("FoO", {1, 0}),
                                               new ConstantNode("3", {1, 5}), {1, 0}),
                            new IdentifierNode("BaR", {1, 9}), {1, 0});
  auto *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "FoO 3 / BaR ==");
}

TEST_CASE("ParseRelExpression_Neq_TreeHeightTwo") {
  std::string source = "ast!=(tsa % 3)";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new RelExpressionNode(RelExprOp::Neq, new IdentifierNode("ast", {1, 0}),
                            new ExpressionNode(ExprOp::Modulo, new IdentifierNode("tsa", {1, 6}),
                                               new ConstantNode("3", {1, 12}), {1, 6}),
                            {1, 0});
  auto *res = ParseRelExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "ast tsa 3 % !=");
}

TEST_CASE("ParseRelExpression_MissingOperator_ThrowsException") {
  std::string source = "x 1";
  BufferedLexer *lexer = new BufferedLexer(source);
  REQUIRE_THROWS_AS(ParseRelExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("ParseCondExpression_Not_TreeHeightTwo") {
  std::string source = "(!(foo > bar))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new CondExpressionNode(CondExprOp::Not,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 3}),
                                                   new IdentifierNode("bar", {1, 9}), {1, 3}),
                             nullptr, {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});

  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > !");
}

TEST_CASE("ParseCondExpression_And_TreeHeightTwo") {
  std::string source = "((foo > bar) && (1 == 1))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new CondExpressionNode(CondExprOp::And,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 2}),
                                                   new IdentifierNode("bar", {1, 8}), {1, 2}),
                             new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 17}),
                                                   new ConstantNode("1", {1, 22}), {1, 17}),
                             {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});
  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > 1 1 == &&");
}

TEST_CASE("ParseCondExpression_Or_TreeHeightTwo") {
  std::string source = "((foo > bar) || (1 == 1))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected =
      new CondExpressionNode(CondExprOp::Or,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 2}),
                                                   new IdentifierNode("bar", {1, 8}), {1, 2}),
                             new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 17}),
                                                   new ConstantNode("1", {1, 22}), {1, 17}),
                             {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});
  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > 1 1 == ||");
}

TEST_CASE("ParseCondExpression_Or_And_TreeHeightThree") {
  std::string source = "(((foo > bar) || (1 == 1)) && (1 <= x))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected = new CondExpressionNode(
      CondExprOp::And,
      new CondExpressionNode(CondExprOp::Or,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 3}),
                                                   new IdentifierNode("bar", {1, 9}), {1, 3}),
                             new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 18}),
                                                   new ConstantNode("1", {1, 23}), {1, 18}),
                             {1, 2}),
      new RelExpressionNode(RelExprOp::Lte, new ConstantNode("1", {1, 31}),
                            new IdentifierNode("x", {1, 36}), {1, 31}),
      {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});
  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > 1 1 == || 1 x <= &&");
}

TEST_CASE("ParseCondExpression_And_Or_TreeHeightThree") {
  std::string source = "(((foo > bar) && (1 == 1)) || (1 <= x))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected = new CondExpressionNode(
      CondExprOp::Or,
      new CondExpressionNode(CondExprOp::And,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 3}),
                                                   new IdentifierNode("bar", {1, 9}), {1, 3}),
                             new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 18}),
                                                   new ConstantNode("1", {1, 23}), {1, 18}),
                             {1, 2}),
      new RelExpressionNode(RelExprOp::Lte, new ConstantNode("1", {1, 31}),
                            new IdentifierNode("x", {1, 36}), {1, 31}),
      {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});
  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > 1 1 == && 1 x <= ||");
}

TEST_CASE("ParseCondExpression_Not_Or_TreeHeightThree") {
  std::string source = "(!((foo > bar) || (1 == 1)))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected = new CondExpressionNode(
      CondExprOp::Not,
      new CondExpressionNode(CondExprOp::Or,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 4}),
                                                   new IdentifierNode("bar", {1, 10}), {1, 4}),
                             new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 19}),
                                                   new ConstantNode("1", {1, 24}), {1, 19}),
                             {1, 3}),
      nullptr, {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});
  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > 1 1 == || !");
}

TEST_CASE("ParseCondExpression_Not_And_TreeHeightThree") {
  std::string source = "(!((foo > bar) && (1 == 1)))";
  BufferedLexer *lexer = new BufferedLexer(source);

  auto *expected = new CondExpressionNode(
      CondExprOp::Not,
      new CondExpressionNode(CondExprOp::And,
                             new RelExpressionNode(RelExprOp::Gt, new IdentifierNode("foo", {1, 4}),
                                                   new IdentifierNode("bar", {1, 10}), {1, 4}),
                             new RelExpressionNode(RelExprOp::Eq, new ConstantNode("1", {1, 19}),
                                                   new ConstantNode("1", {1, 24}), {1, 19}),
                             {1, 3}),
      nullptr, {1, 1});

  auto parse_result = ParseCondExpression(lexer, new ParseState{});
  REQUIRE(std::holds_alternative<CondExpressionNode *>(parse_result));
  auto res = std::get<CondExpressionNode *>(parse_result);

  REQUIRE(IsNodeEqual(expected, res));
  REQUIRE(res->get_expr_string() == "foo bar > 1 1 == && !");
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

  auto *lexer = new BufferedLexer(source);
  ProgramNode *res = ParseProgram(lexer, new ParseState{});

  REQUIRE(res->get_procedures().size() == 4);
}
