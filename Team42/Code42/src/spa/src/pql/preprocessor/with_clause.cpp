#include "with_clause.h"

WithClause::WithClause() = default;

WithClause::WithClause(std::string left_ref, EntityType left_type,
                       AttrValueType left_attr_value_type,
                       std::string right_ref, EntityType right_type,
                       AttrValueType right_attr_value_type) {
  this->left_ref_ = left_ref;
  this->left_type_ = left_type;
  this->left_attr_value_type_ = left_attr_value_type;
  this->right_ref_ = right_ref;
  this->right_type_ = right_type;
  this->right_attr_value_type_ = right_attr_value_type;
}

WithClause::~WithClause() = default;

std::string WithClause::get_left_ref() {
  return this->left_ref_;
}

std::string WithClause::get_right_ref() {
  return this->right_ref_;
}

EntityType WithClause::get_left_type() {
  return this->left_type_;
}

EntityType WithClause::get_right_type() {
  return this->right_type_;
}

AttrValueType WithClause::get_left_attr_value_type() {
  return this->left_attr_value_type_;
}

AttrValueType WithClause::get_right_attr_value_type() {
  return this->right_attr_value_type_;
}

void WithClause::set_values(std::string left_ref, EntityType left_type,
                            AttrValueType left_attr_value_type,
                            std::string right_ref, EntityType right_type,
                            AttrValueType right_attr_value_type) {
  this->left_ref_ = left_ref;
  this->left_type_ = left_type;
  this->left_attr_value_type_ = left_attr_value_type;
  this->right_ref_ = right_ref;
  this->right_type_ = right_type;
  this->right_attr_value_type_ = right_attr_value_type;
}
