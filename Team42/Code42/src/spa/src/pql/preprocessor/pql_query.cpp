#include <string>
#include <vector>

#include "pql_query.h"

PQLQuery::PQLQuery(std::vector<ResultClause *> *return_entities,
                   std::vector<std::shared_ptr<ClauseGroup>> clause_groups,
                   std::unordered_map<std::string, EntityDeclaration *>
                   *synonym_to_entities,
                   bool is_valid_query) {
  this->entities_ = return_entities;
  this->clause_groups_ = clause_groups;
  this->synonym_to_entities_ = synonym_to_entities;
  this->is_valid_query_ = is_valid_query;
}

PQLQuery::~PQLQuery() = default;

std::vector<ResultClause *> * PQLQuery::get_query_entities() {
  return this->entities_;
}

std::vector<std::shared_ptr<ClauseGroup>> PQLQuery::get_clause_groups() {
  return this->clause_groups_;
}

std::unordered_map<std::string, EntityDeclaration *> *PQLQuery::get_synonym_to_entities() {
  return this->synonym_to_entities_;
}

bool PQLQuery::is_valid_query() {
  return this->is_valid_query_;
}
