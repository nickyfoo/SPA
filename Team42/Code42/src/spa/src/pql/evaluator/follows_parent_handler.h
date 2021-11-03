#pragma once

#include <memory>
#include "set"
#include "vector"
#include "string"
#include "unordered_map"
#include "pkb.h"
#include "such_that_clause.h"
#include "entity.hpp"
#include "statement.h"
#include "result_table.h"

class FollowsParentHandler {
 public:
  static FollowsParentHandler *get_instance();
  void set_args(PKB *pkb,
                std::shared_ptr<SuchThatClause> relationship,
                std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  ResultTable *EvaluateFollows();
  ResultTable *EvaluateFollowsT();
  ResultTable *EvaluateParent();
  ResultTable *EvaluateParentT();
  ResultTable *Evaluate(std::set<int> *(Statement::*get_normal)(),
                        std::set<int> *(Statement::*get_reverse)());

 private:
  static FollowsParentHandler *instance_;
  FollowsParentHandler();
  PKB *pkb_;
  std::shared_ptr<SuchThatClause> relationship_;
  std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec_;
};
