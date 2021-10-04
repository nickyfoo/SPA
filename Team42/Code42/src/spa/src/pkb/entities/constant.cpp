#include "constant.h"

Constant::Constant() {}

Constant::Constant(std::string value) {
  this->value_ = value;
}

Constant::~Constant() = default;

std::string Constant::get_value() {
  return value_;
}
