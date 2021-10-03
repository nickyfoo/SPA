#pragma once

#include "vector"
#include "string"
#include "clause.hpp"

class ClauseVertex {
 public:
  ClauseVertex(std::vector<std::string> synonyms_used,
               int priority,
               bool has_return_syn,
               std::shared_ptr<Clause> clause);
  std::vector<std::string> get_synonyms_used();
  bool get_has_return_syn();
  std::shared_ptr<Clause> get_clause();
  void set_id(int id);
  int get_id();
  ~ClauseVertex();
 private:
  std::vector<std::string> synonyms_used_;
  int id_;
  int priority_;
  bool has_return_syn_;
  std::shared_ptr<Clause> clause_;
};