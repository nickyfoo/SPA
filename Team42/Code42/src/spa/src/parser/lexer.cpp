#include "ctype.h"
#include "lexer.h"

Lexer::Lexer(const char *source) {
  this->source = source;
  this->cur_line = 1;
  this->cur_col = 0;
  this->cur_char_pointer = source;
}

void Lexer::AdvanceCharPointer() {
  if (cur_char_pointer[0] == '\n') {
    cur_col = 0;
    cur_line++;
  } else {
    cur_col++;
  }

  cur_char_pointer++;
}

const Token *Lexer::GetNextToken() {
  // idea behind this implementation is to peek at the next character,
  // and greedily evaluate the longest matching token type
  while (isspace(cur_char_pointer[0])) {
    AdvanceCharPointer();
  }

  if (isdigit(cur_char_pointer[0])) {
    return ConstantOrUnknown();
  }

  if (isalpha(cur_char_pointer[0])) {
    return IdentifierOrKeyword();
  }

  switch (cur_char_pointer[0]) {
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
  switch (cur_char_pointer[0]) {
    case '\0':
      res = new Token(TokenType::End, "", cur_line, cur_col);
      break;
    case ';':
      res = new Token(TokenType::Semicolon, "", cur_line, cur_col);
      break;
    case '(':
      res = new Token(TokenType::LParen, "", cur_line, cur_col);
      break;
    case ')':
      res = new Token(TokenType::RParen, "", cur_line, cur_col);
      break;
    case '{':
      res = new Token(TokenType::LBrace, "", cur_line, cur_col);
      break;
    case '}':
      res = new Token(TokenType::RBrace, "", cur_line, cur_col);
      break;
    case '+':
      res = new Token(TokenType::Plus, "", cur_line, cur_col);
      break;
    case '-':
      res = new Token(TokenType::Minus, "", cur_line, cur_col);
      break;
    case '/':
      res = new Token(TokenType::Divide, "", cur_line, cur_col);
      break;
    case '*':
      res = new Token(TokenType::Multiply, "", cur_line, cur_col);
      break;
    case '%':
      res = new Token(TokenType::Modulo, "", cur_line, cur_col);
      break;
    default:
      res = new Token(TokenType::Unknown, std::string(1, cur_char_pointer[0]),
                      cur_line, cur_col);
  }
  AdvanceCharPointer();
  return res;
}

const Token *Lexer::ConstantOrUnknown() {
  std::string value = "";
  int startCol = cur_col;

  while (isdigit(cur_char_pointer[0])) {
    value += cur_char_pointer[0];
    AdvanceCharPointer();
  }

  if (value.length() > 1 && value[0] == '0') {
    return new Token(TokenType::Unknown, value, cur_line, startCol);
  }

  return new Token(TokenType::Constant, value, cur_line, startCol);
}

const Token *Lexer::IdentifierOrKeyword() {
  std::string value = "";
  int startCol = cur_col;

  while (isalpha(cur_char_pointer[0]) || isdigit(cur_char_pointer[0])) {
    value += cur_char_pointer[0];
    AdvanceCharPointer();
  }

  if (value == "if") {
    return new Token(TokenType::If, "", cur_line, startCol);
  }
  if (value == "then") {
    return new Token(TokenType::Then, "", cur_line, startCol);
  }
  if (value == "else") {
    return new Token(TokenType::Else, "", cur_line, startCol);
  }
  if (value == "while") {
    return new Token(TokenType::While, "", cur_line, startCol);
  }
  if (value == "read") {
    return new Token(TokenType::Read, "", cur_line, startCol);
  }
  if (value == "print") {
    return new Token(TokenType::Print, "", cur_line, startCol);
  }
  if (value == "call") {
    return new Token(TokenType::Call, "", cur_line, startCol);
  }
  if (value == "procedure") {
    return new Token(TokenType::Procedure, "", cur_line, startCol);
  }

  return new Token(TokenType::Identifier, value, cur_line, startCol);
}

const Token *Lexer::GtOrGte() {
  // should not be called if the current char is not '>'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Gte, "", cur_line, cur_col - 2);
  }

  return new Token(TokenType::Gt, "", cur_line, cur_col - 1);
}

const Token *Lexer::LtOrLte() {
  // should not be called if the current char is not '<'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Lte, "", cur_line, cur_col - 2);
  }

  return new Token(TokenType::Lt, "", cur_line, cur_col - 1);
}

const Token *Lexer::EqualOrEq() {
  // should not be called if the current char is not '='
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Eq, "", cur_line, cur_col - 2);
  }

  return new Token(TokenType::Equal, "", cur_line, cur_col - 1);
}

const Token *Lexer::NotOrNeq() {
  // should not be called if the current char is not '!'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer[0] == '=') {
    AdvanceCharPointer();
    return new Token(TokenType::Neq, "", cur_line, cur_col - 2);
  }

  return new Token(TokenType::Not, "", cur_line, cur_col - 1);
}

const Token *Lexer::AndOrUnknown() {
  // should not be called if the current char is not '&'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer[0] == '&') {
    AdvanceCharPointer();
    return new Token(TokenType::And, "", cur_line, cur_col - 2);
  }

  return new Token(TokenType::Unknown, "&", cur_line, cur_col - 1);
}

const Token *Lexer::OrOrUnknown() {
  // should not be called if the current char is not '|'
  // current implementation does not check for this
  AdvanceCharPointer();

  if (cur_char_pointer[0] == '|') {
    AdvanceCharPointer();
    return new Token(TokenType::Or, "", cur_line, cur_col - 2);
  }

  return new Token(TokenType::Unknown, "|", cur_line, cur_col - 1);
}
