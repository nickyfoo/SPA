#include "lexer.h"

BufferedLexer::BufferedLexer(const char *source) {
  // this implementation of BufferedLexer uses Lexer
  this->lexer = new Lexer(source);
  this->nextToken = nullptr;
}

const Token *BufferedLexer::getNextToken() {
  if (nextToken != nullptr) {
    const Token *tmp = nextToken;
    nextToken = nullptr;
    return tmp;
  }

  return lexer->getNextToken();
}

const Token *BufferedLexer::peekNextToken() {
  if (nextToken != nullptr) {
    return nextToken;
  }

  nextToken = lexer->getNextToken();
  return nextToken;
}
