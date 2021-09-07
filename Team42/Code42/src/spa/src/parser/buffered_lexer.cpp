#include "lexer.h"

BufferedLexer::BufferedLexer(const char *source) {
  // this implementation of BufferedLexer uses Lexer
  this->lexer_ = new Lexer(source);
  this->next_token_ = nullptr;
}

const Token *BufferedLexer::GetNextToken() {
  if (next_token_ != nullptr) {
    const Token *tmp = next_token_;
    next_token_ = nullptr;
    return tmp;
  }

  return lexer_->GetNextToken();
}

const Token *BufferedLexer::PeekNextToken() {
  if (next_token_ != nullptr) {
    return next_token_;
  }

  next_token_ = lexer_->GetNextToken();
  return next_token_;
}
