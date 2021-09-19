#include "pattern_clause.h"

PatternClause::PatternClause(EntityDeclaration *synonym) {
  this->synonym_ = synonym;
  this->left_ref_ = nullptr;
  this->right_ref_ = nullptr;
}

bool PatternClause::set_ref(EntRef *left, const std::string &right) {
  this->left_ref_ = left;
  return IsValidRightRef(right);
}

EntityDeclaration *PatternClause::get_synonym() {
  return this->synonym_;
}

EntRef *PatternClause::get_left_ref() {
  return this->left_ref_;
}

ExpressionSpec *PatternClause::get_right_ref() {
  return this->right_ref_;
}

bool PatternClause::IsValidRightRef(std::string ref) {
  printf("ref is: %s\n", ref.c_str());
  bool partial_pattern;
  auto *exp_spec = new ExpressionSpec();
  if (ref == "_") {
    exp_spec->set_wild_card();
    this->right_ref_ = exp_spec;
    return true;
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
  printf("ref is: %s\n", ref.c_str());
  for (char &c : ref) {
    printf("c now is: %c\n", c);
    if (!expecting_exp && IsExp(c)) {
      return false;
    } else if (std::isspace(c)) {
      printf("got come here not\n");
      if (expecting_exp) must_be_exp = true;
    } else {
      if (IsExp(c)) {
        expecting_exp = false;
        must_be_exp = false;
      } else if (IsChar(c)) {
        printf("char here is: %c\n", c);
        printf("must be exp is: %d\n", must_be_exp);
        if (must_be_exp) return false;
        expecting_exp = true;
      } else {
        return false;
      }
    }
  }

  if (!expecting_exp && !ref.empty()) {
    return false;
  }

  exp_spec->set_expression(ref);
  exp_spec->set_partial_pattern(partial_pattern);

  this->right_ref_ = exp_spec;
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
  if (c == '+' || c == '-' || c == '*' || c == '/')
    return true;
  return false;
}
