#include "constant.h"

Constant::Constant() {}

Constant::Constant(int value) {
  this->value_ = value;
}

Constant::~Constant() = default;

int Constant::get_value() {
  return value_;
}
