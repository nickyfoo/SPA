#pragma once

#include "vector"
#include "string"
#include <memory>
#include "clause.hpp"

class ClauseVertex {
 public:
  ClauseVertex(std::vector<std::string> synonyms_used,
               bool has_return_syn,
               std::shared_ptr<Clause> clause);
  std::vector<std::string> get_synonyms_used();
  bool get_has_return_syn();
  std::shared_ptr<Clause> get_clause();
  void set_id(int id);
  int get_id();
  void set_priority(int priority);
  int get_priority() const;
  ~ClauseVertex();
 private:
  friend bool operator<(const ClauseVertex &lhs, const ClauseVertex &rhs);
  std::vector<std::string> synonyms_used_;
  int id_;
  int priority_;
  bool has_return_syn_;
  std::shared_ptr<Clause> clause_;
};
