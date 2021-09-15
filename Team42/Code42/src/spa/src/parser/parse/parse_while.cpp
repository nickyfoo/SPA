#include "parse.h"

WhileNode *ParseWhile(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::While) {
    throw ParseException("expected While", t->line_no_, t->col_no_);
  }

  Node *cond = ParseCondExpression(lexer, state);
  std::vector<StatementNode *> stmt_lst = ParseStmtLst(lexer, state);

  return new WhileNode(cond, stmt_lst, stmt_no,
                       {start_line, start_col});
}

