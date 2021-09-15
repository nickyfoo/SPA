#include "parse.h"

#include <iostream>

ProcedureNode *ParseProcedure(BufferedLexer *lexer, ParseState *state) {
  const Token *t = lexer->GetNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  if (t->kind_ != TokenType::Procedure) {
    throw ParseException("expected Procedure", t->line_no_, t->col_no_);
  }

  // procedure name
  t = lexer->GetNextToken();
  if (t->kind_ != TokenType::Identifier) {
    throw ParseException("expected Identifier", t->line_no_, t->col_no_);
  }
  std::string proc_name = t->value_;

  // statements
  std::vector<StatementNode *> stmt_lst = ParseStmtLst(lexer, state);

  return new ProcedureNode(proc_name, stmt_lst,
                           {start_line, start_col});
}
