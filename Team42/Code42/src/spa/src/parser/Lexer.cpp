#include "Lexer.h"
#include "ctype.h"
#include <string>

using namespace lexer;

char Lexer::get() { return source[idx]; }

void Lexer::next() {
  if (get() == '\n') {
    colNo = 0;
    lineNo++;
  } else {
    colNo++;
  }

  idx++;
}

Token *Lexer::getNextToken() {
  while (isspace(get())) {
    next();
  }

  if (isdigit(get())) {
    return constantOrUnknown();
  }

  if (isalpha(get())) {
    return identifierOrKeyword();
  }

  Token *res;

  switch (get()) {
  case '\0':
    res = new Token(Kind::End, "", lineNo, colNo);
    break;

  case ';':
    res = new Token(Kind::Semicolon, "", lineNo, colNo);
    break;

  case '(':
    res = new Token(Kind::LParen, "", lineNo, colNo);
    break;
  case ')':
    res = new Token(Kind::RParen, "", lineNo, colNo);
    break;
  case '{':
    res = new Token(Kind::LBrace, "", lineNo, colNo);
    break;
  case '}':
    res = new Token(Kind::RBrace, "", lineNo, colNo);
    break;

  case '+':
    res = new Token(Kind::Plus, "", lineNo, colNo);
    break;
  case '-':
    res = new Token(Kind::Minus, "", lineNo, colNo);
    break;
  case '/':
    res = new Token(Kind::Divide, "", lineNo, colNo);
    break;
  case '*':
    res = new Token(Kind::Multiply, "", lineNo, colNo);
    break;
  case '%':
    res = new Token(Kind::Modulo, "", lineNo, colNo);
    break;

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

  default:
    res = new Token(Kind::Unknown, std::string(1, get()), lineNo, colNo);
  }

  next();
  return res;
};

Token *Lexer::constantOrUnknown() {
  std::string value = "";
  int startCol = colNo;

  while (isdigit(get())) {
    value += get();
    next();
  }

  if (value.length() > 1 && value[0] == '0') {
    return new Token(Kind::Unknown, value, lineNo, startCol);
  }

  return new Token(Kind::Constant, value, lineNo, startCol);
}

Token *Lexer::identifierOrKeyword() {
  std::string value = "";
  int startCol = colNo;

  while (isalpha(get()) || isdigit(get())) {
    value += get();
    next();
  }

  if (value == "if") {
    return new Token(Kind::If, "", lineNo, startCol);
  }

  if (value == "then") {
    return new Token(Kind::Then, "", lineNo, startCol);
  }

  if (value == "else") {
    return new Token(Kind::Else, "", lineNo, startCol);
  }

  if (value == "while") {
    return new Token(Kind::While, "", lineNo, startCol);
  }

  if (value == "read") {
    return new Token(Kind::Read, "", lineNo, startCol);
  }

  if (value == "print") {
    return new Token(Kind::Print, "", lineNo, startCol);
  }

  if (value == "call") {
    return new Token(Kind::Call, "", lineNo, startCol);
  }

  if (value == "procedure") {
    return new Token(Kind::Procedure, "", lineNo, startCol);
  }

  return new Token(Kind::Identifier, value, lineNo, startCol);
}

Token *Lexer::gtOrGte() {
  int startCol = colNo;

  next();

  if (get() == '=') {
    next();
    return new Token(Kind::Gte, "", lineNo, startCol);
  }

  return new Token(Kind::Gt, "", lineNo, startCol);
}

Token *Lexer::ltOrLte() {
  next();

  if (get() == '=') {
    next();
    return new Token(Kind::Lte, "", lineNo, colNo - 2);
  }

  return new Token(Kind::Lt, "", lineNo, colNo - 1);
}

Token *Lexer::equalOrEq() {
  next();

  if (get() == '=') {
    next();
    return new Token(Kind::Eq, "", lineNo, colNo - 2);
  }

  return new Token(Kind::Equal, "", lineNo, colNo - 1);
}

Token *Lexer::notOrNeq() {
  next();

  if (get() == '=') {
    next();
    return new Token(Kind::Neq, "", lineNo, colNo - 2);
  }

  return new Token(Kind::Not, "", lineNo, colNo - 1);
}

Token *Lexer::andOrUnknown() {
  next();

  if (get() == '&') {
    next();
    return new Token(Kind::And, "", lineNo, colNo - 2);
  }

  return new Token(Kind::Unknown, "&", lineNo, colNo - 1);
}

Token *Lexer::orOrUnknown() {
  next();

  if (get() == '|') {
    next();
    return new Token(Kind::Or, "", lineNo, colNo - 2);
  }

  return new Token(Kind::Unknown, "|", lineNo, colNo - 1);
}
