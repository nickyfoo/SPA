#pragma once

#include "vector"
#include "unordered_map"
#include "string"
#include "pkb.h"
#include "pql_query.h"
#include "entity_declaration.h"

class QueryEvaluator {
 public:
  explicit QueryEvaluator(PQLQuery *pql_query, PKB *pkb);
  std::vector<std::string> *Evaluate();
 private:
  std::vector<std::string> *entities_to_return_;
  std::vector<SuchThatClause *> *relationships_;
  PKB *pkb_;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entity_dec_;
  static bool IsEmpty(
      std::unordered_map<std::string, std::vector<Entity *>>
      *synonym_to_entity_result);
  std::vector<std::string> *ConvertToOutput(
      std::unordered_map<std::string,
                         std::vector<Entity *>> *synonym_to_entity_result);
  static bool IsStmt(EntityType entity_type);
};
