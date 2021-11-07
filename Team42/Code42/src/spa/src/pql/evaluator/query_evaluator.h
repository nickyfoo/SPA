#pragma once

#include "vector"
#include "unordered_map"
#include "unordered_set"
#include "string"
#include "pkb.h"
#include "pql_query.h"
#include "entity_declaration.h"
#include "result_table.h"
#include "optimizer/clause_group.h"

class QueryEvaluator {
 public:
  explicit QueryEvaluator(PQLQuery *pql_query, PKB *pkb);
  std::vector<std::string> *Evaluate();
 private:
  std::vector<ResultClause *> *entities_to_return_;
  std::vector<std::shared_ptr<ClauseGroup>> clause_groups_;
  PKB *pkb_;
  std::unordered_map<std::string, EntityDeclaration *> *synonym_to_entity_dec_;
  std::unordered_set<std::string> used_synonyms_;
  bool is_syntactically_valid_;
  bool is_semantically_valid_;
  std::unordered_map<std::string, std::vector<Entity *>> GetPossibleEntitiesVec(
      ClauseVertex clause_vertex);
  std::vector<std::string> *ConvertToOutput(ResultTable *table_result, bool is_valid_query);
  void MakeTableForUnusedEntity(ResultTable *result_table, ResultClause *result_clause);
  void MakeTableForUsedEntity(ResultTable *result_table,
                              ResultClause *result_clause,
                              ResultTable *other_result_table);
  void set_used_synonyms();
};
