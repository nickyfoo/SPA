#pragma once
#include <memory>
#include "unordered_map"
#include "string"
#include "set"
#include "vector"
#include "entity.hpp"
#include "such_that_clause.h"
#include "procedure.h"
#include "variable.h"
#include "pkb.h"
#include "result_table.h"

class UsesPModifiesPHandler {
 public:
  static UsesPModifiesPHandler *get_instance();
  void set_args(PKB *pkb,
                std::shared_ptr<SuchThatClause> relationship,
                std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  ResultTable *EvaluateUsesP();
  ResultTable *EvaluateModifiesP();
  ResultTable *Evaluate(std::set<std::string> *(Procedure::*get_normal)(),
                        std::set<std::string> *(Variable::*get_reverse)());

 private:
  static UsesPModifiesPHandler *instance_;
  UsesPModifiesPHandler();
  PKB *pkb_;
  std::shared_ptr<SuchThatClause> relationship_;
  std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec_;
};
