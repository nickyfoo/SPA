#include <map>
#include <stdexcept>

#include "ast.h"
#include "lexer.h"

bool IsExprOp(TokenType t) {
  switch (t) {
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

bool IsRelExprOp(TokenType t) {
  switch (t) {
    case TokenType::Gt:
    case TokenType::Gte:
    case TokenType::Lt:
    case TokenType::Lte:
    case TokenType::Eq:
    case TokenType::Neq:
      return true;
    default:
      return false;
  }
}

bool IsCondExprOp(TokenType t) {
  switch (t) {
    case TokenType::Not:
    case TokenType::Or:
    case TokenType::And:
      return true;
    default:
      return false;
  }
}

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

RelExprOp RelExprOpFromToken(const Token *t) {
  switch (t->kind_) {
    case TokenType::Gt:
      return RelExprOp::Gt;
    case TokenType::Gte:
      return RelExprOp::Gte;
    case TokenType::Lt:
      return RelExprOp::Lt;
    case TokenType::Lte:
      return RelExprOp::Lte;
    case TokenType::Eq:
      return RelExprOp::Eq;
    case TokenType::Neq:
      return RelExprOp::Neq;
    default:
      throw std::invalid_argument("RelExprOpFromToken: invalid token type");
  }
}

CondExprOp CondExprOpFromToken(const Token *t) {
  switch (t->kind_) {
    case TokenType::Not:
      return CondExprOp::Not;
    case TokenType::And:
      return CondExprOp::And;
    case TokenType::Or:
      return CondExprOp::Or;
    default:
      throw std::invalid_argument("CondExprOpFromToken: invalid token type");
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
    case TokenType::Gt:
    case TokenType::Gte:
    case TokenType::Lt:
    case TokenType::Lte:
    case TokenType::Eq:
    case TokenType::Neq:
    case TokenType::Not:
    case TokenType::And:
    case TokenType::Or:
      return true;
    default:
      return false;
  }
}

// TODO: ADD SUPPORT FOR REL AND COND OPERATORS
bool Precedes(TokenType op1, TokenType op2) {
  std::map<TokenType, int> operator_weights{
      {TokenType::And, 1},    {TokenType::Or, 1},       {TokenType::Eq, 2},
      {TokenType::Neq, 2},    {TokenType::Gt, 3},       {TokenType::Gte, 3},
      {TokenType::Lt, 3},     {TokenType::Lte, 3},      {TokenType::Plus, 4},
      {TokenType::Minus, 4},  {TokenType::Multiply, 5}, {TokenType::Divide, 5},
      {TokenType::Modulo, 5}, {TokenType::Not, 6}};

  // return true if t1 has greater or equal precedence than t2
  if (operator_weights.find(op1) == operator_weights.end()) {
    throw std::invalid_argument("Precedes: invalid operator token for op2");
  }
  if (operator_weights.find(op2) == operator_weights.end()) {
    throw std::invalid_argument("Precedes: invalid operator token for op2");
  }

  return operator_weights[op1] >= operator_weights[op2];
}

ConstantNode *MakeConstantNodeFromToken(const Token *t) {
  return new ConstantNode(t->value_, {t->line_no_, t->col_no_});
};

IdentifierNode *MakeIdentifierNodeFromToken(const Token *t) {
  return new IdentifierNode(t->value_, {t->line_no_, t->col_no_});
};
