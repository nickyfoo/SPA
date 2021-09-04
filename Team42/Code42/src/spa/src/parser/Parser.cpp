#include "AST.hpp"
#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <queue>
#include <stack>
#include <string>
#include <vector>

using namespace parser;

std::string parseError(std::string expected, int lineNo, int colNo) {
  return "Parse error: expected \"" + expected + "\" at " + std::to_string(lineNo) + ":" +
         std::to_string(colNo);
}

ast::ProgramNode *parser::parseProgram(lexer::BufferedLexer *lexer, State *state) {
  lexer::Token *t = lexer->peekNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  std::vector<ast::ProcedureNode *> procedures{};
  while (t->kind != lexer::Kind::End) {
    if (t->kind != lexer::Kind::Procedure) {
      throw parseError("procedure", t->lineNo, t->colNo);
    }

    ast::ProcedureNode *p = parseProcedure(lexer, state);
    procedures.push_back(p);

    t = lexer->peekNextToken();
  }

  return new ast::ProgramNode{procedures, startLine, startCol};
}

ast::ProcedureNode *parser::parseProcedure(lexer::BufferedLexer *lexer, State *state) {
  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Procedure) {
    throw parseError("procedure", t->lineNo, t->colNo);
  }

  // procedure name
  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (procedure)", t->lineNo, t->colNo);
  }
  std::string procName = t->value;

  // statements
  std::vector<ast::Node *> stmtLst = parseStmtLst(lexer, state);

  return new ast::ProcedureNode{procName, stmtLst, startLine, startCol};
}

ast::ReadNode *parser::parseRead(lexer::BufferedLexer *lexer, State *state) {
  int stmtNo = ++(state->stmtCount);

  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Read) {
    throw parseError("read", t->lineNo, t->colNo);
  }

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (variable)", t->lineNo, t->colNo);
  }
  auto var = new ast::IdentifierNode{t->value, t->lineNo, t->colNo};

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return new ast::ReadNode{var, stmtNo, startLine, startCol};
}

ast::PrintNode *parser::parsePrint(lexer::BufferedLexer *lexer, State *state) {
  int stmtNo = ++(state->stmtCount);

  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Print) {
    throw parseError("print", t->lineNo, t->colNo);
  }

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (variable)", t->lineNo, t->colNo);
  }
  auto var = new ast::IdentifierNode{t->value, t->lineNo, t->colNo};

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return new ast::PrintNode{var, stmtNo, startLine, startCol};
}

ast::CallNode *parser::parseCall(lexer::BufferedLexer *lexer, State *state) {
  int stmtNo = ++(state->stmtCount);

  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Call) {
    throw parseError("call", t->lineNo, t->colNo);
  }

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier> (procedure)", t->lineNo, t->colNo);
  }
  auto var = new ast::IdentifierNode{t->value, t->lineNo, t->colNo};

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return new ast::CallNode{var, stmtNo, startLine, startCol};
}

ast::WhileNode *parser::parseWhile(lexer::BufferedLexer *lexer, State *state) {
  int stmtNo = ++(state->stmtCount);

  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::While) {
    throw parseError("while", t->lineNo, t->colNo);
  }

  ast::CondExpressionNode *condExpr = parseCondExpression(lexer, state);
  std::vector<ast::Node *> stmtLst = parseStmtLst(lexer, state);

  return new ast::WhileNode{condExpr, stmtLst, stmtNo, startLine, startCol};
}

ast::IfNode *parser::parseIf(lexer::BufferedLexer *lexer, State *state) {
  int stmtNo = ++(state->stmtCount);

  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::If) {
    throw parseError("if", t->lineNo, t->colNo);
  }

  ast::CondExpressionNode *condExpr = parseCondExpression(lexer, state);

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Then) {
    throw parseError("then", t->lineNo, t->colNo);
  }

  std::vector<ast::Node *> thenStmtLst = parseStmtLst(lexer, state);

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Else) {
    throw parseError("else", t->lineNo, t->colNo);
  }

  std::vector<ast::Node *> elseStmtLst = parseStmtLst(lexer, state);

  return new ast::IfNode{condExpr, thenStmtLst, elseStmtLst, stmtNo, startLine, startCol};
}

