#include <parse.h>

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

    t = lexer->GetNextToken();
    if (t->kind_ != TokenType::RParen) {
      throw ParseException("expected right parenthesis", t->line_no_, t->col_no_);
    }

    return new CondExpressionNode(CondExprOp::Not, left, nullptr,
                                  LocInfo{.line_no = start_line, .col_no = start_col});
  }

  if (t->kind_ == TokenType::Constant || t->kind_ == TokenType::Identifier) {
    // rel expression, cond expressoin must be surrounded by parentheses
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

  // only two possiblities left is cond expression with And or Or
  Node *left = ParseCondExpression(lexer, state);
  CondExprOp op;

  t = lexer->GetNextToken();
  if (t->kind_ == TokenType::And) {
    op = CondExprOp::And;
  } else if (t->kind_ == TokenType::Or) {
    op = CondExprOp::Or;
  } else {
    throw ParseException("expected And or Or", t->line_no_, t->col_no_);
  }

  Node *right = ParseCondExpression(lexer, state);

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::RParen) {
    throw ParseException("expected right parenthesis", t->line_no_, t->col_no_);
  }

  return new CondExpressionNode(op, left, right, LocInfo{.line_no=start_line, .col_no=start_col});
}
