#pragma once

#include <memory>
#include <unordered_map>

#include "pkb.h"
#include "result_table.h"
#include "such_that_clause.h"

class CallsHandler {
 public:
  static CallsHandler *get_instance();
  void set_args(PKB *pkb, std::shared_ptr<SuchThatClause> relationship,
                std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  ResultTable *EvaluateCalls();
  ResultTable *EvaluateCallsT();

 private:
  static CallsHandler *instance_;
  CallsHandler();
  PKB *pkb_;
  std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec_;
  std::shared_ptr<SuchThatClause> relationship_;
  ResultTable *Evaluate(std::set<std::string> *(Procedure::*get_normal)(),
                        std::set<std::string> *(Procedure::*get_reverse)());
};
