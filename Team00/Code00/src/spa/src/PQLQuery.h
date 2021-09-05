#pragma once
#include <unordered_map>
#include "Entity.h"
#include "Pattern.h"
#include "Relationship.h"

using namespace std;

class PQLQuery {
public:
    PQLQuery(vector<Entity*> *return_entities, vector<Relationship*> *relationships, vector<Pattern*> *patterns,
             unordered_map<string, Entity*> *synonym_to_entities);
    void setQueryEntities(vector<Entity*> *entities);
    void setQueryRelationships(vector<Relationship*> *relationships);
    vector<Entity*> *getQueryEntities();
    vector<Relationship*> *getQueryRelationships();
    vector<Pattern*> *getQueryPatterns();
    unordered_map<string, Entity*> *getSynonymToEntities();
private:
    vector<Entity*> *entities;
    vector<Relationship*> *relationships;
    vector<Pattern*> *patterns;
    unordered_map<string, Entity*> *synonym_to_entities;
};