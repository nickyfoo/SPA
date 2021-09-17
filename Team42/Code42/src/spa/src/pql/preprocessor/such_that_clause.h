#pragma once
#include <string>
#include "such_that_ref.h"

enum class RelRef {
  Follows,
  FollowsT,
  Parent,
  ParentT,
  Uses,
  UsesS,
  UsesP,
  Modifies,
  ModifiesS,
  ModifiesP,
  None
};

class SuchThatClause {
 public:
  explicit SuchThatClause(const std::string &type);
  bool set_ref(SuchThatRef *left, SuchThatRef *right);
  void set_type(RelRef rel);
  RelRef get_type();
  std::string get_type_str();
  SuchThatRef *get_left_ref();
  SuchThatRef *get_right_ref();
 private:
  RelRef type_;
  SuchThatRef *left_ref_;
  SuchThatRef *right_ref_;
};
