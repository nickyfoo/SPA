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

class QueryOptimizer {
 public:
  QueryOptimizer(std::vector<SuchThatClause *> *relationships,
                 std::vector<PatternClause *> *patterns,
                 std::vector<WithClause *> *withs,
                 std::vector<std::string> *return_entities);
  std::vector<std::shared_ptr<ClauseGroup>> CreateGroupings();

 private:
  std::vector<SuchThatClause *> *relationships_;
  std::vector<PatternClause *> *patterns_;
  std::vector<WithClause *> *withs_;
  std::vector<std::string> *return_entities_;
  ClauseVertex MakeSuchThatVertex(SuchThatClause *such_that_clause);
  ClauseVertex MakePatternVertex(PatternClause *pattern_clause);
  ClauseVertex MakeWithVertex(WithClause *with_clause);
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
};
