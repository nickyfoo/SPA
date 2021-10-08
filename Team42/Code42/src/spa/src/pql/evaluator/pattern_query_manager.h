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
  explicit PatternQueryManager(PKB *pkb);
  ~PatternQueryManager();
  ResultTable* EvaluatePattern(std::shared_ptr<PatternClause> pattern,
                               const std::unordered_map<std::string,
                               std::vector<Entity *>>& synonym_to_entities_vec);
  ResultTable* EvaluateAssignPattern(std::shared_ptr<PatternClause> pattern,
                                     std::unordered_map<std::string,
                                     std::vector<Entity *>> synonym_to_entities_vec);
  ResultTable* EvaluateIfAndWhilePattern(std::shared_ptr<PatternClause> pattern,
                                         std::unordered_map<std::string,
                                         std::vector<Entity *>> synonym_to_entities_vec);
  std::vector<std::pair<int, std::string>> *get_vec_results();

 private:
  PKB *pkb_;
};
