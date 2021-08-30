#pragma once
#include "Entity.h"
#include "Relationship.h"

using namespace std;

class PQLQuery {
public:
    PQLQuery(vector<Entity*> *entities, vector<Relationship*> *relationship);
    void setQueryEntities(vector<Entity*> *entities);
    void setQueryRelationships(vector<Relationship*> *relationships);
    vector<Entity*> *getQueryEntities();
    vector<Relationship*> *getQueryRelationships();
private:
    vector<Entity*> *entities;
    vector<Relationship*> *relationships;

};