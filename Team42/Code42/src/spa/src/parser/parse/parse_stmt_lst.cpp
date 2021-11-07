#include <iostream>
#include "parse.h"

std::vector<StatementNode *> ParseStmtLst(BufferedLexer *lexer, ParseState *state) {
  // include parsing of "{" and "}" that always surrounds a stmtLst
  const Token *t = lexer->GetNextToken();

  if (t->kind_ != TokenType::LBrace) {
    throw ParseException("expected '{' but got '" + t->value_ + "'", t->line_no_, t->col_no_);
  }

  std::vector<StatementNode *> stmt_lst{};
  t = lexer->PeekNextToken();

  while (t->kind_ != TokenType::RBrace) {
    if (t->kind_ != TokenType::Name) {
      throw ParseException("expected statement but got '" + t->value_ + "'", t->line_no_, t->col_no_);
    }

    if (t->value_ == "read") {
      if (lexer->PeekNextToken(1)->kind_ == TokenType::Equal) {
        stmt_lst.push_back(ParseAssign(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      if (lexer->PeekNextToken(1)->kind_ == TokenType::Name) {
        stmt_lst.push_back(ParseRead(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      throw ParseException("invalid read or assign statement", t->line_no_, t->col_no_);
    }

    if (t->value_ == "print") {
      if (lexer->PeekNextToken(1)->kind_ == TokenType::Equal) {
        stmt_lst.push_back(ParseAssign(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      if (lexer->PeekNextToken(1)->kind_ == TokenType::Name) {
        stmt_lst.push_back(ParsePrint(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      throw ParseException("invalid print or assign statement", t->line_no_, t->col_no_);
    }

    if (t->value_ == "call") {
      if (lexer->PeekNextToken(1)->kind_ == TokenType::Equal) {
        stmt_lst.push_back(ParseAssign(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      if (lexer->PeekNextToken(1)->kind_ == TokenType::Name) {
        stmt_lst.push_back(ParseCall(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      throw ParseException("invalid call or assign statement", t->line_no_, t->col_no_);
    }

    if (t->value_ == "while") {
      if (lexer->PeekNextToken(1)->kind_ == TokenType::Equal) {
        stmt_lst.push_back(ParseAssign(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      if (lexer->PeekNextToken(1)->kind_ == TokenType::LParen) {
        stmt_lst.push_back(ParseWhile(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      throw ParseException("invalid while or assign statement", t->line_no_, t->col_no_);
    }

    if (t->value_ == "if") {
      if (lexer->PeekNextToken(1)->kind_ == TokenType::Equal) {
        stmt_lst.push_back(ParseAssign(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      if (lexer->PeekNextToken(1)->kind_ == TokenType::LParen) {
        stmt_lst.push_back(ParseIf(lexer, state));
        t = lexer->PeekNextToken();
        continue;
      }

      throw ParseException("invalid if or assign statement", t->line_no_, t->col_no_);
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
