#pragma once
#include <unordered_map>
#include "EntityDeclaration.h"
#include "Pattern.h"
#include "SuchThatClause.h"

class PQLQuery {
public:
    PQLQuery(std::vector<std::string> *return_entities, std::vector<SuchThatClause*> *relationships,
             std::vector<Pattern*> *patterns, std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entities);
    std::vector<std::string> *getQueryEntities();
    std::vector<SuchThatClause*> *getQueryRelationships();
    std::vector<Pattern*> *getQueryPatterns();
    std::unordered_map<std::string, EntityDeclaration*> *getSynonymToEntities();
private:
    std::vector<std::string> *entities;
    std::vector<SuchThatClause*> *relationships;
    std::vector<Pattern*> *patterns;
    std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entities;
};