#include "with_clause.h"

WithClause::WithClause(std::string left_ref, std::string right_ref) {
  this->left_ref_ = left_ref;
  this->right_ref_ = right_ref;
}

std::string WithClause::get_left_ref() {
  return this->left_ref_;
}

std::string WithClause::get_right_ref() {
  return this->right_ref_;
}