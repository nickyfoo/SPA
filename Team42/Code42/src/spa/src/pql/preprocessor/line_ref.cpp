#include "line_ref.h"

LineRef::LineRef() {
  this->type_ = LineRefType::None;
  this->line_num_ = -1;
}

void LineRef::set_synonym(const std::string &syn) {
  this->type_ = LineRefType::Synonym;
  this->synonym_ = syn;
}

void LineRef::set_line_num(int line_num) {
  this->type_ = LineRefType::StmtNum;
  this->line_num_ = line_num;
}

void LineRef::set_wild_card() {
  this->type_ = LineRefType::WildCard;
}

LineRefType LineRef::get_type() {
  return this->type_;
}

std::string LineRef::get_synonym() {
  return this->synonym_;
}

int LineRef::get_line_num() const {
  return this->line_num_;
}

// used for logging purposes
std::string LineRef::get_value() {
  switch (this->type_) {
    case LineRefType::Synonym:return this->synonym_;
    case LineRefType::StmtNum:return std::to_string(this->line_num_);
    case LineRefType::WildCard:return "_";
    default:return "No value!";
  }
}

void LineRef::set_entity_type(EntityType type) {
  this->entity_type_ = type;
}

EntityType LineRef::get_entity_type() {
  return this->entity_type_;
}
