#pragma once
#include "unordered_map"
#include "string"
#include "set"
#include "vector"
#include "statement.h"
#include "variable.h"
#include "pkb.h"
#include "entity.hpp"
#include "such_that_clause.h"

class UsesSModifiesSHandler {
 public:
  static UsesSModifiesSHandler *get_instance();
  void set_args(PKB *pkb,
                std::unordered_map<std::string, std::vector<Entity *>>
                *synonym_to_entity_result,
                SuchThatClause *relationship,
                std::vector<std::string> *entities_to_return,
                bool has_two_repeated_synonyms);
  void set_function_pointers(
      std::set<std::string> *(Statement::*get_normal)(),
      std::set<int> *(Variable::*get_reverse)());
  void Evaluate();
  std::vector<std::pair<int, std::string>> *get_stmt_var_pair_vector();

 private:
  static UsesSModifiesSHandler *instance_;
  UsesSModifiesSHandler();
  std::set<std::string> *(Statement::*get_normal_)();
  std::set<int> *(Variable::*get_reverse_)();
  PKB *pkb_;
  std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result_;
  SuchThatClause *relationship_;
  std::vector<std::string> *entities_to_return_;
  static std::set<std::string> *StatementForwarder(
      std::set<std::string> *(Statement::*function)(),
      Statement *stmt);
  static std::set<int> *VariableForwarder(
      std::set<int> *(Variable::*function)(),
      Variable *var);
  std::vector<std::pair<int, std::string>> *stmt_var_pair_vector_;
  bool has_two_repeated_synonyms_;
};