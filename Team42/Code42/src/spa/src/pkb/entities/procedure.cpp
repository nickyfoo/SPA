#include "procedure.h"

Procedure::Procedure(std::string name) {
  this->name_ = name;
}

Procedure::~Procedure() = default;

std::string Procedure::get_name() {
  return name_;
}