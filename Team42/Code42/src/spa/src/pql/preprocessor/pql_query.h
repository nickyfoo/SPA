#pragma once
#include <unordered_map>
#include <vector>

#include "entity_declaration.h"
#include "pattern_clause.h"
#include "such_that_clause.h"

class PQLQuery {
 public:
  PQLQuery(std::vector<std::string> *return_entities, std::vector<SuchThatClause *> *relationships,
           std::vector<PatternClause *> *patterns,
           std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entities,
           bool has_one_repeated_synonym, bool has_two_repeated_synonyms);
  ~PQLQuery();
  std::vector<std::string> *get_query_entities();
  std::vector<SuchThatClause *> *get_query_relationships();
  std::vector<PatternClause *> *get_query_patterns();
  std::unordered_map<std::string, EntityDeclaration *> *get_synonym_to_entities();
  bool has_one_repeated_synonym();
  bool has_two_repeated_synonyms();

 private:
  std::vector<std::string> *entities_;
  std::vector<SuchThatClause *> *relationships_;
  std::vector<PatternClause *> *patterns_;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entities_;
  bool has_one_repeated_synonym_;
  bool has_two_repeated_synonyms_;
};
