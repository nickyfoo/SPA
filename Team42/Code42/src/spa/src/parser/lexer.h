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
  TokenType kind;
  std::string value;
  int line_no;
  int col_no;

  Token(TokenType kind, std::string value, int line_no, int col_no);
};

class Lexer {
 private:
  const char *source;
  int cur_line;
  int cur_col;
  const char *cur_char_pointer;

  void AdvanceCharPointer();
  const Token *ConstantOrUnknown();
  const Token *IdentifierOrKeyword();
  const Token *GtOrGte();
  const Token *LtOrLte();
  const Token *EqualOrEq();
  const Token *NotOrNeq();
  const Token *AndOrUnknown();
  const Token *OrOrUnknown();

 public:
  explicit Lexer(const char *source);
  const Token *GetNextToken();
};

class BufferedLexer {
 private:
  Lexer *lexer;
  const Token *next_token;

 public:
  explicit BufferedLexer(const char *source);
  const Token *GetNextToken();
  const Token *PeekNextToken();
};
