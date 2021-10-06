#pragma once
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

#include "entity_declaration.h"
#include "pattern_clause.h"
#include "such_that_clause.h"
#include "with_clause.h"
#include "clause_group.h"
#include "result_clause.h"

class PQLQuery {
 public:
  PQLQuery(std::vector<ResultClause *> *return_entities,
           std::vector<std::shared_ptr<ClauseGroup>> clause_groups,
           std::unordered_map<std::string, EntityDeclaration *>
           *synonym_to_entities);
  ~PQLQuery();
  std::vector<ResultClause *> *get_query_entities();
  std::vector<std::shared_ptr<ClauseGroup>> get_clause_groups();
  std::unordered_map<std::string, EntityDeclaration *> *get_synonym_to_entities();

 private:
  std::vector<ResultClause *> *entities_;
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups_;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entities_;
};
