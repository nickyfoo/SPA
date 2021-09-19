#include "parse.h"

CallNode *ParseCall(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name || t->value_ != "call") {
    throw ParseException("expected 'call' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Name) {
    throw ParseException("expected procedure but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }
  auto var = new IdentifierNode(t->value_, {t->line_no_, t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException("expected ';' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  return new CallNode(var, stmt_no, {start_line, start_col});
}
