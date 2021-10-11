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

class PQLQuery {
 public:
  PQLQuery(std::vector<std::string> *return_entities,
           std::vector<std::shared_ptr<ClauseGroup>> clause_groups,
           std::unordered_map<std::string, EntityDeclaration *>
           *synonym_to_entities,
           bool has_one_repeated_synonym, bool has_two_repeated_synonyms);
  ~PQLQuery();
  std::vector<std::string> *get_query_entities();
  std::vector<std::shared_ptr<ClauseGroup>> get_clause_groups();
  std::unordered_map<std::string, EntityDeclaration *> *get_synonym_to_entities();
  bool has_one_repeated_synonym();
  bool has_two_repeated_synonyms();

 private:
  std::vector<std::string> *entities_;
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups_;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entities_;
  bool has_one_repeated_synonym_;
  bool has_two_repeated_synonyms_;
};
