#include "parse.h"
#include "string_utils.h"

PrintNode *ParsePrint(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Name || t->value_ != "print") {
    throw ParseException(StringFormat("expected 'print' but got '%s'", t->value_.c_str()),
                         t->line_no_, t->col_no_);
  }

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Name) {
    throw ParseException(StringFormat("expected variable but got '%s'", t->value_.c_str()),
                         t->line_no_, t->col_no_);
  }
  IdentifierNode *var = new IdentifierNode(t->value_, {t->line_no_, t->col_no_});

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Semicolon) {
    throw ParseException(StringFormat("expected ';' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  return new PrintNode(var, stmt_no, {start_line, start_col});
}
