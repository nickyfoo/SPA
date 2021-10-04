#pragma once

#include "vector"
#include "string"
#include "clause_vertex.h"

class ClauseGroup {
 public:
  ClauseGroup();
  void AddClause(ClauseVertex clause_vertex);
  void set_clauses(std::vector<ClauseVertex> clauses);
  std::vector<ClauseVertex> get_clauses();
  void AddSynUsed(std::string syn);
  std::vector<std::string> get_syn_used();
  void SortWithinGroup();
  ~ClauseGroup();

 private:
  std::vector<ClauseVertex> clauses_;
  std::vector<std::string> synonyms_used_;
};
