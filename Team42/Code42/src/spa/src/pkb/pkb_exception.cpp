#include <cstdio>

#include "pkb_exception.h"
#include "string_utils.h"

PKBException::PKBException(std::string error) {
  this->msg_ = StringFormat("PKBException: %s", error.c_str());
}

const char *PKBException::what() const noexcept { return this->msg_.c_str(); }
