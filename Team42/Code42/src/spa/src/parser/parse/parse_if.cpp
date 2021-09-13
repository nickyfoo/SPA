#include <parse.h>

IfNode *ParseIf(BufferedLexer *lexer, ParseState *state) {
  int stmt_no = ++(state->stmt_count_);

  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::If) {
    throw ParseException("expected If", t->line_no_, t->col_no_);
  }

  Node *cond = ParseCondExpression(lexer, state);

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Then) {
    throw ParseException("expected Then", t->line_no_, t->col_no_);
  }

  std::vector<StatementNode *> then_stmt_lst = ParseStmtLst(lexer, state);

  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Else) {
    throw ParseException("expected Else", t->line_no_, t->col_no_);
  }

  std::vector<StatementNode *> else_stmt_lst = ParseStmtLst(lexer, state);

  return new IfNode(cond, then_stmt_lst, else_stmt_lst, stmt_no,
                    LocInfo{.line_no = start_line, .col_no = start_col});
}
