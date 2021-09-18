#include <cassert>
#include <cctype>
#include <iostream>

#include "lexer.h"

Lexer::Lexer(const std::string &source) : source_(source) {
  this->ptr_ = 0;
  this->cur_line_ = 1;
  this->cur_col_ = 0;
}

void Lexer::AdvancePointer() {
  if (this->PointerAtEnd()) {
    return;
  }

  if (this->GetCurChar() == '\n') {
    cur_col_ = 0;
    cur_line_++;
  } else {
    cur_col_++;
  }

  this->ptr_++;
}

bool Lexer::PointerAtEnd() { return this->ptr_ >= this->source_.size(); }

char Lexer::GetCurChar() {
  if (this->PointerAtEnd()) {
    return '\0';
  }

  return this->source_.at(this->ptr_);
}

const Token *Lexer::GetNextToken() {
  while (!this->PointerAtEnd() && isspace(this->GetCurChar())) {
    AdvancePointer();
  }

  if (this->PointerAtEnd()) {
    return new Token(TokenType::End, "", cur_line_, cur_col_);
  }

  if (isdigit(this->GetCurChar())) {
    return NumberOrUnknown();
  }

  if (isalpha(this->GetCurChar())) {
    return Name();
  }

  switch (this->GetCurChar()) {
    case '>':
      return GtOrGte();
    case '<':
      return LtOrLte();
    case '=':
      return EqualOrEq();
    case '!':
      return NotOrNeq();
    case '&':
      return AndOrUnknown();
    case '|':
      return OrOrUnknown();
  }

  switch (this->GetCurChar()) {
    case ';':
      this->AdvancePointer();
      return new Token(TokenType::Semicolon, "", cur_line_, cur_col_);
    case '(':
      this->AdvancePointer();
      return new Token(TokenType::LParen, "", cur_line_, cur_col_);
    case ')':
      this->AdvancePointer();
      return new Token(TokenType::RParen, "", cur_line_, cur_col_);
    case '{':
      this->AdvancePointer();
      return new Token(TokenType::LBrace, "", cur_line_, cur_col_);
    case '}':
      this->AdvancePointer();
      return new Token(TokenType::RBrace, "", cur_line_, cur_col_);
    case '+':
      this->AdvancePointer();
      return new Token(TokenType::Plus, "", cur_line_, cur_col_);
    case '-':
      this->AdvancePointer();
      return new Token(TokenType::Minus, "", cur_line_, cur_col_);
    case '/':
      this->AdvancePointer();
      return new Token(TokenType::Divide, "", cur_line_, cur_col_);
    case '*':
      this->AdvancePointer();
      return new Token(TokenType::Multiply, "", cur_line_, cur_col_);
    case '%':
      this->AdvancePointer();
      return new Token(TokenType::Modulo, "", cur_line_, cur_col_);
    default:
      std::string value = std::string(1, this->GetCurChar());
      this->AdvancePointer();
      return new Token(TokenType::Unknown, value, cur_line_, cur_col_);
  }
}

const Token *Lexer::NumberOrUnknown() {
  assert(std::isdigit(this->GetCurChar()));

  std::string value = "";
  int startCol = cur_col_;

  while (!this->PointerAtEnd() && isdigit(this->GetCurChar())) {
    value += this->GetCurChar();
    this->AdvancePointer();
  }

  if (value.length() > 1 && value.at(0) == '0') {
    return new Token(TokenType::Unknown, value, cur_line_, startCol);
  }

  return new Token(TokenType::Number, value, cur_line_, startCol);
}

const Token *Lexer::Name() {
  assert(std::isalpha(this->GetCurChar()));

  std::string value = "";
  int startCol = cur_col_;

  while (!this->PointerAtEnd() &&
         (std::isalpha(this->GetCurChar()) || std::isdigit(this->GetCurChar()))) {
    value += this->GetCurChar();
    this->AdvancePointer();
  }

  return new Token(TokenType::Name, value, cur_line_, startCol);
}

const Token *Lexer::GtOrGte() {
  assert(this->GetCurChar() == '>');

  this->AdvancePointer();
  if (this->PointerAtEnd() || this->GetCurChar() != '=') {
    return new Token(TokenType::Gt, ">", cur_line_, cur_col_ - 1);
  }

  this->AdvancePointer();
  return new Token(TokenType::Gte, ">=", cur_line_, cur_col_ - 2);
}

const Token *Lexer::LtOrLte() {
  assert(this->GetCurChar() == '<');

  this->AdvancePointer();
  if (this->PointerAtEnd() || this->GetCurChar() != '=') {
    return new Token(TokenType::Lt, "<", cur_line_, cur_col_ - 1);
  }

  this->AdvancePointer();
  return new Token(TokenType::Lte, "<=", cur_line_, cur_col_ - 2);
}

const Token *Lexer::EqualOrEq() {
  assert(this->GetCurChar() == '=');

  this->AdvancePointer();
  if (this->PointerAtEnd() || this->GetCurChar() != '=') {
    return new Token(TokenType::Equal, "=", cur_line_, cur_col_ - 1);
  }

  this->AdvancePointer();
  return new Token(TokenType::Eq, "==", cur_line_, cur_col_ - 2);
}

const Token *Lexer::NotOrNeq() {
  assert(this->GetCurChar() == '!');

  this->AdvancePointer();
  if (this->PointerAtEnd() || this->GetCurChar() != '=') {
    return new Token(TokenType::Not, "!", cur_line_, cur_col_ - 1);
  }

  this->AdvancePointer();
  return new Token(TokenType::Neq, "!=", cur_line_, cur_col_ - 2);
}

const Token *Lexer::AndOrUnknown() {
  assert(this->GetCurChar() == '&');

  this->AdvancePointer();
  if (this->PointerAtEnd() || this->GetCurChar() != '&') {
    return new Token(TokenType::Unknown, "&", cur_line_, cur_col_ - 1);
  }

  this->AdvancePointer();
  return new Token(TokenType::And, "&&", cur_line_, cur_col_ - 2);
}

const Token *Lexer::OrOrUnknown() {
  assert(this->GetCurChar() == '|');

  this->AdvancePointer();
  if (this->PointerAtEnd() || this->GetCurChar() != '|') {
    return new Token(TokenType::Unknown, "|", cur_line_, cur_col_ - 1);
  }

  this->AdvancePointer();
  return new Token(TokenType::Or, "||", cur_line_, cur_col_ - 2);
}
