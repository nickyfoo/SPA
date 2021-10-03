#pragma once

#include <vector>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "such_that_clause.h"
#include "entity.hpp"
#include "pkb.h"
#include "result_table.h"

class RelationshipQueryManager {
 public:
  RelationshipQueryManager(
      std::unordered_map<std::string, std::vector<Entity *>>
      *synonym_to_entity_result,
      std::vector<std::string> *entities_to_return,
      PKB *pkb);
  ResultTable *EvaluateRelationship(SuchThatClause relationship,
                                    std::unordered_map<std::string,
                                                       std::vector<Entity *>> synonym_to_entities_vec);
  ~RelationshipQueryManager();
  std::vector<std::pair<int, std::string>> *get_stmt_var_pair_vector();
 private:
  std::unordered_map<std::string, std::vector<Entity *>>
      *synonym_to_entity_result_;
  std::vector<SuchThatClause *> *relationships_;
  std::vector<std::string> *entities_to_return_;
  PKB *pkb_;
  bool has_two_repeated_synonyms_;
  std::vector<std::pair<int, std::string>> *stmt_var_pair_vector_;
};
