#include "constant.h"

Constant::Constant() {
  this->value_ = -1;
}

Constant::Constant(int value) {
  this->value_ = value;
}

Constant::~Constant() = default;

int Constant::get_value() {
  return value_;
}