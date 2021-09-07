#include "lexer.h"

BufferedLexer::BufferedLexer(const char *source) {
  // this implementation of BufferedLexer uses Lexer
  this->lexer = new Lexer(source);
  this->next_token = nullptr;
}

const Token *BufferedLexer::GetNextToken() {
  if (next_token != nullptr) {
    const Token *tmp = next_token;
    next_token = nullptr;
    return tmp;
  }

  return lexer->GetNextToken();
}

const Token *BufferedLexer::PeekNextToken() {
  if (next_token != nullptr) {
    return next_token;
  }

  next_token = lexer->GetNextToken();
  return next_token;
}
