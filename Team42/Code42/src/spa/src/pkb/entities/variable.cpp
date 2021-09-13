#include "variable.h"

Variable::Variable(int index, std::string name) {
  this->index_ = index;
  this->name_ = name;
}

Variable::~Variable() = default;

std::string Variable::get_name() {
  return name_;
}

int Variable::get_index() {
  return index_;
}