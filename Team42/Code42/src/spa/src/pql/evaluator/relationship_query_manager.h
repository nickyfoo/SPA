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
  RelationshipQueryManager(PKB *pkb);
  ResultTable *EvaluateRelationship(SuchThatClause relationship,
                                    std::unordered_map<std::string,
                                                       std::vector<Entity *>> synonym_to_entities_vec);
  ~RelationshipQueryManager();
 private:
  PKB *pkb_;
};
