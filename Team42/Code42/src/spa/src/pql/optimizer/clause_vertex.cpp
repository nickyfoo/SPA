#include "clause_vertex.h"

ClauseVertex::ClauseVertex(std::vector<std::string> synonyms_used,
                           bool has_return_syn,
                           std::shared_ptr<Clause> clause) {
  this->synonyms_used_ = synonyms_used;
  this->priority_ = 0;
  this->has_return_syn_ = has_return_syn;
  this->clause_ = clause;
  this->id_ = 0;
}

ClauseVertex::~ClauseVertex() = default;

std::shared_ptr<Clause> ClauseVertex::get_clause() {
  return this->clause_;
}

bool ClauseVertex::get_has_return_syn() {
  return this->has_return_syn_;
}

std::vector<std::string> ClauseVertex::get_synonyms_used() {
  return this->synonyms_used_;
}

void ClauseVertex::set_id(int id) {
  this->id_ = id;
}

int ClauseVertex::get_id() {
  return this->id_;
}

void ClauseVertex::set_priority(int priority) {
  this->priority_ = priority;
}

int ClauseVertex::get_priority() const {
  return this->priority_;
}

bool operator<(const ClauseVertex &lhs, const ClauseVertex &rhs) {
  return lhs.get_priority() < rhs.get_priority();
}