ast::AssignNode *parser::parseAssign(lexer::BufferedLexer *lexer, State *state) {
  int stmtNo = ++(state->stmtCount);

  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::Identifier) {
    throw parseError("<Identifier>", t->lineNo, t->colNo);
  }

  auto var = new ast::IdentifierNode{t->value, t->lineNo, t->colNo};

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Equal) {
    throw parseError("=", t->lineNo, t->colNo);
  }

  ast::ExpressionNode *expr = parser::parseExpression(lexer, state);
  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::Semicolon) {
    throw parseError(";", t->lineNo, t->colNo);
  }

  return new ast::AssignNode{var, expr, stmtNo, startLine, startCol};
}

bool isExpressionToken(lexer::Token *t) {
  switch (t->kind) {
  case lexer::Kind::Constant:
  case lexer::Kind::Identifier:
  case lexer::Kind::LParen:
  case lexer::Kind::RParen:
  case lexer::Kind::Plus:
  case lexer::Kind::Minus:
  case lexer::Kind::Multiply:
  case lexer::Kind::Divide:
  case lexer::Kind::Modulo:
    return true;
  default:
    return false;
  }
}

ast::ExprOp exprOpFromToken(lexer::Token *t) {
  switch (t->kind) {
  case lexer::Kind::Plus:
    return ast::ExprOp::Plus;
  case lexer::Kind::Minus:
    return ast::ExprOp::Minus;
  case lexer::Kind::Multiply:
    return ast::ExprOp::Times;
  case lexer::Kind::Divide:
    return ast::ExprOp::Divide;
  case lexer::Kind::Modulo:
    return ast::ExprOp::Modulo;
  default:
    throw "Expected op but got non-op kind at " + std::to_string(t->lineNo) + ":" +
        std::to_string(t->colNo);
  }
}

std::string exprOpStringFromToken(lexer::Token *t) {
  switch (t->kind) {
  case lexer::Kind::Plus:
    return "+";
  case lexer::Kind::Minus:
    return "-";
  case lexer::Kind::Multiply:
    return "*";
  case lexer::Kind::Divide:
    return "/";
  case lexer::Kind::Modulo:
    return "%";
  default:
    throw "Expected op but got non-op kind at " + std::to_string(t->lineNo) + ":" +
        std::to_string(t->colNo);
  }
}

bool precedes(lexer::Token *t1, lexer::Token *t2) {
  // return true if t1 has greater or equal precedence than t2
  switch (t1->kind) {
  case lexer::Kind::Multiply:
  case lexer::Kind::Divide:
  case lexer::Kind::Modulo:
    return true;
  case lexer::Kind::Plus:
  case lexer::Kind::Minus:
    if (t2->kind == lexer::Kind::Plus || t2->kind == lexer::Kind::Minus) {
      return true;
    } else if (t2->kind == lexer::Kind::Multiply || t2->kind == lexer::Kind::Divide ||
               t2->kind == lexer::Kind::Modulo) {
      return false;
    }
  default:
    throw "Expected op but got non-op kind at " + std::to_string(t1->lineNo) + ":" +
        std::to_string(t1->colNo);
  }
}

