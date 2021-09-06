#include "PQLQuery.h"

PQLQuery::PQLQuery(std::vector<std::string> *return_entities, std::vector<SuchThatClause*> *relationships,
                   std::vector<Pattern*> *patterns, std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entities) {
    this->entities = return_entities;
    this->relationships = relationships;
    this->patterns = patterns;
    this->synonym_to_entities = synonym_to_entities;
}

std::vector<std::string> *PQLQuery::getQueryEntities() {
    return this->entities;
}

std::vector<SuchThatClause*> *PQLQuery::getQueryRelationships() {
    return this->relationships;
}

std::vector<Pattern*> *PQLQuery::getQueryPatterns() {
    return this->patterns;
}

std::unordered_map<std::string, EntityDeclaration*> *PQLQuery::getSynonymToEntities() {
    return this->synonym_to_entities;
}