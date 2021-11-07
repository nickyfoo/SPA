#pragma once

#include <memory>
#include "entity.hpp"
#include "pkb.h"
#include "result_table.h"
#include "set"
#include "statement.h"
#include "such_that_clause.h"
#include "variable.h"
#include "vector"

class UsesSModifiesSHandler {
 public:
  static UsesSModifiesSHandler *get_instance();
  void set_args(PKB *pkb, std::shared_ptr<SuchThatClause> relationship,
                std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  ResultTable *EvaluateUsesS();
  ResultTable *EvaluateModifiesS();
  ResultTable *Evaluate(std::set<std::string> *(Statement::*get_normal)(),
                        std::set<int> *(Variable::*get_reverse)());

 private:
  static UsesSModifiesSHandler *instance_;
  UsesSModifiesSHandler();
  PKB *pkb_;
  std::shared_ptr<SuchThatClause> relationship_;
  std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec_;
};
