#pragma once

#include <utility>
#include <vector>
#include <pkb.h>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "pattern_clause.h"
#include "entity.hpp"
#include "result_table.h"

class PatternQueryManager {
 public:
  PatternQueryManager(PKB *pkb);
  ~PatternQueryManager();
  ResultTable* EvaluatePattern(PatternClause pattern,
                               std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  std::vector<std::pair<int, std::string>> *get_vec_results();

 private:
  PKB *pkb_;
  std::vector<std::pair<int, std::string>> *vec_results_;
};
