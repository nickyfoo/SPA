#pragma once
#include <string>
#include "entity_declaration.h"
#include "expression_spec.h"
#include "ent_ref.h"

class PatternClause {
 public:
  explicit PatternClause(EntityDeclaration *synonym);
  bool set_ref(EntRef *left, const std::string &right);
  EntityDeclaration *get_synonym();
  EntRef *get_left_ref();
  ExpressionSpec *get_right_ref();

 private:
  EntityDeclaration *synonym_;
  EntRef *left_ref_;
  ExpressionSpec *right_ref_;
  bool IsValidRightRef(std::string ref);
  static bool IsChar(char c);
  static bool IsExp(char c);
};
