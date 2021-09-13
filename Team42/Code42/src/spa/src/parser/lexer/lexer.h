#pragma once

#include <string>

enum class TokenType {
  End,
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
  Or,
  If,
  Then,
  Else,
  While,
  Read,
  Print,
  Call,
  Procedure,
  Identifier,
  Constant,
  Unknown
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
  explicit Lexer(const char *source);
  const Token *GetNextToken();

 private:
  const char *source_;
  int cur_line_;
  int cur_col_;
  const char *cur_char_pointer_;

  void AdvanceCharPointer();
  const Token *ConstantOrUnknown();
  const Token *IdentifierOrKeyword();
  const Token *GtOrGte();
  const Token *LtOrLte();
  const Token *EqualOrEq();
  const Token *NotOrNeq();
  const Token *AndOrUnknown();
  const Token *OrOrUnknown();
};

class BufferedLexer {
 public:
  explicit BufferedLexer(const char *source);
  const Token *GetNextToken();
  const Token *PeekNextToken();

 private:
  Lexer *lexer_;
  const Token *next_token_;
};

