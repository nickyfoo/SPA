#pragma once

#include <tuple>
#include <utility>
#include <vector>
#include <pkb.h>
#include "unordered_map"
#include "string"
#include "entity_declaration.h"
#include "with_clause.h"
#include "entity.hpp"
#include "result_table.h"

class WithQueryManager {
 public:
  WithQueryManager();
  ~WithQueryManager();
  ResultTable *EvaluateWith(std::shared_ptr<WithClause> with,
                            std::unordered_map<std::string,
                            std::vector<Entity *>> synonym_to_entities_vec);
 private:
  std::vector<std::string> GetNames(std::string synonym, EntityType type,
                                    std::string argument,
                                    std::unordered_map<std::string,
                                                       std::vector<Entity *>>
                                                       synonym_to_entities_vec);
  std::vector<std::string> GetIntegers(std::string synonym, EntityType type,
                                       std::string argument,
                                       std::unordered_map<std::string,
                                                          std::vector<Entity *>>
                                                          synonym_to_entities_vec);
  std::tuple<std::vector<std::string>, std::vector<std::string>> GetSynonymPairs(
      std::shared_ptr<WithClause> with, std::unordered_map<std::string,
      std::vector<Entity *>> synonym_to_entities_vec);
};
