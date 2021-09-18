#include <stdexcept>

#include "parse.h"
#include "string_utils.h"

RelExpressionNode *ParseRelExpression(BufferedLexer *lexer, ParseState *state) {
  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  auto parse_result = ParseExpression(lexer, state);
  RelExpressionNodeChild *left;
  if (std::holds_alternative<ConstantNode *>(parse_result)) {
    left = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(parse_result));
  } else if (std::holds_alternative<IdentifierNode *>(parse_result)) {
    left = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(parse_result));
  } else {
    left = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(parse_result));
  }

  t = lexer->GetNextToken();
  RelExprOp op;
  switch (t->kind_) {
    case TokenType::Gt:
      op = RelExprOp::Gt;
      break;
    case TokenType::Gte:
      op = RelExprOp::Gte;
      break;
    case TokenType::Lt:
      op = RelExprOp::Lt;
      break;
    case TokenType::Lte:
      op = RelExprOp::Lte;
      break;
    case TokenType::Eq:
      op = RelExprOp::Eq;
      break;
    case TokenType::Neq:
      op = RelExprOp::Neq;
      break;
    default:
      throw ParseException(
          StringFormat("expected '>', '>=', '<', '<=', '==' or '!=' but got %s", t->value_),
          t->line_no_, t->col_no_);
  }

  parse_result = ParseExpression(lexer, state);
  RelExpressionNodeChild *right;
  if (std::holds_alternative<ConstantNode *>(parse_result)) {
    right = static_cast<RelExpressionNodeChild *>(std::get<ConstantNode *>(parse_result));
  } else if (std::holds_alternative<IdentifierNode *>(parse_result)) {
    right = static_cast<RelExpressionNodeChild *>(std::get<IdentifierNode *>(parse_result));
  } else {
    right = static_cast<RelExpressionNodeChild *>(std::get<ExpressionNode *>(parse_result));
  }

  return new RelExpressionNode(op, left, right, {start_line, start_col});
}
