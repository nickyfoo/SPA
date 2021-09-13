#include "lexer.h"

Token::Token(TokenType kind, std::string value, int lineNo, int colNo) {
  this->kind_ = kind;
  this->value_ = value;
  this->line_no_ = lineNo;
  this->col_no_ = colNo;
}

