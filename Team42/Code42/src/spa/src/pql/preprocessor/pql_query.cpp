#include <string>
#include <vector>

#include "pql_query.h"

PQLQuery::PQLQuery(std::vector<std::string> *return_entities,
                   std::vector<SuchThatClause *> *relationships,
                   std::vector<PatternClause *> *patterns,
                   std::unordered_map<std::string, EntityDeclaration *>
                           *synonym_to_entities,
                   bool has_one_repeated_synonym, bool has_two_repeated_synonyms) {
  this->entities_ = return_entities;
  this->relationships_ = relationships;
  this->patterns_ = patterns;
  this->synonym_to_entities_ = synonym_to_entities;
  this->has_one_repeated_synonym_ = has_one_repeated_synonym;
  this->has_two_repeated_synonyms_ = has_two_repeated_synonyms;
}

PQLQuery::~PQLQuery() = default;


std::vector<std::string> *PQLQuery::get_query_entities() {
    return this->entities_;
}

std::vector<SuchThatClause *> *PQLQuery::get_query_relationships() {
    return this->relationships_;
}

std::vector<PatternClause *> *PQLQuery::get_query_patterns() {
    return this->patterns_;
}

std::unordered_map<std::string, EntityDeclaration *> *PQLQuery::get_synonym_to_entities() {
  return this->synonym_to_entities_;
}

bool PQLQuery::has_one_repeated_synonym() {
  return this->has_one_repeated_synonym_;
}

bool PQLQuery::has_two_repeated_synonyms() {
  return this->has_two_repeated_synonyms_;
}