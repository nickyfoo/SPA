#include <string>
#include <vector>

#include "pql_query.h"

PQLQuery::PQLQuery(std::vector<std::string> *return_entities,
                   std::vector<SuchThatClause *> *relationships,
                   std::vector<PatternClause *> *patterns,
                   std::unordered_map<std::string, EntityDeclaration *>
                           *synonym_to_entities) {
  this->entities_ = return_entities;
  this->relationships_ = relationships;
  this->patterns_ = patterns;
  this->synonym_to_entities_ = synonym_to_entities;
}

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
