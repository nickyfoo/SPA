#include "lexer.h"

Token::Token(TokenType kind, std::string value, int lineNo, int colNo) {
  this->kind = kind;
  this->value = value;
  this->line_no = lineNo;
  this->col_no = colNo;
}
