#include "clause_group.h"

#include <utility>

ClauseGroup::ClauseGroup() = default;

ClauseGroup::~ClauseGroup() = default;

std::vector<ClauseVertex> ClauseGroup::get_clauses() {
  return this->clauses_;
}

void ClauseGroup::AddSynUsed(std::string syn) {
  this->synonyms_used_.push_back(syn);
}

std::vector<std::string> ClauseGroup::get_syn_used() {
  return this->synonyms_used_;
}

void ClauseGroup::AddClause(ClauseVertex clause_vertex) {
  this->clauses_.push_back(clause_vertex);
}

void ClauseGroup::set_clauses(std::vector<ClauseVertex> clauses) {
  this->clauses_ = clauses;
}
