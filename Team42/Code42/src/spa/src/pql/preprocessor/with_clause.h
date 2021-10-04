#pragma once
#include <string>
#include <vector>
#include "entity_declaration.h"
#include "clause.hpp"

enum class AttrValueType {
  Name,
  Integer,
  None
};

class WithClause : public Clause {
 public:
  WithClause(std::string left_ref, EntityType left_type, AttrValueType left_attr_value_type,
             std::string right_ref, EntityType right_type, AttrValueType right_attr_value_type);
  ~WithClause();
  std::string get_left_ref();
  std::string get_right_ref();
  EntityType get_left_type();
  EntityType get_right_type();
  AttrValueType get_left_attr_value_type();
  AttrValueType get_right_attr_value_type();
 private:
  std::string left_ref_;
  std::string right_ref_;
  EntityType left_type_;
  EntityType right_type_;
  AttrValueType left_attr_value_type_;
  AttrValueType right_attr_value_type_;
};
