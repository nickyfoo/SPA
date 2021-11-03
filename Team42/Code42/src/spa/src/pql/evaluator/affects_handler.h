#pragma once

#include <memory>

#include "pkb.h"
#include "result_table.h"
#include "such_that_clause.h"

class AffectsHandler {
 public:
  static AffectsHandler *get_instance();
  void set_args(PKB *pkb, std::shared_ptr<SuchThatClause> relationship,
                std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  ResultTable *EvaluateAffects();
  ResultTable *EvaluateAffectsT();
  ResultTable *EvaluateAffectsBip();
  ResultTable *EvaluateAffectsTBip();

 private:
  static AffectsHandler *instance_;
  AffectsHandler();
  PKB *pkb_;
  std::shared_ptr<SuchThatClause> relationship_;
  ResultTable *Evaluate(std::set<std::pair<int, int>> *(PKB::*func)(int, int));
  std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec_;
};
