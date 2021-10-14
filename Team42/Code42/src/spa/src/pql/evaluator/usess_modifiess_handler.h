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
  void set_function_pointers(std::set<std::string> *(Statement::*get_normal)(),
                             std::set<int> *(Variable::*get_reverse)());
  ResultTable *Evaluate();

 private:
  static UsesSModifiesSHandler *instance_;
  UsesSModifiesSHandler();
  std::set<std::string> *(Statement::*get_normal_)();
  std::set<int> *(Variable::*get_reverse_)();
  PKB *pkb_;
  std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result_;
  std::shared_ptr<SuchThatClause> relationship_;
  std::vector<std::string> *entities_to_return_;
  static std::set<std::string> *StatementForwarder(std::set<std::string> *(Statement::*function)(),
                                                   Statement *stmt);
  static std::set<int> *VariableForwarder(std::set<int> *(Variable::*function)(), Variable *var);
  std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec_;
};
