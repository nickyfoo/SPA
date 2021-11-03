#include "pkb_exception.h"

PKBException::PKBException(const std::string &error) {
  this->msg_ = "PKBException: " + error;
}

const char *PKBException::what() const noexcept { return this->msg_.c_str(); }
