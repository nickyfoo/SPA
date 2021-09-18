#include <cctype>

#include "iostream"
#include "lexer.h"

Lexer::Lexer(const std::string &source) : source_(source) {
  this->ptr_ = 0;
  this->cur_line_ = 1;
  this->cur_col_ = 0;
}

char Lexer::GetCurChar() {
  if (this->ptr_ >= this->source_.size()) {
    throw std::runtime_error("Lexer: GetNextChar(): pointer at end of source");
  }

  return this->source_.at(this->ptr_);
}

void Lexer::AdvancePointer() {
  if (this->ptr_ >= this->source_.size()) {
    throw std::runtime_error("Lexer: GetNextChar(): pointer at end of source");
  }

  if (this->GetCurChar() == '\n') {
    cur_col_ = 0;
    cur_line_++;
  } else {
    cur_col_++;
  }

  this->ptr_++;
}

const Token *Lexer::GetNextToken() {
  if (this->ptr_ >= this->source_.size()) {
    return new Token(TokenType::End, "", cur_line_, cur_col_);
  }

  while (isspace(this->GetCurChar())) {
    AdvancePointer();
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
  if (!std::isdigit(this->GetCurChar())) {
    throw std::runtime_error("Lexer: NumberOrUnknown(): first char is not digit");
  }

  std::string value = "";
  int startCol = cur_col_;

  while (isdigit(this->GetCurChar())) {
    value += this->GetCurChar();
    this->AdvancePointer();
  }

  if (value.length() > 1 && value.at(0) == '0') {
    return new Token(TokenType::Unknown, value, cur_line_, startCol);
  }

  return new Token(TokenType::Number, value, cur_line_, startCol);
}

const Token *Lexer::Name() {
  if (!std::isalpha(this->GetCurChar())) {
    throw std::runtime_error("Lexer: Name(): first char is not letter");
  }

  std::string value = "";
  int startCol = cur_col_;

  while (std::isalpha(this->GetCurChar()) || std::isdigit(this->GetCurChar())) {
    value += this->GetCurChar();
    this->AdvancePointer();
  }

  return new Token(TokenType::Name, value, cur_line_, startCol);
}

const Token *Lexer::GtOrGte() {
  if (this->GetCurChar() != '>') {
    throw std::runtime_error("Lexer: GtOrGte(): first char is not '>'");
  }

  this->AdvancePointer();
  if (this->GetCurChar() == '=') {
    this->AdvancePointer();
    return new Token(TokenType::Gte, ">=", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Gt, ">", cur_line_, cur_col_ - 1);
}

const Token *Lexer::LtOrLte() {
  if (this->GetCurChar() != '<') {
    throw std::runtime_error("Lexer: LtorLte(): first char is not '<'");
  }

  this->AdvancePointer();
  if (this->GetCurChar() == '=') {
    AdvancePointer();
    return new Token(TokenType::Lte, "<=", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Lt, "<", cur_line_, cur_col_ - 1);
}

const Token *Lexer::EqualOrEq() {
  if (this->GetCurChar() != '=') {
    throw std::runtime_error("Lexer: EqualOrEq(): first char is not '='");
  }

  this->AdvancePointer();
  if (this->GetCurChar() == '=') {
    this->AdvancePointer();
    return new Token(TokenType::Eq, "==", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Equal, "=", cur_line_, cur_col_ - 1);
}

const Token *Lexer::NotOrNeq() {
  if (this->GetCurChar() != '!') {
    throw std::runtime_error("Lexer: NotOrNeq(): first char is not '!'");
  }

  this->AdvancePointer();

  if (this->GetCurChar() == '=') {
    this->AdvancePointer();
    return new Token(TokenType::Neq, "!=", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Not, "!", cur_line_, cur_col_ - 1);
}

const Token *Lexer::AndOrUnknown() {
  if (this->GetCurChar() != '&') {
    throw std::runtime_error("Lexer: AndOrUnknown(): first char is not '&'");
  }

  this->AdvancePointer();
  if (this->GetCurChar() == '&') {
    this->AdvancePointer();
    return new Token(TokenType::And, "&&", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Unknown, "&", cur_line_, cur_col_ - 1);
}

const Token *Lexer::OrOrUnknown() {
  if (this->GetCurChar() != '|') {
    throw std::runtime_error("Lexer: OrOrUnknown(): first char is not '|'");
  }

  this->AdvancePointer();
  if (this->GetCurChar() == '|') {
    this->AdvancePointer();
    return new Token(TokenType::Or, "||", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Unknown, "|", cur_line_, cur_col_ - 1);
}
