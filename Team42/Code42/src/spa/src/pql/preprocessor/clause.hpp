#pragma once

enum class ClauseType {
  SuchThatClause,
  PatternClause,
  WithClause
};

class Clause {
 public:
  void set_type(ClauseType clause_type) {
    this->clause_type_ = clause_type;
  }
  ClauseType get_type() {
    return this->clause_type_;
  }

 protected:
  Clause() {}

  virtual ~Clause() {};

 private:
  ClauseType clause_type_;
};
