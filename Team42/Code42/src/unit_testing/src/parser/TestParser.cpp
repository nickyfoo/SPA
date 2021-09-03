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
                       "\tflag = 0;\n"             // stmtNo 1
                       "\tcall computeCentroid;\n" // stmtNo 2
                       "\tcall printResults;\n"    // stmtNo 3
                       "}";

  BufferedLexer *B = new BufferedLexer(source.c_str());
  State *s = new State{};
  ProgramNode *p = parseProgram(B, s);

  REQUIRE(p->procedures.size() == 1);

  ProcedureNode *pr = p->procedures[0];
  REQUIRE(pr->lineNo == 1);
  REQUIRE(pr->colNo == 0);

  AssignNode *a = static_cast<AssignNode *>(pr->stmtLst[0]);
  REQUIRE(a->var->name == "flag");
  REQUIRE(a->lineNo == 2);
  REQUIRE(a->colNo == 1);
  REQUIRE(a->stmtNo == 1);

  CallNode *c1 = static_cast<CallNode *>(pr->stmtLst[1]);
  REQUIRE(c1->proc->name == "computeCentroid");
  REQUIRE(c1->lineNo == 3);
  REQUIRE(c1->colNo == 1);
  REQUIRE(c1->stmtNo == 2);

  CallNode *c2 = static_cast<CallNode *>(pr->stmtLst[2]);
  REQUIRE(c2->proc->name == "printResults");
  REQUIRE(c2->lineNo == 4);
  REQUIRE(c2->colNo == 1);
  REQUIRE(c2->stmtNo == 3);
}

TEST_CASE("Test parseExpression") {
  std::string source = "1 + 2 *3-1";

  BufferedLexer *B = new BufferedLexer(source.c_str());

  ExpressionNode *p = parseExpression(B, new parser::State{});
  REQUIRE(p->op == ast::ExprOp::Minus);
  REQUIRE(p->exprString == "1 2 3 * + 1 -");
  REQUIRE(p->left->kind == ast::Kind::Expression);
  REQUIRE(p->right->kind == ast::Kind::Constant);

  ConstantNode *r = static_cast<ConstantNode *>(p->right);
  REQUIRE(r->value == "1");

  ExpressionNode *l = static_cast<ExpressionNode *>(p->left);
  REQUIRE(l->op == ast::ExprOp::Plus);
  REQUIRE(l->exprString == "1 2 3 * +");
  REQUIRE(l->left->kind == ast::Kind::Constant);
  REQUIRE(l->right->kind == ast::Kind::Expression);

  ConstantNode *ll = static_cast<ConstantNode *>(l->left);
  REQUIRE(ll->value == "1");

  ExpressionNode *lr = static_cast<ExpressionNode *>(l->right);
  REQUIRE(lr->op == ast::ExprOp::Times);
  REQUIRE(lr->exprString == "2 3 *");
  REQUIRE(lr->left->kind == ast::Kind::Constant);
  REQUIRE(lr->right->kind == ast::Kind::Constant);

  ConstantNode *lrl = static_cast<ConstantNode *>(lr->left);
  REQUIRE(lrl->value == "2");

  ConstantNode *lrr = static_cast<ConstantNode *>(lr->right);
  REQUIRE(lrr->value == "3");
}