ast::ExpressionNode *parser::parseExpression(lexer::BufferedLexer *lexer, State *state) {
  // use shunting yard algorithm to parse expressions
  std::stack<ast::Node *> outputStack{};
  std::stack<lexer::Token *> operatorStack{};
  std::stack<std::string> exprStringStack{};

  lexer::Token *t = lexer->peekNextToken();
  while (isExpressionToken(t)) {
    if (t->kind == lexer::Kind::Constant) {
      outputStack.push(new ast::ConstantNode{t->value, t->lineNo, t->colNo});
      exprStringStack.push(t->value);

    } else if (t->kind == lexer::Kind::Identifier) {
      outputStack.push(new ast::IdentifierNode{t->value, t->lineNo, t->colNo});
      exprStringStack.push(t->value);

    } else if (t->kind == lexer::Kind::LParen) {
      operatorStack.push(t);

    } else if (t->kind == lexer::Kind::RParen) {
      lexer::Token *op;
      while (!operatorStack.empty() && (op = operatorStack.top())->kind != lexer::Kind::LParen) {
        operatorStack.pop();

        if (operatorStack.empty()) {
          throw parseError("matching parenthesis", t->lineNo, t->colNo);
        }

        if (outputStack.size() < 2) {
          throw parseError("missing operand", op->lineNo, op->colNo);
        }

        ast::Node *right = outputStack.top();
        outputStack.pop();
        ast::Node *left = outputStack.top();
        outputStack.pop();
        ast::ExprOp exprOp = exprOpFromToken(op);

        std::string exprString = exprStringStack.top();
        exprStringStack.pop();
        exprString += " " + exprStringStack.top();
        exprStringStack.pop();
        exprString += " " + exprOpStringFromToken(op);

        outputStack.push(
            new ast::ExpressionNode{exprOp, left, right, exprString, op->lineNo, op->colNo});
        exprStringStack.push(exprString);
      }

      // flush matching (
      operatorStack.pop();
    } else {
      // t is an operator
      lexer::Token *op;
      while (!operatorStack.empty() && (op = operatorStack.top())->kind != lexer::Kind::LParen &&
             precedes(op, t)) {
        operatorStack.pop();

        if (outputStack.size() < 2) {
          throw parseError("missing operand", op->lineNo, op->colNo);
        }

        ast::Node *right = outputStack.top();
        outputStack.pop();
        ast::Node *left = outputStack.top();
        outputStack.pop();
        ast::ExprOp exprOp = exprOpFromToken(op);

        std::string exprString = exprStringStack.top();
        exprStringStack.pop();
        exprString = exprStringStack.top() + " " + exprString;
        exprStringStack.pop();
        exprString += " " + exprOpStringFromToken(op);

        outputStack.push(
            new ast::ExpressionNode{exprOp, left, right, exprString, op->lineNo, op->colNo});
        exprStringStack.push(exprString);
      }

      operatorStack.push(t);
    }

    // flush the token and peek next
    lexer->getNextToken();
    t = lexer->peekNextToken();
  }

  while (!operatorStack.empty()) {
    lexer::Token *op = operatorStack.top();
    operatorStack.pop();

    if (outputStack.size() < 2) {
      throw parseError("missing operand", op->lineNo, op->colNo);
    }

    ast::Node *right = outputStack.top();
    outputStack.pop();
    ast::Node *left = outputStack.top();
    outputStack.pop();
    ast::ExprOp exprOp = exprOpFromToken(op);

    std::string exprString = exprStringStack.top();
    exprStringStack.pop();
    exprString = exprStringStack.top() + " " + exprString;
    exprStringStack.pop();
    exprString += " " + exprOpStringFromToken(op);

    outputStack.push(
        new ast::ExpressionNode{exprOp, left, right, exprString, op->lineNo, op->colNo});
    exprStringStack.push(exprString);
  }

  ast::Node *n = outputStack.top();
  outputStack.pop();

  if (outputStack.size() > 0) {
    throw parseError("dangling operand", outputStack.top()->lineNo, outputStack.top()->colNo);
  }

  if (n->kind == ast::Kind::Expression) {
    ast::ExpressionNode *res = static_cast<ast::ExpressionNode *>(n);
    return res;
  }

  if (n->kind == ast::Kind::Constant) {
    ast::ConstantNode *c = static_cast<ast::ConstantNode *>(n);
    return new ast::ExpressionNode{ast::ExprOp::Noop, n, nullptr, c->value, n->lineNo, n->colNo};
  }

  if (n->kind == ast::Kind::Identifier) {
    ast::IdentifierNode *i = static_cast<ast::IdentifierNode *>(n);
    return new ast::ExpressionNode{ast::ExprOp::Noop, n, nullptr, i->name, n->lineNo, n->colNo};
  }

  throw parseError("expression, constant or identifier", n->lineNo, n->colNo);
}

ast::CondExpressionNode *parser::parseCondExpression(lexer::BufferedLexer *lexer, State *state) {
  // include parsing "(" and ")": that always comes with condExpr
  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  if (t->kind != lexer::Kind::LParen) {
    throw parseError("(", t->lineNo, t->colNo);
  }

  t = lexer->peekNextToken();

  if (t->kind == lexer::Kind::Not) {
    // flush "!"
    lexer->getNextToken();
    ast::Node *left = parser::parseCondExpression(lexer, state);

    t = lexer->getNextToken();
    if (t->kind != lexer::Kind::RParen) {
      throw parseError(")", t->lineNo, t->colNo);
    }

    return new ast::CondExpressionNode{ast::CondExprOp::Not, left, nullptr, startLine, startCol};
  }

  if (t->kind == lexer::Kind::Constant || t->kind == lexer::Kind::Identifier) {
    ast::Node *left = parser::parseRelExpression(lexer, state);

    t = lexer->getNextToken();
    if (t->kind != lexer::Kind::RParen) {
      throw parseError(")", t->lineNo, t->colNo);
    }

    return new ast::CondExpressionNode{ast::CondExprOp::None, left, nullptr, startLine, startCol};
  }

  if (t->kind != lexer::Kind::LParen) {
    throw parseError("! or ( or <Constant> or <Identifier>", t->lineNo, t->colNo);
  }

  // And or Or
  ast::Node *left = parser::parseCondExpression(lexer, state);
  ast::CondExprOp op;

  t = lexer->getNextToken();
  if (t->kind == lexer::Kind::And) {
    op = ast::CondExprOp::And;
  } else if (t->kind == lexer::Kind::Or) {
    op = ast::CondExprOp::Or;
  } else {
    throw parseError("|| or &&", t->lineNo, t->colNo);
  }

  ast::Node *right = parser::parseCondExpression(lexer, state);

  t = lexer->getNextToken();
  if (t->kind != lexer::Kind::RParen) {
    throw parseError(")", t->lineNo, t->colNo);
  }

  return new ast::CondExpressionNode{op, left, right, startLine, startCol};
}

