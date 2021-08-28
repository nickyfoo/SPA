#include "AST.hpp"
#include "Lexer.h"
#include "Parser.h"
#include <string>
#include <vector>

using namespace parser;

std::string parseError(std::string expected, int lineNo, int colNo) {
  return "Parse error: expected \"" + expected + "\" at " + std::to_string(lineNo) + ":" +
         std::to_string(colNo);
}

ast::ProgramNode parseProgram(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.peekNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  std::vector<ast::ProcedureNode> procedures{};
  while (t->kind != lexer::Kind::End) {
    if (t->kind != lexer::Kind::Procedure) {
      throw parseError("procedure", t->lineNo, t->colNo);
    }

    ast::ProcedureNode p = parseProcedure(lexer);
    procedures.push_back(p);
  }

  return ast::ProgramNode{procedures, startLine, startCol};
}

ast::ProcedureNode parseProcedure(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Procedure) {
    throw parseError("procedure", t->lineNo, t->colNo);
  }

  // procedure name
  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (procedure)", t->lineNo, t->colNo);
  }
  std::string procName = t->value;

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::LBrace) {
    throw parseError("{", t->lineNo, t->colNo);
  }

  // statements
  std::vector<ast::Node> stmtLst = parseStmtLst(lexer);

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::RBrace) {
    throw parseError("}", t->lineNo, t->colNo);
  }

  return ast::ProcedureNode{stmtLst, startLine, startCol};
}

ast::ReadNode parseRead(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Read) {
    throw parseError("read", t->lineNo, t->colNo);
  }

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (variable)", t->lineNo, t->colNo);
  }
  ast::IdentifierNode var{t->value, t->lineNo, t->colNo};

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return ast::ReadNode{var, startLine, startCol};
}

ast::PrintNode parsePrint(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Print) {
    throw parseError("print", t->lineNo, t->colNo);
  }

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (variable)", t->lineNo, t->colNo);
  }
  ast::IdentifierNode var{t->value, t->lineNo, t->colNo};

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return ast::PrintNode{var, startLine, startCol};
}

ast::CallNode parseCall(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Call) {
    throw parseError("call", t->lineNo, t->colNo);
  }

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (procedure)", t->lineNo, t->colNo);
  }
  ast::IdentifierNode var{t->value, t->lineNo, t->colNo};

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return ast::CallNode{var, startLine, startCol};
}

ast::WhileNode parseWhile(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::While) {
    throw parseError("while", t->lineNo, t->colNo);
  }

  ast::CondExpressionNode condExpr = parseCondExpression(lexer);
  std::vector<ast::Node> stmtLst = parseStmtLst(lexer);

  return ast::WhileNode{condExpr, stmtLst, startLine, startCol};
}

ast::IfNode parseIf(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::If) {
    throw parseError("if", t->lineNo, t->colNo);
  }

  ast::CondExpressionNode condExpr = parseCondExpression(lexer);

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Then) {
    throw parseError("then", t->lineNo, t->colNo);
  }

  std::vector<ast::Node> thenStmtLst = parseStmtLst(lexer);

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Else) {
    throw parseError("else", t->lineNo, t->colNo);
  }

  std::vector<ast::Node> elseStmtLst = parseStmtLst(lexer);

  return ast::IfNode{condExpr, thenStmtLst, elseStmtLst, startLine, startCol};
}

ast::AssignNode parseAssign(lexer::BufferedLexer lexer) {
  lexer::Token *t = lexer.getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> variable", t->lineNo, t->colNo);
  }

  ast::IdentifierNode var = ast::IdentifierNode{t->value, t->lineNo, t->colNo};

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Equal) {
    throw parseError("=", t->lineNo, t->colNo);
  }

  ast::ExpressionNode expr = parser::parseExpression(lexer);

  t = lexer.getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return ast::AssignNode{var, expr, startLine, startCol};
}

ast::ExpressionNode parseExpression(lexer::BufferedLexer lexer) { throw "Not implemented!"; }

ast::CondExpressionNode parseCondExpression(lexer::BufferedLexer lexer) {
  // include parsing "(" and ")" that always comes with condExpr
  throw "Not implemented!";
}

ast::RelExpressionNode parseRelExpression(lexer::BufferedLexer lexer) { throw "Not implemented!"; }

std::vector<ast::Node> parseStmtLst(lexer::BufferedLexer lexer) {
  // include parsing of "{" and "}" that always surrounds a stmtLst
  lexer::Token *t = lexer.getNextToken();

  if (t->kind != lexer::Kind::LBrace) {
    throw parseError("{", t->lineNo, t->colNo);
  }

  std::vector<ast::Node> stmtLst{};
  while (t->kind != lexer::Kind::RBrace) {
    switch (t->kind) {
    case lexer::Kind::Read:
      stmtLst.push_back(parser::parseRead(lexer));
      break;
    case lexer::Kind::Print:
      stmtLst.push_back(parser::parsePrint(lexer));
      break;
    case lexer::Kind::Call:
      stmtLst.push_back(parser::parseCall(lexer));
      break;
    case lexer::Kind::While:
      stmtLst.push_back(parser::parseWhile(lexer));
      break;
    case lexer::Kind::If:
      stmtLst.push_back(parser::parseIf(lexer));
      break;
    case lexer::Kind::Identifier:
      stmtLst.push_back(parser::parseAssign(lexer));
      break;
    default:
      throw parseError("<Statement>", t->lineNo, t->colNo);
    }

    t = lexer.peekNextToken();
  }

  // flush the "}"
  t = lexer.getNextToken();

  return stmtLst;
}
