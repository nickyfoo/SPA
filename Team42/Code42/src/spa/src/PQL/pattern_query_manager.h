#pragma once

#include <vector>
#include <pkb.h>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "pattern_clause.h"
#include "entity.hpp"

class PatternQueryManager {
 public:
  PatternQueryManager(std::unordered_map<std::string,
                                         std::vector<Entity *>> *synonym_to_entity_result,
                      std::vector<PatternClause *> *patterns,
                      std::vector<std::string> *entities_to_return,
                      PKB *pkb);
  void EvaluatePatterns();

 private:
  std::unordered_map<std::string, std::vector<Entity *>> *synonym_to_entity_result_;
  std::vector<PatternClause *> *patterns_;
  std::vector<std::string> *entities_to_return_;
  PKB *pkb_;
  void ManagePatterns(PatternClause *pattern);
};
