#pragma once
#include <unordered_map>
#include "Entity.h"
#include "Pattern.h"
#include "Relationship.h"

class PQLQuery {
public:
    PQLQuery(std::vector<Entity*> *return_entities, std::vector<Relationship*> *relationships,
             std::vector<Pattern*> *patterns, std::unordered_map<std::string, Entity*> *synonym_to_entities);
    std::vector<Entity*> *getQueryEntities();
    std::vector<Relationship*> *getQueryRelationships();
    std::vector<Pattern*> *getQueryPatterns();
    std::unordered_map<std::string, Entity*> *getSynonymToEntities();
private:
    std::vector<Entity*> *entities;
    std::vector<Relationship*> *relationships;
    std::vector<Pattern*> *patterns;
    std::unordered_map<std::string, Entity*> *synonym_to_entities;
};