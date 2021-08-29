#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <string>
#include <vector>

#include "catch.hpp"

using namespace std;
using namespace lexer;
using namespace parser;
using namespace ast;

TEST_CASE("Test parseProgram") {
  std::string source = "procedure main {\n"
                       "\tflag = 0;\n"
                       "\tcall computeCentroid;\n"
                       "\tcall printResults;\n"
                       "}";

  BufferedLexer *B = new BufferedLexer(source.c_str());
  ProgramNode p = parseProgram(B);

  REQUIRE(p.procedures.size() == 1);
}

TEST_CASE("Test parseExpression") {
  std::string source = "1 + 2 *3-1";

  BufferedLexer *B = new BufferedLexer(source.c_str());
  ExpressionNode p = parseExpression(B);

  REQUIRE(p.op == ast::ExprOp::Minus);
  ast::Node *tmp = &p.left;
  if (tmp->kind == ast::Kind::Constant) {
    std::cout << "la" << std::endl;
  }

  std::cout << static_cast<ast::ConstantNode *>(tmp)->value << std::endl;
  REQUIRE(p.left.kind == ast::Kind::Expression);
  REQUIRE(p.right.kind == ast::Kind::Constant);
}
