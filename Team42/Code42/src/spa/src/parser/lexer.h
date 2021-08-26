#pragma once

#include <string>

enum TokenType {
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
  Assign
};

struct Token {
  TokenType type;
  std::string value;
};

class Lexer {
public:
  Lexer(std::string source);
  Token getNextToken();
};
