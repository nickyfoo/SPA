#pragma once

#include <memory>
#include <vector>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "such_that_clause.h"
#include "entity.hpp"
#include "pkb.h"
#include "result_table.h"

class RelationshipQueryHandler {
 public:
  explicit RelationshipQueryHandler(PKB *pkb);
  ResultTable *EvaluateRelationship(std::shared_ptr<SuchThatClause> relationship,
                                    std::unordered_map<std::string,
                                                       std::vector<Entity *>>
                                    synonym_to_entities_vec);
  ~RelationshipQueryHandler();
 private:
  PKB *pkb_;
};