ast::RelExpressionNode *parser::parseRelExpression(lexer::BufferedLexer *lexer, State *state) {
  lexer::Token *t = lexer->getNextToken();
  int startLine = t->lineNo;
  int startCol = t->colNo;

  ast::Node *left;
  if (t->kind == lexer::Kind::Constant) {
    left = new ast::ConstantNode{t->value, t->lineNo, t->colNo};
  } else if (t->kind == lexer::Kind::Identifier) {
    left = new ast::IdentifierNode{t->value, t->lineNo, t->colNo};
  } else {
    throw parseError("<Constant> or <Identifier>", t->lineNo, t->colNo);
  }

  t = lexer->getNextToken();
  ast::RelExprOp op;
  switch (t->kind) {
  case lexer::Kind::Gt:
    op = ast::RelExprOp::Gt;
    break;
  case lexer::Kind::Gte:
    op = ast::RelExprOp::Gte;
    break;
  case lexer::Kind::Lt:
    op = ast::RelExprOp::Lt;
    break;
  case lexer::Kind::Lte:
    op = ast::RelExprOp::Lte;
    break;
  case lexer::Kind::Eq:
    op = ast::RelExprOp::Eq;
    break;
  case lexer::Kind::Neq:
    op = ast::RelExprOp::Neq;
    break;
  default:
    throw parseError(">, >=, <, <=, ==, !=", t->lineNo, t->colNo);
  }

  t = lexer->getNextToken();
  ast::Node *right;
  if (t->kind == lexer::Kind::Constant) {
    right = new ast::ConstantNode{t->value, t->lineNo, t->colNo};
  } else if (t->kind == lexer::Kind::Identifier) {
    right = new ast::IdentifierNode{t->value, t->lineNo, t->colNo};
  } else {
    throw parseError("<Constant> or <Identifier>", t->lineNo, t->colNo);
  }

  return new ast::RelExpressionNode{op, left, right, startLine, startCol};
}

std::vector<ast::Node *> parser::parseStmtLst(lexer::BufferedLexer *lexer, State *state) {
  // include parsing of "{" and "}" that always surrounds a stmtLst
  lexer::Token *t = lexer->getNextToken();

  if (t->kind != lexer::Kind::LBrace) {
    throw parseError("{", t->lineNo, t->colNo);
  }

  std::vector<ast::Node *> stmtLst{};
  t = lexer->peekNextToken();

  while (t->kind != lexer::Kind::RBrace) {
    switch (t->kind) {
    case lexer::Kind::Read:
      stmtLst.push_back(parser::parseRead(lexer, state));
      break;
    case lexer::Kind::Print:
      stmtLst.push_back(parser::parsePrint(lexer, state));
      break;
    case lexer::Kind::Call:
      stmtLst.push_back(parser::parseCall(lexer, state));
      break;
    case lexer::Kind::While:
      stmtLst.push_back(parser::parseWhile(lexer, state));
      break;
    case lexer::Kind::If:
      stmtLst.push_back(parser::parseIf(lexer, state));
      break;
    case lexer::Kind::Identifier:
      stmtLst.push_back(parser::parseAssign(lexer, state));
      break;
    default:
      throw parseError("<Statement>", t->lineNo, t->colNo);
    }

    t = lexer->peekNextToken();
  }

  // flush the "}"
  t = lexer->getNextToken();

  return stmtLst;
}
