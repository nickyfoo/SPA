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
  ProgramNode *p = parseProgram(B);

  REQUIRE(p->procedures.size() == 1);
}

TEST_CASE("Test parseExpression") {
  std::string source = "1 + 2 *3-1";

  BufferedLexer *B = new BufferedLexer(source.c_str());

  ExpressionNode *p = parseExpression(B);
  REQUIRE(p->op == ast::ExprOp::Minus);
  REQUIRE(p->left->kind == ast::Kind::Expression);
  REQUIRE(p->right->kind == ast::Kind::Constant);

  ConstantNode *r = static_cast<ConstantNode *>(p->right);
  REQUIRE(r->value == "1");

  ExpressionNode *l = static_cast<ExpressionNode *>(p->left);
  REQUIRE(l->op == ast::ExprOp::Plus);
  REQUIRE(l->left->kind == ast::Kind::Constant);
  REQUIRE(l->right->kind == ast::Kind::Expression);

  ConstantNode *ll = static_cast<ConstantNode *>(l->left);
  REQUIRE(ll->value == "1");

  ExpressionNode *lr = static_cast<ExpressionNode *>(l->right);
  REQUIRE(lr->op == ast::ExprOp::Times);
  REQUIRE(lr->left->kind == ast::Kind::Constant);
  REQUIRE(lr->right->kind == ast::Kind::Constant);

  ConstantNode *lrl = static_cast<ConstantNode *> (lr->left);
  REQUIRE(lrl->value == "2");

  ConstantNode *lrr = static_cast<ConstantNode *> (lr->right);
  REQUIRE(lrr->value == "3");
}
