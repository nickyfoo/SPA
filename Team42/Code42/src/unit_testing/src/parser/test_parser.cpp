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

TEST_CASE("TestParseExpression 3") { std::string source = "1 + x * 4 - b"; }

TEST_CASE("TestParseExpression 4") { std::string source = "3 * 1 / a + 5"; }

TEST_CASE("TestParseExpression 5") { std::string source = "(3 + b) * 2 + 4"; }

TEST_CASE("TestParseExpression 6") { std::string source = "(3 + 1"; }

TEST_CASE("TestParseExpression 7") { std::string source = "x + 3)"; }

TEST_CASE("TestParseExpression 8") { std::string source = "1 + 1 3 * 2"; }
