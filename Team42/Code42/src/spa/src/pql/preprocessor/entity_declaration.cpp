#include "entity_declaration.h"

EntityDeclaration::EntityDeclaration(EntityType type, std::string synonym) {
  this->type_ = type;
  this->synonym_ = synonym;
}

std::string EntityDeclaration::get_synonym() {
  return this->synonym_;
}

EntityType EntityDeclaration::get_type() {
  return this->type_;
}
