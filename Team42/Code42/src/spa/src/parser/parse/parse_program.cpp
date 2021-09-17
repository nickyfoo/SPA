#include "parse.h"
#include "iostream"

ProgramNode *ParseProgram(BufferedLexer *lexer, ParseState *state) {
  const Token *t = lexer->PeekNextToken();
  int start_line = t->line_no_;
  int start_col = t->col_no_;

  std::vector<ProcedureNode *> procedures{};
  while (t->kind_ != TokenType::End) {
    if (t->kind_ != TokenType::Procedure) {
      throw ParseException("expected Procedure", t->line_no_, t->col_no_);
    }

    ProcedureNode *p = ParseProcedure(lexer, state);
    procedures.push_back(p);

    t = lexer->PeekNextToken();
  }

  return new ProgramNode(procedures, {start_line, start_col});
}

