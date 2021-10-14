#pragma once

#include <pkb.h>
#include <memory>
#include <tuple>
#include <utility>
#include <vector>

#include "entity.hpp"
#include "entity_declaration.h"
#include "result_table.h"
#include "string"
#include "unordered_map"
#include "with_clause.h"

class WithQueryManager {
 public:
  WithQueryManager();
  ~WithQueryManager();
  ResultTable *EvaluateWith(
      std::shared_ptr<WithClause> with,
      std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);

 private:
  std::vector<std::string> GetNames(
      std::string synonym, EntityType type, std::string argument,
      std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  std::vector<std::string> GetIntegers(
      std::string synonym, EntityType type, std::string argument,
      std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
  std::tuple<std::vector<std::string>, std::vector<std::string>> GetSynonymPairs(
      std::shared_ptr<WithClause> with,
      std::unordered_map<std::string, std::vector<Entity *>> synonym_to_entities_vec);
};
