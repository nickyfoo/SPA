#include "lexer.h"

#include "iostream"

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

  auto n = lexer_->GetNextToken();
  std::cout << static_cast<typename std::underlying_type<TokenType>::type>(n->kind_) << "\n";

  return n;
}

const Token *BufferedLexer::PeekNextToken() {
  if (next_token_ != nullptr) {
    return next_token_;
  }

  next_token_ = lexer_->GetNextToken();
  return next_token_;
}

