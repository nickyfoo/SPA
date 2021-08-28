#pragma once

#include <string>

namespace lexer {

enum class Kind {
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
  Kind kind;
  std::string value;
  int lineNo;
  int colNo;

  Token(Kind _kind, std::string _value, int _lineNo, int _colNo)
      : kind(_kind), value(_value), lineNo(_lineNo), colNo(_colNo){};
};

class Lexer {
private:
  const char *source;

  int lineNo;
  int colNo;
  int idx;

  Token *constantOrUnknown();
  Token *identifierOrKeyword();

  char get();
  void next();

  Token *gtOrGte();
  Token *ltOrLte();
  Token *equalOrEq();
  Token *notOrNeq();
  Token *andOrUnknown();
  Token *orOrUnknown();

public:
  Lexer(const char *_source) : source(_source), lineNo(1), colNo(0), idx(0){};
  Token *getNextToken();
};

class BufferedLexer {
private:
  Lexer *lexer;
  Token *next = nullptr;

public:
  BufferedLexer(Lexer *_lexer) : lexer(_lexer){};
  Token *getNextToken();
  Token *peekNextToken();
};

} // namespace lexer
