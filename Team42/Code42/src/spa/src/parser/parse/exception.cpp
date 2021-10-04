#include <cstdio>

#include "parse.h"

ParseException::ParseException(std::string error, int line_no, int col_no) {
  this->msg_ =
      "ParseError: " + error + " [" + std::to_string(line_no) + "," + std::to_string(col_no) + "]";
}

const char *ParseException::what() const noexcept { return this->msg_.c_str(); }
