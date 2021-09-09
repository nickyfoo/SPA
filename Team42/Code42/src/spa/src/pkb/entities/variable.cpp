#include "variable.h"

Variable::Variable(int index, std::string name) {
  this->index_ = index;
  this->name_ = name;
}

Variable::~Variable() = default;

std::string Variable::GetName() {
  return name_;
}

int Variable::GetIndex() {
  return index_;
}