#include "parse.h"

std::vector<StatementNode *> ParseStmtLst(BufferedLexer *lexer, ParseState *state) {
  // include parsing of "{" and "}" that always surrounds a stmtLst
  const Token *t = lexer->GetNextToken();

  if (t->kind_ != TokenType::LBrace) {
    throw ParseException("expected left brace", t->line_no_, t->col_no_);
  }

  std::vector<StatementNode *> stmt_lst{};
  t = lexer->PeekNextToken();

  while (t->kind_ != TokenType::RBrace) {
    switch (t->kind_) {
      case TokenType::Read:stmt_lst.push_back(ParseRead(lexer, state));
        break;
      case TokenType::Print:stmt_lst.push_back(ParsePrint(lexer, state));
        break;
      case TokenType::Call:stmt_lst.push_back(ParseCall(lexer, state));
        break;
      case TokenType::While:stmt_lst.push_back(ParseWhile(lexer, state));
        break;
      case TokenType::If:stmt_lst.push_back(ParseIf(lexer, state));
        break;
      case TokenType::Identifier:stmt_lst.push_back(ParseAssign(lexer, state));
        break;
      default:throw ParseException("expected Statement", t->line_no_, t->col_no_);
    }

    t = lexer->PeekNextToken();
  }

  // flush the "}"
  t = lexer->GetNextToken();

  return stmt_lst;
}

