#include "lexer.h"

Token::Token(TokenType kind, std::string value, int lineNo, int colNo) {
  this->kind = kind;
  this->value = value;
  this->lineNo = lineNo;
  this->colNo = colNo;
}
