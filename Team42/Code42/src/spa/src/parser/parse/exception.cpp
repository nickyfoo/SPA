#include <cstdio>

#include "parse.h"
#include "string_utils.h"

ParseException::ParseException(std::string error, int line_no, int col_no) {
  this->msg_ = StringFormat("ParseError: %s [%d,%d]", error, line_no, col_no);
}

const char *ParseException::what() const noexcept { return this->msg_.c_str(); }
