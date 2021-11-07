#pragma once

#include "vector"
#include "unordered_map"
#include "memory"
#include "string"
#include "such_that_clause.h"
#include "pattern_clause.h"
#include "with_clause.h"
#include "clause_group.h"
#include "clause_vertex.h"
#include "result_clause.h"

class QueryOptimizer {
 public:
  QueryOptimizer(std::vector<SuchThatClause *> *relationships,
                 std::vector<PatternClause *> *patterns,
                 std::vector<WithClause *> *withs,
                 std::vector<ResultClause *> *return_entities);
  std::vector<std::shared_ptr<ClauseGroup>> CreateOptimizedGroupings();
  std::vector<std::shared_ptr<ClauseGroup>> CreateDefaultGroupings();

 private:
  std::vector<SuchThatClause *> *relationships_;
  std::vector<PatternClause *> *patterns_;
  std::vector<WithClause *> *withs_;
  std::vector<ResultClause *> *return_entities_;
  std::vector<ClauseVertex> MakeSuchThatVertices(std::vector<SuchThatClause *> *such_that_clauses);
  std::vector<ClauseVertex> MakePatternVertices(std::vector<PatternClause *> *pattern_clauses);
  std::vector<ClauseVertex> MakeWithVertices(std::vector<WithClause *> *with_clauses);
  std::vector<ClauseVertex> CombineAllVertices(std::vector<ClauseVertex> such_that_vertices,
                                               std::vector<ClauseVertex> pattern_vertices,
                                               std::vector<ClauseVertex> with_vertices);
  std::shared_ptr<ClauseGroup> MakeNoReturnSynGroup(
      std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause,
      std::unordered_map<int, bool> *has_visited_clause);
  int AssignPriority(std::vector<std::string> synonyms_used,
                     std::shared_ptr<Clause> clause);
  void FindConnectedGroups(ClauseGroup *clause_group,
                           std::unordered_map<std::string, std::vector<ClauseVertex>> syn_to_clause,
                           std::unordered_map<std::string, bool> *has_visited_syn,
                           std::unordered_map<int, bool> *has_visited_clause,
                           std::string curr_syn);
  bool ReturnEntitiesContainSynonym(std::string s);
  std::vector<std::shared_ptr<ClauseGroup>> MakeClauseGroupings(
      std::unordered_map<std::string, std::vector<ClauseVertex>> *vertices_to_iterate,
      std::unordered_map<std::string, std::vector<ClauseVertex>> *syn_to_clause,
      std::unordered_map<std::string, bool> *has_visited_syn,
      std::unordered_map<int, bool> *has_visited_clause);
};
