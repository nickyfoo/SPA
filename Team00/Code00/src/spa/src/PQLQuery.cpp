#include "PQLQuery.h"

PQLQuery::PQLQuery(std::vector<EntityDeclaration*> *return_entities, std::vector<Relationship*> *relationships,
                   std::vector<Pattern*> *patterns, std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entities) {
    this->entities = return_entities;
    this->relationships = relationships;
    this->patterns = patterns;
    this->synonym_to_entities = synonym_to_entities;
}

std::vector<EntityDeclaration*> *PQLQuery::getQueryEntities() {
    return this->entities;
}

std::vector<Relationship*> *PQLQuery::getQueryRelationships() {
    return this->relationships;
}

std::vector<Pattern*> *PQLQuery::getQueryPatterns() {
    return this->patterns;
}

std::unordered_map<std::string, EntityDeclaration*> *PQLQuery::getSynonymToEntities() {
    return this->synonym_to_entities;
}