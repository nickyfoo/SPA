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
  int lineNo;
  int colNo;

  Token(TokenType kind, std::string value, int lineNo, int colNo);
};

class Lexer {
 private:
  const char *source;
  int curLine;
  int curCol;
  const char *curCharPointer;

  void advanceCharPointer();
  const Token *constantOrUnknown();
  const Token *identifierOrKeyword();
  const Token *gtOrGte();
  const Token *ltOrLte();
  const Token *equalOrEq();
  const Token *notOrNeq();
  const Token *andOrUnknown();
  const Token *orOrUnknown();

 public:
  explicit Lexer(const char *source);
  const Token *getNextToken();
};

class BufferedLexer {
 private:
  Lexer *lexer;
  const Token *nextToken;

 public:
  explicit BufferedLexer(const char *source);
  const Token *getNextToken();
  const Token *peekNextToken();
};
