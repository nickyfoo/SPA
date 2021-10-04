#pragma once
#include <string>
#include "such_that_ref.h"
#include "clause.hpp"

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
  Calls,
  CallsT,
  Next,
  NextT,
  Affects,
  AffectsT,
  None
};

class SuchThatClause : public Clause {
 public:
  explicit SuchThatClause(const std::string &type);
  ~SuchThatClause();
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
