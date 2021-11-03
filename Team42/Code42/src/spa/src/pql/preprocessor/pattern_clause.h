#pragma once
#include <string>
#include "entity_declaration.h"
#include "expression_spec.h"
#include "ent_ref.h"
#include "clause.hpp"

class PatternClause : public Clause {
 public:
  explicit PatternClause(EntityDeclaration *synonym);
  ~PatternClause();
  bool set_ref(EntRef *variable, const std::string &right_ref);
  EntityDeclaration *get_synonym();
  EntityType get_type();
  EntRef *get_variable();
  ExpressionSpec *get_exp_spec();
  bool IsSemanticallyValidExpSpec();

 private:
  EntityDeclaration *synonym_;
  EntityType type_;
  EntRef *variable_;
  ExpressionSpec *exp_spec_;
  bool IsSyntacticallyValidExpSpec(std::string ref);
  static bool IsChar(char c);
  static bool IsExp(char c);
};
