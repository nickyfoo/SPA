#include "pattern_clause.h"

PatternClause::PatternClause(EntityDeclaration *synonym) {
  this->synonym_ = synonym;
  this->type_ = synonym->get_type();
  this->variable_ = nullptr;
  this->exp_spec_ = nullptr;
}

PatternClause::~PatternClause() = default;

bool PatternClause::set_ref(EntRef *variable, const std::string &right_ref) {
  this->variable_ = variable;
  if (this->type_ == EntityType::Assign) {
    return IsSyntacticallyValidExpSpec(right_ref);
  } else {
    return right_ref == "_";
  }
}

EntityDeclaration *PatternClause::get_synonym() {
  return this->synonym_;
}

EntityType PatternClause::get_type() {
  return this->type_;
}

EntRef *PatternClause::get_variable() {
  return this->variable_;
}

ExpressionSpec *PatternClause::get_exp_spec() {
  return this->exp_spec_;
}

bool PatternClause::IsSyntacticallyValidExpSpec(std::string ref) {

  bool partial_pattern;
  auto *exp_spec = new ExpressionSpec();
  if (ref == "_") {
    exp_spec->set_wild_card();
    this->exp_spec_ = exp_spec;
    return true;
  } else if (ref == "") {
    return false;
  }

  if (ref.at(0) == '_' && ref.at(ref.length() - 1) == '_') {
    partial_pattern = true;
    ref = ref.substr(1, ref.length() - 2);  // remove _
  } else {
    partial_pattern = false;
  }

  // check for '\''
  if (!((ref.at(0) == '\'' && ref.at(ref.length() - 1) == '\'') ||
      (ref.at(0) == '\"' && ref.at(ref.length() - 1) == '\"'))) {
    return false;
  }

  ref = ref.substr(1, ref.length() - 2);  // remove ""

  bool expecting_exp = false;
  bool must_be_exp = false;
  bool must_be_char = true;
  int bracket_count = 0;
  for (char &c : ref) {
    if ((must_be_char && IsExp(c)) || (must_be_exp && IsChar(c))) {
      return false;
    } else {
      if (IsExp(c)) {
        expecting_exp = false;
        must_be_exp = false;
        must_be_char = true;
      } else if (IsChar(c)) {
        expecting_exp = true;
        must_be_char = false;
      } else if (c == '(') {
        bracket_count++;
      } else if (c == ')') {
        bracket_count--;
      } else if (isspace(c)) {
        if (expecting_exp) {
          must_be_exp = true;
          must_be_char = false;
        } else {
          must_be_exp = false;
          must_be_char = true;
        }
      } else {
        return false;
      }
    }
    if (bracket_count < 0) return false;
  }
  if (bracket_count != 0 || (!expecting_exp && !ref.empty())) {
    return false;
  }

  exp_spec->set_expression(ref);
  exp_spec->set_partial_pattern(partial_pattern);

  this->exp_spec_ = exp_spec;
  return true;
}

bool PatternClause::IsChar(char c) {
  if (!((c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z')
      || (c >= '0' && c <= '9')
      || c == '_'))
    return false;
  return true;
}

bool PatternClause::IsExp(char c) {
  if (c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
    return true;
  return false;
}
