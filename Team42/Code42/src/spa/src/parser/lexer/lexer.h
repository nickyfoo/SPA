#pragma once

#include <string>

enum class TokenType {
  End,
  Unknown,
  Number,
  Name,
  LParen,
  RParen,
  LBrace,
  RBrace,
  Semicolon,
  Plus,
  Minus,
  Divide,
  Multiply,
  Modulo,
  Gt,
  Gte,
  Lt,
  Lte,
  Equal,
  Eq,
  Not,
  Neq,
  And,
  Or
};

class Token {
 public:
  TokenType kind_;
  std::string value_;
  int line_no_;
  int col_no_;

  Token(TokenType kind, std::string value, int line_no, int col_no);
};

class Lexer {
 public:
  Lexer(const std::string &source);
  const Token *GetNextToken();

 private:
  const std::string &source_;
  size_t ptr_;
  int cur_line_;
  int cur_col_;

  void AdvancePointer();
  bool PointerAtEnd();
  char GetCurChar();
  const Token *NumberOrUnknown();
  const Token *Name();
  const Token *GtOrGte();
  const Token *LtOrLte();
  const Token *EqualOrEq();
  const Token *NotOrNeq();
  const Token *AndOrUnknown();
  const Token *OrOrUnknown();
};

class BufferedLexer {
 public:
  explicit BufferedLexer(const std::string &source);
  const Token *GetNextToken();
  const Token *PeekNextToken();

 private:
  Lexer *lexer_;
  const Token *next_token_;
};

