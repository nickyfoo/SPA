#include "parse.h"

PrintNode *ParsePrint(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name || t->value_ != "print") {
    throw ParseException("expected 'print' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Name) {
    throw ParseException("expected variable but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }
  IdentifierNode *var = new IdentifierNode(t->value_, {t->line_no_, t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException("expected ';' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  return new PrintNode(var, stmt_no, {start_line, start_col});
}
