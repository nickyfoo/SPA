#pragma once
#include <unordered_map>
#include "EntityDeclaration.h"
#include "Pattern.h"
#include "Relationship.h"

class PQLQuery {
public:
    PQLQuery(std::vector<EntityDeclaration*> *return_entities, std::vector<Relationship*> *relationships,
             std::vector<Pattern*> *patterns, std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entities);
    std::vector<EntityDeclaration*> *getQueryEntities();
    std::vector<Relationship*> *getQueryRelationships();
    std::vector<Pattern*> *getQueryPatterns();
    std::unordered_map<std::string, EntityDeclaration*> *getSynonymToEntities();
private:
    std::vector<EntityDeclaration*> *entities;
    std::vector<Relationship*> *relationships;
    std::vector<Pattern*> *patterns;
    std::unordered_map<std::string, EntityDeclaration*> *synonym_to_entities;
};