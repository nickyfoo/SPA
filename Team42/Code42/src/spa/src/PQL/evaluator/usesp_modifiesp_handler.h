#pragma once
#include "unordered_map"
#include "string"
#include "set"
#include "vector"
#include "entity.hpp"
#include "such_that_clause.h"
#include "procedure.h"
#include "variable.h"
#include "pkb.h"

class UsesPModifiesPHandler {
 public:
  static UsesPModifiesPHandler *get_instance();
  void set_args(PKB *pkb,
                std::unordered_map<std::string, std::vector<Entity *>>
                *synonym_to_entity_result,
                SuchThatClause *relationship,
                std::vector<std::string> *entities_to_return);
  void set_function_pointers(
      std::set<std::string> *(Procedure::*get_normal)(),
      std::set<std::string> *(Variable::*get_reverse)());
  void Evaluate();

 private:
  static UsesPModifiesPHandler *instance_;
  UsesPModifiesPHandler();
  std::set<std::string> *(Procedure::*get_normal_)();
  std::set<std::string> *(Variable::*get_reverse_)();
  PKB *pkb_;
  std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result_;
  SuchThatClause *relationship_;
  std::vector<std::string> *entities_to_return_;
  std::set<std::string> *ProcedureForwarder(
      std::set<std::string> *(Procedure::*function)(),
      Procedure *proc);
  std::set<std::string> *VariableForwarder(
      std::set<std::string> *(Variable::*function)(),
      Variable *var);
};
