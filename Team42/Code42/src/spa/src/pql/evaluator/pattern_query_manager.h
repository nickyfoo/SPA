#pragma once

#include <utility>
#include <vector>
#include <pkb.h>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "pattern_clause.h"
#include "entity.hpp"

class PatternQueryManager {
 public:
  PatternQueryManager(
      std::unordered_map<std::string, std::vector<Entity *>>
      *synonym_to_entity_result,
      std::vector<PatternClause *> *patterns,
      std::vector<std::string> *entities_to_return,
      PKB *pkb,
      bool has_two_repeated_synonyms);
  ~PatternQueryManager();
  void EvaluatePatterns();
  std::vector<std::pair<int, std::string>> *get_vec_results();

 private:
  std::unordered_map<std::string, std::vector<Entity *>>
      *synonym_to_entity_result_;
  std::vector<PatternClause *> *patterns_;
  std::vector<std::string> *entities_to_return_;
  PKB *pkb_;
  std::vector<std::pair<int, std::string>> *vec_results_;
  bool has_two_repeated_synonyms_;
  void ManagePatterns(PatternClause *pattern);
};
