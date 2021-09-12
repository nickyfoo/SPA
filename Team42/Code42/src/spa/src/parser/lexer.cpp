#include "ctype.h"
#include "lexer.h"

Lexer::Lexer(const char *source) {
  this->source = source;
  this->curLine = 1;
  this->curCol = 0;
  this->curCharPointer = source;
}

void Lexer::advanceCharPointer() {
  if (curCharPointer[0] == '\n') {
    curCol = 0;
    curLine++;
  } else {
    curCol++;
  }

  curCharPointer++;
}

const Token *Lexer::getNextToken() {
  // idea behind this implementation is to peek at the next character,
  // and greedily evaluate the longest matching token type
  while (isspace(curCharPointer[0])) {
    advanceCharPointer();
  }

  if (isdigit(curCharPointer[0])) {
    return constantOrUnknown();
  }

  if (isalpha(curCharPointer[0])) {
    return identifierOrKeyword();
  }

  switch (curCharPointer[0]) {
    case '>':
      return gtOrGte();
    case '<':
      return ltOrLte();
    case '=':
      return equalOrEq();
    case '!':
      return notOrNeq();
    case '&':
      return andOrUnknown();
    case '|':
      return orOrUnknown();
  }

  const Token *res = nullptr;
  switch (curCharPointer[0]) {
    case '\0':
      res = new Token(TokenType::End, "", curLine, curCol);
      break;
    case ';':
      res = new Token(TokenType::Semicolon, "", curLine, curCol);
      break;
    case '(':
      res = new Token(TokenType::LParen, "", curLine, curCol);
      break;
    case ')':
      res = new Token(TokenType::RParen, "", curLine, curCol);
      break;
    case '{':
      res = new Token(TokenType::LBrace, "", curLine, curCol);
      break;
    case '}':
      res = new Token(TokenType::RBrace, "", curLine, curCol);
      break;
    case '+':
      res = new Token(TokenType::Plus, "", curLine, curCol);
      break;
    case '-':
      res = new Token(TokenType::Minus, "", curLine, curCol);
      break;
    case '/':
      res = new Token(TokenType::Divide, "", curLine, curCol);
      break;
    case '*':
      res = new Token(TokenType::Multiply, "", curLine, curCol);
      break;
    case '%':
      res = new Token(TokenType::Modulo, "", curLine, curCol);
      break;
    default:
      res = new Token(TokenType::Unknown, std::string(1, curCharPointer[0]),
                      curLine, curCol);
  }
  advanceCharPointer();
  return res;
}

const Token *Lexer::constantOrUnknown() {
  std::string value = "";
  int startCol = curCol;

  while (isdigit(curCharPointer[0])) {
    value += curCharPointer[0];
    advanceCharPointer();
  }

  if (value.length() > 1 && value[0] == '0') {
    return new Token(TokenType::Unknown, value, curLine, startCol);
  }

  return new Token(TokenType::Constant, value, curLine, startCol);
}

const Token *Lexer::identifierOrKeyword() {
  std::string value = "";
  int startCol = curCol;

  while (isalpha(curCharPointer[0]) || isdigit(curCharPointer[0])) {
    value += curCharPointer[0];
    advanceCharPointer();
  }

  if (value == "if") {
    return new Token(TokenType::If, "", curLine, startCol);
  }
  if (value == "then") {
    return new Token(TokenType::Then, "", curLine, startCol);
  }
  if (value == "else") {
    return new Token(TokenType::Else, "", curLine, startCol);
  }
  if (value == "while") {
    return new Token(TokenType::While, "", curLine, startCol);
  }
  if (value == "read") {
    return new Token(TokenType::Read, "", curLine, startCol);
  }
  if (value == "print") {
    return new Token(TokenType::Print, "", curLine, startCol);
  }
  if (value == "call") {
    return new Token(TokenType::Call, "", curLine, startCol);
  }
  if (value == "procedure") {
    return new Token(TokenType::Procedure, "", curLine, startCol);
  }

  return new Token(TokenType::Identifier, value, curLine, startCol);
}

const Token *Lexer::gtOrGte() {
  // should not be called if the current char is not '>'
  // current implementation does not check for this
  advanceCharPointer();

  if (curCharPointer[0] == '=') {
    advanceCharPointer();
    return new Token(TokenType::Gte, "", curLine, curCol - 2);
  }

  return new Token(TokenType::Gt, "", curLine, curCol - 1);
}

const Token *Lexer::ltOrLte() {
  // should not be called if the current char is not '<'
  // current implementation does not check for this
  advanceCharPointer();

  if (curCharPointer[0] == '=') {
    advanceCharPointer();
    return new Token(TokenType::Lte, "", curLine, curCol - 2);
  }

  return new Token(TokenType::Lt, "", curLine, curCol - 1);
}

const Token *Lexer::equalOrEq() {
  // should not be called if the current char is not '='
  // current implementation does not check for this
  advanceCharPointer();

  if (curCharPointer[0] == '=') {
    advanceCharPointer();
    return new Token(TokenType::Eq, "", curLine, curCol - 2);
  }

  return new Token(TokenType::Equal, "", curLine, curCol - 1);
}

const Token *Lexer::notOrNeq() {
  // should not be called if the current char is not '!'
  // current implementation does not check for this
  advanceCharPointer();

  if (curCharPointer[0] == '=') {
    advanceCharPointer();
    return new Token(TokenType::Neq, "", curLine, curCol - 2);
  }

  return new Token(TokenType::Not, "", curLine, curCol - 1);
}

const Token *Lexer::andOrUnknown() {
  // should not be called if the current char is not '&'
  // current implementation does not check for this
  advanceCharPointer();

  if (curCharPointer[0] == '&') {
    advanceCharPointer();
    return new Token(TokenType::And, "", curLine, curCol - 2);
  }

  return new Token(TokenType::Unknown, "&", curLine, curCol - 1);
}

const Token *Lexer::orOrUnknown() {
  // should not be called if the current char is not '|'
  // current implementation does not check for this
  advanceCharPointer();

  if (curCharPointer[0] == '|') {
    advanceCharPointer();
    return new Token(TokenType::Or, "", curLine, curCol - 2);
  }

  return new Token(TokenType::Unknown, "|", curLine, curCol - 1);
}
