#pragma once

#include <memory>

#include "pkb.h"
#include "result_table.h"
#include "such_that_clause.h"

class NextHandler {
 public:
  static NextHandler *get_instance();
  void set_args(PKB *pkb, std::shared_ptr<SuchThatClause> relationship);
  ResultTable *EvaluateNext();
  ResultTable *EvaluateNextT();

 private:
  static NextHandler *instance_;
  NextHandler();
  PKB *pkb_;
  std::shared_ptr<SuchThatClause> relationship_;
  ResultTable *evaluate(std::set<std::pair<int, int>> (PKB::*func)(int, int));
};
