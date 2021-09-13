#include <parse.h>

RelExpressionNode *ParseRelExpression(BufferedLexer *lexer, ParseState *state) {
  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  // for rel expressions, left and right should be constants or identifiers only
  Node *left;
  if (t->kind_ == TokenType::Constant) {
    left = new ConstantNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
  } else if (t->kind_ == TokenType::Identifier) {
    left = new IdentifierNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
  } else {
    throw ParseException("expected Constant or Identifier", t->line_no_, t->col_no_);
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
      throw ParseException("expected RelExpression operator", t->line_no_, t->col_no_);
  }

  t = lexer->GetNextToken();
  Node *right;
  if (t->kind_ == TokenType::Constant) {
    right = new ConstantNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
  } else if (t->kind_ == TokenType::Identifier) {
    right = new IdentifierNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});
  } else {
    throw ParseException("expected Constant or Identifier", t->line_no_, t->col_no_);
  }

  return new RelExpressionNode(op, left, right,
                               LocInfo{.line_no = start_line, .col_no = start_col});
}
