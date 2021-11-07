#include "lexer.h"

#include "iostream"

BufferedLexer::BufferedLexer(const std::string &source) {
  // this implementation of BufferedLexer uses Lexer
  this->lexer_ = new Lexer(source);
  this->buffer_ = std::vector<const Token *>();
}

const Token *BufferedLexer::GetNextToken() {
  if (!this->buffer_.empty()) {
    const Token *res = this->buffer_.front();
    this->buffer_.erase(this->buffer_.begin());
    return res;
  }

  auto n = this->lexer_->GetNextToken();
  return n;
}

const Token *BufferedLexer::PeekNextToken() {
  if (!this->buffer_.empty()) {
    return this->buffer_.front();
  }

  this->buffer_.push_back(lexer_->GetNextToken());
  return this->buffer_.front();
}

const Token *BufferedLexer::PeekNextToken(int offset) {
  if (this->buffer_.size() > offset) {
    return this->buffer_[offset];
  }

  for (auto i = this->buffer_.size(); i <= offset; i++) {
    this->buffer_.push_back(this->lexer_->GetNextToken());
  }

  return this->buffer_[offset];
}
