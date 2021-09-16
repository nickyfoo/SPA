#pragma once

#include "set"
#include "vector"
#include "string"
#include "unordered_map"
#include "pkb.h"
#include "such_that_clause.h"
#include "entity.hpp"
#include "statement.h"

class FollowsParentsHandler {
 public:
  static FollowsParentsHandler *get_instance();
  void set_args(PKB *pkb,
                std::unordered_map<std::string, std::vector<Entity *>>
                *synonym_to_entity_result,
                SuchThatClause *relationship,
                std::vector<std::string> *entities_to_return);
  void set_function_pointers(std::set<int> *(Statement::*get_normal)(),
      std::set<int> *(Statement::*get_reverse)());
  void Evaluate();

 private:
  static FollowsParentsHandler *instance_;
  FollowsParentsHandler();
  std::set<int> *(Statement::*get_normal_)();
  std::set<int> *(Statement::*get_reverse_)();
  PKB *pkb_;
  std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result_;
  SuchThatClause *relationship_;
  std::vector<std::string> *entities_to_return_;
  std::set<int> *Forwarder(std::set<int> *(Statement::*function)(), Statement *stmt);
};
