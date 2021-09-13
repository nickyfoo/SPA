#include <parse.h>

PrintNode *ParsePrint(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Print) {
    throw ParseException("expected Print", t->line_no_, t->col_no_);
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Identifier) {
    throw ParseException("expected Identifier", t->line_no_, t->col_no_);
  }
  IdentifierNode *var =
      new IdentifierNode(t->value_, LocInfo{.line_no = t->line_no_, .col_no = t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException("expected semicolon", t->line_no_, t->col_no_);
  }

  return new PrintNode(var, stmt_no, LocInfo{.line_no = start_line, .col_no = start_col});
}
