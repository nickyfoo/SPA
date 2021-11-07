#include <string>
#include <vector>

#include "pql_query.h"

PQLQuery::PQLQuery(std::vector<ResultClause *> *return_entities,
                   std::vector<std::shared_ptr<ClauseGroup>> clause_groups,
                   std::unordered_map<std::string, EntityDeclaration *>
                   *synonym_to_entities,
                   bool is_syntactically_valid,
                   bool is_semantically_valid) {
  this->entities_ = return_entities;
  this->clause_groups_ = clause_groups;
  this->synonym_to_entities_ = synonym_to_entities;
  this->is_syntactically_valid_ = is_syntactically_valid;
  this->is_semantically_valid_ = is_semantically_valid;
}

PQLQuery::~PQLQuery() = default;

std::vector<ResultClause *> *PQLQuery::get_query_entities() {
  return this->entities_;
}

std::vector<std::shared_ptr<ClauseGroup>> PQLQuery::get_clause_groups() {
  return this->clause_groups_;
}

std::unordered_map<std::string, EntityDeclaration *> *PQLQuery::get_synonym_to_entities() {
  return this->synonym_to_entities_;
}

bool PQLQuery::is_syntactically_valid() {
  return this->is_syntactically_valid_;
}

bool PQLQuery::is_semantically_valid() {
  return this->is_semantically_valid_;
}

