#include "Procedure.h"

Procedure::Procedure(std::string name) {
  this->name_ = name;
}

Procedure::~Procedure() {}

std::string Procedure::GetName() {
  return name_;
}