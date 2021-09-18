#include <stdexcept>

#include "ast.h"
#include "lexer.h"

ExprOp ExprOpFromToken(const Token *t) {
  switch (t->kind_) {
    case TokenType::Plus:
      return ExprOp::Plus;
    case TokenType::Minus:
      return ExprOp::Minus;
    case TokenType::Multiply:
      return ExprOp::Times;
    case TokenType::Divide:
      return ExprOp::Divide;
    case TokenType::Modulo:
      return ExprOp::Modulo;
    default:
      throw std::invalid_argument("ExprOpFromToken: invalid token type");
  }
}

bool IsExpressionToken(const Token *t) {
  switch (t->kind_) {
    case TokenType::Name:
    case TokenType::Number:
    case TokenType::LParen:
    case TokenType::RParen:
    case TokenType::Plus:
    case TokenType::Minus:
    case TokenType::Multiply:
    case TokenType::Divide:
    case TokenType::Modulo:
      return true;
    default:
      return false;
  }
}

bool Precedes(const Token *op1, const Token *op2) {
  // return true if t1 has greater or equal precedence than t2
  switch (op1->kind_) {
    case TokenType::Multiply:
    case TokenType::Divide:
    case TokenType::Modulo:
      return true;

    case TokenType::Plus:
    case TokenType::Minus:
      switch (op2->kind_) {
        case TokenType::Plus:
        case TokenType::Minus:
          return true;

        case TokenType::Multiply:
        case TokenType::Divide:
        case TokenType::Modulo:
          return false;

        default:
          throw std::invalid_argument("Precedes: invalid operator token for op2");
      }

    default:
      throw std::invalid_argument("Precedes: invalid operator token for op1");
  }
}

ConstantNode *MakeConstantNodeFromToken(const Token *t) {
  return new ConstantNode(t->value_, {t->line_no_, t->col_no_});
};

IdentifierNode *MakeIdentifierNodeFromToken(const Token *t) {
  return new IdentifierNode(t->value_, {t->line_no_, t->col_no_});
};
