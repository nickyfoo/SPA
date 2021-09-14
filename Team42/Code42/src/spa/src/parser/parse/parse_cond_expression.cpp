#include <cassert>

#include "parse.h"
#include "string_utils.h"

std::string CondExprOpToString(CondExprOp op) {
  switch (op) {
    case CondExprOp::And:
      return "&&";
    case CondExprOp::Or:
      return "||";
    case CondExprOp::Not:
      return "!";
    default:
      throw std::invalid_argument("CondExprOpToString: invalid CondExprOp value");
  }
}

Node *ParseCondExpression(BufferedLexer *lexer, ParseState *state) {
  // include parsing "(" and ")": that always comes with condExpr
  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::LParen) {
    throw ParseException("expected left parenthesis", t->line_no_, t->col_no_);
  }

  t = lexer->PeekNextToken();

  if (t->kind_ == TokenType::Not) {
    // flush "!"
    lexer->GetNextToken();
    Node *left = ParseCondExpression(lexer, state);

    // this should be a runtime gurantee
    assert(left->get_kind() == NodeType::CondExpression ||
           left->get_kind() == NodeType::RelExpression);

    std::string expr_string;
    if (left->get_kind() == NodeType::RelExpression) {
      expr_string = dynamic_cast<RelExpressionNode *>(left)->get_expr_string();
    } else {
      expr_string = dynamic_cast<CondExpressionNode *>(left)->get_expr_string();
    }

    expr_string = StringFormat("%s %s", expr_string.c_str(), CondExprOpToString(CondExprOp::Not).c_str());

    t = lexer->GetNextToken();
    if (t->kind_ != TokenType::RParen) {
      throw ParseException("expected right parenthesis", t->line_no_, t->col_no_);
    }

    return new CondExpressionNode(CondExprOp::Not, left, nullptr, expr_string,
                                  LocInfo{.line_no = start_line, .col_no = start_col});
  }

  if (t->kind_ == TokenType::Constant || t->kind_ == TokenType::Identifier) {
    // must be rel expression as cond expression is surrounded by parentheses
    Node *rel = ParseRelExpression(lexer, state);

    t = lexer->GetNextToken();
    if (t->kind_ != TokenType::RParen) {
      throw ParseException("expected right parenthesis", t->line_no_, t->col_no_);
    }

    return rel;
  }

  if (t->kind_ != TokenType::LParen) {
    throw ParseException("expected Not, right parenthesis, Constant or Identifier", t->line_no_,
                         t->col_no_);
  }

  // only two possiblities left, cond expression with And or Or
  CondExprOp op;

  Node *left = ParseCondExpression(lexer, state);
  assert(left->get_kind() == NodeType::CondExpression ||
         left->get_kind() == NodeType::RelExpression);
  std::string left_expr_string;
  if (left->get_kind() == NodeType::RelExpression) {
    left_expr_string = dynamic_cast<RelExpressionNode *>(left)->get_expr_string();
  } else {
    left_expr_string = dynamic_cast<CondExpressionNode *>(left)->get_expr_string();
  }

  t = lexer->GetNextToken();
  if (t->kind_ == TokenType::And) {
    op = CondExprOp::And;
  } else if (t->kind_ == TokenType::Or) {
    op = CondExprOp::Or;
  } else {
    throw ParseException("expected And or Or", t->line_no_, t->col_no_);
  }

  Node *right = ParseCondExpression(lexer, state);
  assert(right->get_kind() == NodeType::CondExpression ||
         right->get_kind() == NodeType::RelExpression);
  std::string right_expr_string;
  if (right->get_kind() == NodeType::RelExpression) {
    right_expr_string = dynamic_cast<RelExpressionNode *>(right)->get_expr_string();
  } else {
    right_expr_string = dynamic_cast<CondExpressionNode *>(right)->get_expr_string();
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::RParen) {
    throw ParseException("expected right parenthesis", t->line_no_, t->col_no_);
  }

  std::string expr_string = StringFormat("%s %s %s", left_expr_string.c_str(),
                                         right_expr_string.c_str(), CondExprOpToString(op).c_str());

  return new CondExpressionNode(op, left, right, expr_string,
                                LocInfo{.line_no = start_line, .col_no = start_col});
}
