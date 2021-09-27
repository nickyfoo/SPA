#pragma once
#include <string>
#include <vector>
#include "entity_declaration.h"

class WithClause {
 public:
  WithClause(std::string left_ref, std::string right_ref);
  std::string get_left_ref();
  std::string get_right_ref();
 private:
  std::string left_ref_;
  std::string right_ref_;
};
