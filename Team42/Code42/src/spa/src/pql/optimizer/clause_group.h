#pragma once

#include "vector"
#include "string"
#include "unordered_map"
#include "unordered_set"
#include "queue"
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
  bool get_has_return_syn();
  bool set_has_return_syn(bool new_bool);
  ~ClauseGroup();

 private:
  std::vector<ClauseVertex> clauses_;
  std::vector<std::string> synonyms_used_;
  bool has_return_syn_;
};
