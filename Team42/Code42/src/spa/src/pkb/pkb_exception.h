#pragma once
#include <exception>
#include <string>

class PKBException : public std::exception {
 public:
  explicit PKBException(const std::string &error);
  virtual const char *what() const noexcept;

 private:
  std::string msg_;
};
