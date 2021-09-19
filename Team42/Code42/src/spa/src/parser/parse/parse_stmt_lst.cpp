#include <iostream>
#include "parse.h"
#include "string_utils.h"

std::vector<StatementNode *> ParseStmtLst(BufferedLexer *lexer, ParseState *state) {
  // include parsing of "{" and "}" that always surrounds a stmtLst
  const Token *t = lexer->GetNextToken();

  if (t->kind_ != TokenType::LBrace) {
    throw ParseException(StringFormat("expected '{' but got '%s'", t->value_.c_str()), t->line_no_,
                         t->col_no_);
  }

  std::vector<StatementNode *> stmt_lst{};
  t = lexer->PeekNextToken();

  while (t->kind_ != TokenType::RBrace) {
    if (t->kind_ != TokenType::Name) {
      throw ParseException(StringFormat("expected statement but got '%s'", t->value_), t->line_no_,
                           t->col_no_);
    }

    if (t->value_ == "read") {
      stmt_lst.push_back(ParseRead(lexer, state));
      t = lexer->PeekNextToken();
      continue;
    }

    if (t->value_ == "print") {
      stmt_lst.push_back(ParsePrint(lexer, state));
      t = lexer->PeekNextToken();
      continue;
    }

    if (t->value_ == "call") {
      stmt_lst.push_back(ParseCall(lexer, state));
      t = lexer->PeekNextToken();
      continue;
    }

    if (t->value_ == "while") {
      stmt_lst.push_back(ParseWhile(lexer, state));
      t = lexer->PeekNextToken();
      continue;
    }

    if (t->value_ == "if") {
      stmt_lst.push_back(ParseIf(lexer, state));
      t = lexer->PeekNextToken();
      continue;
    }

    // must be assign
    stmt_lst.push_back(ParseAssign(lexer, state));
    t = lexer->PeekNextToken();
  }

  // flush the "}"
  // we already verified it is a '}' in the while loop above
  t = lexer->GetNextToken();

  return stmt_lst;
}
