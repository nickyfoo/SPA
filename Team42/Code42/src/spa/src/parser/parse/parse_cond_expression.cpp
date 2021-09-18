#include <cassert>
#include <variant>

#include "ast.h"
#include "ast_utils.hpp"
#include "parse.h"
#include "string_utils.h"

std::variant<CondExpressionNode *, RelExpressionNode *> ParseCondExpression(BufferedLexer *lexer,
                                                                            ParseState *state) {
  // include parsing "(" and ")": that always comes with condExpr
  auto *t = lexer->GetNextToken();
  auto start_line = t->line_no_;
  auto start_col = t->col_no_;

  if (t->kind_ != TokenType::LParen) {
    throw ParseException(StringFormat("expected '(' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  t = lexer->PeekNextToken();

  // handle RelExpressoin
  if (t->kind_ == TokenType::Number || t->kind_ == TokenType::Name) {
    RelExpressionNode *rel = ParseRelExpression(lexer, state);

    t = lexer->GetNextToken();
    if (t->kind_ != TokenType::RParen) {
      throw ParseException(StringFormat("expected ')' but got '%s'", t->value_.c_str()),
                           t->line_no_, t->col_no_);
    }

    return rel;
  }

  // hanlde Not operator
  if (t->kind_ == TokenType::Not) {
    // flush "!"
    lexer->GetNextToken();
    auto parse_result = ParseCondExpression(lexer, state);

    CondExpressionNodeChild *left;
    if (std::holds_alternative<CondExpressionNode *>(parse_result)) {
      left = static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(parse_result));
    } else {
      left = static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(parse_result));
    }

    t = lexer->GetNextToken();
    if (t->kind_ != TokenType::RParen) {
      throw ParseException(StringFormat("expected ')' but got '%s'", t->value_.c_str()),
                           t->line_no_, t->col_no_);
    }

    return new CondExpressionNode(CondExprOp::Not, left, nullptr, {start_line, start_col});
  }

  // handle And and Or
  if (t->kind_ != TokenType::LParen) {
    throw ParseException(
        StringFormat("expected '!', '(', variable or integer but got '%s'", t->value_.c_str()),
        t->line_no_, t->col_no_);
  }

  CondExpressionNodeChild *left, *right;
  CondExprOp op;

  auto parse_result = ParseCondExpression(lexer, state);
  if (std::holds_alternative<CondExpressionNode *>(parse_result)) {
    left = static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(parse_result));
  } else {
    left = static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(parse_result));
  }

  t = lexer->GetNextToken();
  if (t->kind_ == TokenType::And) {
    op = CondExprOp::And;
  } else if (t->kind_ == TokenType::Or) {
    op = CondExprOp::Or;
  } else {
    throw ParseException(StringFormat("expected '&&' or '||' but got '%s'", t->value_.c_str()),
                         t->line_no_, t->col_no_);
  }

  parse_result = ParseCondExpression(lexer, state);
  if (std::holds_alternative<CondExpressionNode *>(parse_result)) {
    right = static_cast<CondExpressionNodeChild *>(std::get<CondExpressionNode *>(parse_result));
  } else {
    right = static_cast<CondExpressionNodeChild *>(std::get<RelExpressionNode *>(parse_result));
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::RParen) {
    throw ParseException(StringFormat("expected ')' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  return new CondExpressionNode(op, left, right, {start_line, start_col});
}
