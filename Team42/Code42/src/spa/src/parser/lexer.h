#pragma once

#include <string>

namespace lexer {

enum Kind {
  Identifier,
  Constant,
  LBrace,
  RBrace,
  LParen,
  RParen,
  Plus,
  Minus,
  Divide,
  Multiply,
  Mod,
  Gt,
  Gte,
  Lt,
  Lte,
  Eq,
  Neq,
  Not,
  And,
  Or,
  Read,
  Print,
  Call,
  While,
  If,
  Assign,
  End,
  Unexpected
};

struct Token {
  Kind type;
  std::string value;
};

class Lexer {
private:
  std::string m_source;

public:
  Lexer(std::string source);
  Token getNextToken();
};

} // namespace lexer
