#include <string>
#include <vector>

#include "pql_query.h"

PQLQuery::PQLQuery(std::vector<std::string> *return_entities,
                   std::vector<std::shared_ptr<ClauseGroup>> clause_groups,
                   std::unordered_map<std::string, EntityDeclaration *>
                   *synonym_to_entities) {
  this->entities_ = return_entities;
  this->clause_groups_ = clause_groups;
  this->synonym_to_entities_ = synonym_to_entities;
}

PQLQuery::~PQLQuery() = default;

std::vector<std::string> *PQLQuery::get_query_entities() {
  return this->entities_;
}

std::vector<std::shared_ptr<ClauseGroup>> PQLQuery::get_clause_groups() {
  return this->clause_groups_;
}

std::unordered_map<std::string, EntityDeclaration *> *PQLQuery::get_synonym_to_entities() {
  return this->synonym_to_entities_;
}
