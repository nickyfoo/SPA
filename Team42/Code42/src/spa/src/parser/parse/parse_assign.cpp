
#include <parse.h>

AssignNode *ParseAssign(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Identifier) {
    throw ParseException("expected Identifier", t->line_no_, t->col_no_);
  }

  auto var = new IdentifierNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Equal) {
    throw ParseException("expected Equal", t->line_no_, t->col_no_);
  }

  Node *expr = ParseExpression(lexer, state);
  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException("expected Semicolon", t->line_no_, t->col_no_);
  }

  return new AssignNode(var, expr, stmt_no, LocInfo{.line_no = start_line, .col_no = start_col});
}
