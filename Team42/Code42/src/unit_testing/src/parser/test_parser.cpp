#include <iostream>
#include <string>
#include <vector>

#include "ast_utils.hpp"
#include "catch.hpp"
#include "lexer.h"
#include "parse.h"

TEST_CASE("TestParseExpression 1") {
  std::string source = "102";
  auto *expected = new ConstantNode("102", LocInfo{.line_no = 1, .col_no = 0});

  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  Node *res = ParseExpression(lexer, new ParseState{});

  REQUIRE(IsNodeEqual(expected, res));
}

TEST_CASE("TestParseExpression 2") {
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

TEST_CASE("TestParseExpression 3") {
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

TEST_CASE("TestParseExpression 4") {
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

TEST_CASE("TestParseExpression 5") {
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

TEST_CASE("TestParseExpression 6") {
  std::string source = "(3 + 1";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("TestParseExpression 7") {
  std::string source = "x + 3)";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}

TEST_CASE("TestParseExpression 8") {
  std::string source = "1 + 1 3 * 2";
  BufferedLexer *lexer = new BufferedLexer(source.c_str());
  REQUIRE_THROWS_AS(ParseExpression(lexer, new ParseState{}), ParseException);
}
