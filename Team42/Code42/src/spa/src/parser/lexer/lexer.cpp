#include "lexer.h"

Lexer::Lexer(const char *source) {
  this->source_ = source;
  this->cur_line_ = 1;
  this->cur_col_ = 0;
  this->cur_char_pointer_ = source;
}

void Lexer::AdvanceCharPointer() {
  if (cur_char_pointer_[0] == '\n') {
    cur_col_ = 0;
    cur_line_++;
  } else {
    cur_col_++;
  }

  cur_char_pointer_++;
}

const Token *Lexer::GetNextToken() {
  // idea behind this implementation is to peek at the next character,
  // and greedily evaluate the longest matching token type
  while (isspace(cur_char_pointer_[0])) {
    AdvanceCharPointer();
  }

  if (isdigit(cur_char_pointer_[0])) {
    return ConstantOrUnknown();
  }

  if (isalpha(cur_char_pointer_[0])) {
    return IdentifierOrKeyword();
  }

  switch (cur_char_pointer_[0]) {
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

  const Token *res = nullptr;
  switch (cur_char_pointer_[0]) {
    case '\0':
      res = new Token(TokenType::End, "", cur_line_, cur_col_);
      break;
    case ';':
      res = new Token(TokenType::Semicolon, "", cur_line_, cur_col_);
      break;
    case '(':
      res = new Token(TokenType::LParen, "", cur_line_, cur_col_);
      break;
    case ')':
      res = new Token(TokenType::RParen, "", cur_line_, cur_col_);
      break;
    case '{':
      res = new Token(TokenType::LBrace, "", cur_line_, cur_col_);
      break;
    case '}':
      res = new Token(TokenType::RBrace, "", cur_line_, cur_col_);
      break;
    case '+':
      res = new Token(TokenType::Plus, "", cur_line_, cur_col_);
      break;
    case '-':
      res = new Token(TokenType::Minus, "", cur_line_, cur_col_);
      break;
    case '/':
      res = new Token(TokenType::Divide, "", cur_line_, cur_col_);
      break;
    case '*':
      res = new Token(TokenType::Multiply, "", cur_line_, cur_col_);
      break;
    case '%':
      res = new Token(TokenType::Modulo, "", cur_line_, cur_col_);
      break;
    default:
      res =
          new Token(TokenType::Unknown, std::string(1, cur_char_pointer_[0]), cur_line_, cur_col_);
  }
  AdvanceCharPointer();
  return res;
}

const Token *Lexer::ConstantOrUnknown() {
  std::string value = "";
  int startCol = cur_col_;

  while (isdigit(cur_char_pointer_[0])) {
    value += cur_char_pointer_[0];
    AdvanceCharPointer();
  }

  if (value.length() > 1 && value[0] == '0') {
    return new Token(TokenType::Unknown, value, cur_line_, startCol);
  }

  return new Token(TokenType::Constant, value, cur_line_, startCol);
}

const Token *Lexer::IdentifierOrKeyword() {
  std::string value = "";
  int startCol = cur_col_;

  while (isalpha(cur_char_pointer_[0]) || isdigit(cur_char_pointer_[0])) {
    value += cur_char_pointer_[0];
    AdvanceCharPointer();
  }

  if (value == "if") {
    return new Token(TokenType::If, "", cur_line_, startCol);
  }
  if (value == "then") {
    return new Token(TokenType::Then, "", cur_line_, startCol);
  }
  if (value == "else") {
    return new Token(TokenType::Else, "", cur_line_, startCol);
  }
  if (value == "while") {
    return new Token(TokenType::While, "", cur_line_, startCol);
  }
  if (value == "read") {
    return new Token(TokenType::Read, "", cur_line_, startCol);
  }
  if (value == "print") {
    return new Token(TokenType::Print, "", cur_line_, startCol);
  }
  if (value == "call") {
    return new Token(TokenType::Call, "", cur_line_, startCol);
  }
  if (value == "procedure") {
    return new Token(TokenType::Procedure, "", cur_line_, startCol);
  }

  return new Token(TokenType::Identifier, value, cur_line_, startCol);
}

const Token *Lexer::GtOrGte() {
  // should not be called if the current char is not '>'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer_[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Gte, "", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Gt, "", cur_line_, cur_col_ - 1);
}

const Token *Lexer::LtOrLte() {
  // should not be called if the current char is not '<'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer_[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Lte, "", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Lt, "", cur_line_, cur_col_ - 1);
}

const Token *Lexer::EqualOrEq() {
  // should not be called if the current char is not '='
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer_[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Eq, "", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Equal, "", cur_line_, cur_col_ - 1);
}

const Token *Lexer::NotOrNeq() {
  // should not be called if the current char is not '!'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer_[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Neq, "", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Not, "", cur_line_, cur_col_ - 1);
}

const Token *Lexer::AndOrUnknown() {
  // should not be called if the current char is not '&'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer_[0] == '&') {
    AdvanceCharPointer();
    return new Token(TokenType::And, "", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Unknown, "&", cur_line_, cur_col_ - 1);
}

const Token *Lexer::OrOrUnknown() {
  // should not be called if the current char is not '|'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer_[0] == '|') {
    AdvanceCharPointer();
    return new Token(TokenType::Or, "", cur_line_, cur_col_ - 2);
  }

  return new Token(TokenType::Unknown, "|", cur_line_, cur_col_ - 1);
}

